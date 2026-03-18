///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#include "scy/stun/message.h"
#include "scy/logger.h"
#include "scy/util.h"

#include <sstream>
#include <stdexcept>
#include <utility>


namespace scy {
namespace stun {


Message::Message()
    : _class(Request)
    , _method(Undefined)
    , _size(0)
    , _transactionID(util::randomString(kTransactionIdLength))
{
    if (_transactionID.size() != kTransactionIdLength)
        throw std::runtime_error("Message: invalid transaction ID length");
}


Message::Message(ClassType clss, MethodType meth)
    : _class(clss)
    , _method(meth)
    , _size(0)
    , _transactionID(util::randomString(kTransactionIdLength))
{
}


Message::Message(const Message& that)
    : IPacket()
    , _class(that._class)
    , _method(that._method)
    , _size(that._size)
    , _transactionID(that._transactionID)
{
    if (!_method)
        throw std::runtime_error("Message copy: invalid method");
    if (_transactionID.size() != kTransactionIdLength)
        throw std::runtime_error("Message copy: invalid transaction ID length");

    // Deep-copy attributes from source object
    for (const auto& attr : that._attrs)
        _attrs.push_back(attr->clone());
}


Message::Message(Message&& that) noexcept
    : _class(that._class)
    , _method(that._method)
    , _size(that._size)
    , _transactionID(std::move(that._transactionID))
    , _attrs(std::move(that._attrs))
{
    that._class = Request;
    that._method = Undefined;
    that._size = 0;
}


Message& Message::operator=(const Message& that)
{
    if (&that != this) {
        _method = that._method;
        _class = that._class;
        _size = that._size;
        _transactionID = that._transactionID;
        if (!_method)
            throw std::runtime_error("Message assign: invalid method");
        if (_transactionID.size() != kTransactionIdLength)
            throw std::runtime_error("Message assign: invalid transaction ID length");

        // Clear and deep-copy attributes from source object
        _attrs.clear();
        for (const auto& attr : that._attrs)
            _attrs.push_back(attr->clone());
    }

    return *this;
}


Message& Message::operator=(Message&& that) noexcept
{
    if (&that != this) {
        _method = that._method;
        _class = that._class;
        _size = that._size;
        _transactionID = std::move(that._transactionID);
        _attrs = std::move(that._attrs);

        that._class = Request;
        that._method = Undefined;
        that._size = 0;
    }

    return *this;
}


Message::~Message()
{
}


std::unique_ptr<IPacket> Message::clone() const
{
    return std::make_unique<Message>(*this);
}


void Message::add(Attribute* attr)
{
    add(std::unique_ptr<Attribute>(attr));
}


void Message::add(std::unique_ptr<Attribute> attr)
{
    size_t attrLength = attr->size();
    if (attrLength % 4 != 0)
        attrLength += (4 - (attrLength % 4));
    _size += attrLength + kAttributeHeaderSize;
    _attrs.push_back(std::move(attr));
}


Attribute* Message::get(Attribute::Type type, int index) const
{
    for (const auto& attr : _attrs) {
        if (attr->type() == type) {
            if (index == 0)
                return attr.get();
            else
                index--;
        }
    }
    return nullptr;
}


ssize_t Message::read(const ConstBuffer& buf)
{
    LTrace("Parse STUN packet: ", buf.size());

    try {
        BitReader reader(buf);

        // Message type
        uint16_t type;
        reader.getU16(type);
        if (type & 0x8000) {
            // RTP and RTCP set MSB of first byte, since first two bits are version,
            // and version is always 2 (10). If set, this is not a STUN packet.
            LWarn("Not STUN packet");
            return 0;
        }

        uint16_t classType = type & 0x0110;
        uint16_t methodType = type & 0x000F;
        if (!isValidMethod(methodType)) {
            LWarn("STUN message unknown method: ", methodType);
            return 0;
        }

        _class = classType;
        _method = methodType;

        // Message length
        reader.getU16(_size);
        if (_size > buf.size()) {
            LWarn("STUN message larger than buffer: ", _size, " > ", buf.size());
            return 0;
        }

        // Magic cookie
        reader.skip(kMagicCookieLength);

        // Transaction ID
        std::string transactionID;
        reader.get(transactionID, kTransactionIdLength);
        if (transactionID.size() != kTransactionIdLength)
            throw std::runtime_error("invalid transaction ID length");
        _transactionID = transactionID;

        // Attributes
        _attrs.clear();
        int rest = _size;
        uint16_t attrType, attrLength, padLength;
        if (static_cast<int>(reader.available()) < rest)
            throw std::runtime_error("insufficient data for attributes");
        while (rest > 0) {
            reader.getU16(attrType);
            reader.getU16(attrLength);
            padLength = attrLength % 4 == 0 ? 0 : 4 - (attrLength % 4);

            auto attr = Attribute::create(attrType, attrLength);
            if (attr) {
                attr->read(reader); // parse or throw
                _attrs.push_back(std::move(attr));
            } else
                SWarn << "Failed to parse attribute: "
                      << Attribute::typeString(attrType) << ": " << attrLength
                     ;

            rest -= (attrLength + kAttributeHeaderSize + padLength);
        }

        LTrace("Parse success: ", reader.position(), ": ", buf.size());
        if (rest != 0)
            throw std::runtime_error("attribute parsing left non-zero remainder");
        if (reader.position() != static_cast<size_t>(_size) + kMessageHeaderSize)
            throw std::runtime_error("reader position mismatch after parsing");
        return reader.position();
    } catch (std::exception& exc) {
        LDebug("Parse error: ", exc.what());
    }

    return 0;
}


void Message::write(Buffer& buf) const
{
    DynamicBitWriter writer(buf);
    writer.putU16(static_cast<uint16_t>(_class | _method));
    writer.putU16(_size);
    writer.putU32(kMagicCookie);
    writer.put(_transactionID.c_str(), _transactionID.size());

    // Note: MessageIntegrity must be at the end

    for (const auto& attr : _attrs) {
        writer.putU16(attr->type());
        writer.putU16(attr->size());
        attr->write(writer);
    }
}


std::string Message::classString() const
{
    switch (_class) {
        case Request:
            return "Request";
        case Indication:
            return "Indication";
        case SuccessResponse:
            return "SuccessResponse";
        case ErrorResponse:
            return "ErrorResponse";
        default:
            return "UnknownState";
    }
}


std::string Message::errorString(uint16_t errorCode) const
{
    switch (errorCode) {
        case BadRequest:
            return "BAD REQUEST";
        case NotAuthorized:
            return "UNAUTHORIZED";
        case UnknownAttribute:
            return "UNKNOWN ATTRIBUTE";
        case StaleCredentials:
            return "STALE CREDENTIALS";
        case IntegrityCheckFailure:
            return "INTEGRITY CHECK FAILURE";
        case MissingUsername:
            return "MISSING USERNAME";
        case UseTLS:
            return "USE TLS";
        case RoleConflict:
            return "Role Conflict"; // (487) rfc5245
        case ServerError:
            return "SERVER ERROR";
        case GlobalFailure:
            return "GLOBAL FAILURE";
        case ConnectionAlreadyExists:
            return "Connection Already Exists";
        case ConnectionTimeoutOrFailure:
            return "Connection Timeout or Failure";
        default:
            return "UnknownError";
    }
}


std::string Message::methodString() const
{
    switch (_method) {
        case Binding:
            return "BINDING";
        case Allocate:
            return "ALLOCATE";
        case Refresh:
            return "REFRESH";
        case SendIndication:
            return "SEND-INDICATION";
        case DataIndication:
            return "DATA-INDICATION";
        case CreatePermission:
            return "CREATE-PERMISSION";
        case ChannelBind:
            return "CHANNEL-BIND";
        case Connect:
            return "CONNECT";
        case ConnectionBind:
            return "CONNECTION-BIND";
        case ConnectionAttempt:
            return "CONNECTION-ATTEMPT";
        default:
            return "UnknownMethod";
    }
}


std::string Message::toString() const
{
    std::ostringstream os;
    os << "STUN[" << methodString() << ":" << transactionID();
    for (const auto& attr : _attrs)
        os << ":" << attr->typeString();
    os << "]";
    return os.str();
}


void Message::print(std::ostream& os) const
{
    os << "STUN[" << methodString() << ":" << transactionID();
    for (const auto& attr : _attrs)
        os << ":" << attr->typeString();
    os << "]";
}


void Message::setTransactionID(const std::string& id)
{
    if (id.size() != kTransactionIdLength)
        throw std::runtime_error("Message::setTransactionID: invalid length");
    _transactionID = id;
}


Message::ClassType Message::classType() const
{
    return static_cast<ClassType>(_class);
}


Message::MethodType Message::methodType() const
{
    return static_cast<MethodType>(_method);
}


void Message::setClass(ClassType type)
{
    _class = type;
}

void Message::setMethod(MethodType type)
{
    _method = type;
}
} // namespace stun
} // namespace scy


/// @\}
