///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#include "icy/stun/message.h"
#include "icy/logger.h"
#include "icy/util.h"

#include <sstream>
#include <stdexcept>
#include <utility>


namespace icy {
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


void Message::add(std::unique_ptr<Attribute> attr)
{
    _attrs.push_back(std::move(attr));
}


uint16_t Message::computeBodySize() const
{
    uint16_t size = 0;
    for (const auto& attr : _attrs)
        size += static_cast<uint16_t>(attr->paddedBytes() + kAttributeHeaderSize);
    return size;
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

        // Message type - RFC 5389 section 6:
        // The message type field is decomposed as:
        //   M11..M7 | C1 | M6..M4 | C0 | M3..M0
        // where M = method bits, C = class bits.
        uint16_t type;
        reader.getU16(type);
        if (type & 0xC000) {
            // First two bits must be zero for STUN (RFC 5389 section 6).
            // RTP/RTCP sets MSB since version is always 2 (10).
            LWarn("Not STUN packet");
            return 0;
        }

        // Extract class: C1 is bit 8, C0 is bit 4
        uint16_t classType = ((type >> 7) & 0x02) | ((type >> 4) & 0x01);
        // Reconstruct the class in our internal format (C1C0 shifted to bits 8,4)
        classType = ((classType & 0x02) << 7) | ((classType & 0x01) << 4);

        // Extract method: M11..M7 from bits 14..9, M6..M4 from bits 7..5, M3..M0 from bits 3..0
        uint16_t methodType = ((type >> 2) & 0x0F80) |
                               ((type >> 1) & 0x0070) |
                               (type & 0x000F);
        if (!isValidMethod(methodType)) {
            LWarn("STUN message unknown method: ", methodType);
            return 0;
        }

        _class = classType;
        _method = methodType;

        // Message length (body size, not including the 20-byte header)
        reader.getU16(_size);
        if (_size % 4 != 0) {
            LWarn("STUN message length not 4-byte aligned: ", _size);
            return 0;
        }
        if (static_cast<size_t>(_size) + kMessageHeaderSize > buf.size()) {
            LWarn("STUN message larger than buffer: ", _size + kMessageHeaderSize, " > ", buf.size());
            return 0;
        }

        // Magic cookie - RFC 5389 section 6: fixed value 0x2112A442
        uint32_t cookie;
        reader.getU32(cookie);
        if (cookie != kMagicCookie) {
            LWarn("Invalid STUN magic cookie: ", cookie);
            return 0;
        }

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
            if (rest < kAttributeHeaderSize)
                throw std::runtime_error("truncated STUN attribute header");

            reader.getU16(attrType);
            reader.getU16(attrLength);
            padLength = Attribute::paddingBytes(attrLength);
            auto wireLength = static_cast<int>(attrLength + kAttributeHeaderSize + padLength);
            if (wireLength > rest)
                throw std::runtime_error("attribute length exceeds remaining message body");

            auto attr = Attribute::create(attrType, attrLength);
            if (attr) {
                attr->read(reader); // parse or throw
                _attrs.push_back(std::move(attr));
            } else {
                // Skip unknown attribute body + padding
                reader.skip(attrLength + padLength);
                SWarn << "Skipping unknown attribute: "
                      << Attribute::typeString(attrType) << ": " << attrLength
                     ;
            }

            rest -= wireLength;
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
    // Encode message type per RFC 5389 section 6:
    //   M11..M7 | C1 | M6..M4 | C0 | M3..M0
    uint16_t c1 = (_class >> 7) & 0x02;  // C1 from bit 8
    uint16_t c0 = (_class >> 4) & 0x01;  // C0 from bit 4
    uint16_t type = ((_method & 0x0F80) << 2) |
                    (c1 << 7) |
                    ((_method & 0x0070) << 1) |
                    (c0 << 4) |
                    (_method & 0x000F);
    writer.putU16(type);
    writer.putU16(computeBodySize());
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
        case TryAlternate:
            return "Try Alternate";
        case BadRequest:
            return "Bad Request";
        case NotAuthorized:
            return "Unauthorized";
        case Forbidden:
            return "Forbidden";
        case UnknownAttribute:
            return "Unknown Attribute";
        case StaleCredentials:
            return "Stale Credentials";
        case IntegrityCheckFailure:
            return "Integrity Check Failure";
        case MissingUsername:
            return "Missing Username";
        case UseTLS:
            return "Use TLS";
        case AllocationMismatch:
            return "Allocation Mismatch";
        case StaleNonce:
            return "Stale Nonce";
        case WrongCredentials:
            return "Wrong Credentials";
        case UnsupportedTransport:
            return "Unsupported Transport Protocol";
        case AllocationQuotaReached:
            return "Allocation Quota Reached";
        case RoleConflict:
            return "Role Conflict";
        case ServerError:
            return "Server Error";
        case InsufficientCapacity:
            return "Insufficient Capacity";
        case GlobalFailure:
            return "Global Failure";
        case ConnectionAlreadyExists:
            return "Connection Already Exists";
        case ConnectionTimeoutOrFailure:
            return "Connection Timeout or Failure";
        default:
            return "Unknown Error";
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
} // namespace icy


/// @\}
