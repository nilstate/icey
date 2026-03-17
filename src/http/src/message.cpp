///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/message.h"


namespace scy {
namespace http {


const std::string Message::HTTP_1_0 = "HTTP/1.0";
const std::string Message::HTTP_1_1 = "HTTP/1.1";
const std::string Message::IDENTITY_TRANSFER_ENCODING = "identity";
const std::string Message::CHUNKED_TRANSFER_ENCODING = "chunked";
const int Message::UNKNOWN_CONTENT_LENGTH = -1;
const std::string Message::UNKNOWN_CONTENT_TYPE;
const std::string Message::CONTENT_LENGTH = "Content-Length";
const std::string Message::CONTENT_TYPE = "Content-Type";
const std::string Message::TRANSFER_ENCODING = "Transfer-Encoding";
const std::string Message::CONNECTION = "Connection";
const std::string Message::CONNECTION_KEEP_ALIVE = "Keep-Alive";
const std::string Message::CONNECTION_CLOSE = "Close";
const std::string Message::EMPTY;


Message::Message()
    : _version(HTTP_1_1)
{
}


Message::Message(const std::string& version)
    : _version(version)
{
}


Message::~Message()
{
}


void Message::setVersion(const std::string& version)
{
    _version = version;
}


void Message::setContentLength(uint64_t length)
{
    if (static_cast<int>(length) != UNKNOWN_CONTENT_LENGTH)
        set(CONTENT_LENGTH, util::itostr<uint64_t>(length));
    else
        erase(CONTENT_LENGTH);
}


uint64_t Message::getContentLength() const
{
    const std::string& contentLength = get(CONTENT_LENGTH, EMPTY);
    if (!contentLength.empty()) {
        return util::strtoi<uint64_t>(contentLength);
    } else
        return static_cast<uint64_t>(UNKNOWN_CONTENT_LENGTH);
}


void Message::setTransferEncoding(const std::string& transferEncoding)
{
    if (util::icompare(transferEncoding, IDENTITY_TRANSFER_ENCODING) == 0)
        erase(TRANSFER_ENCODING);
    else
        set(TRANSFER_ENCODING, transferEncoding);
}


const std::string& Message::getTransferEncoding() const
{
    return get(TRANSFER_ENCODING, IDENTITY_TRANSFER_ENCODING);
}


void Message::setChunkedTransferEncoding(bool flag)
{
    if (flag)
        setTransferEncoding(CHUNKED_TRANSFER_ENCODING);
    else
        setTransferEncoding(IDENTITY_TRANSFER_ENCODING);
}


bool Message::isChunkedTransferEncoding() const
{
    return util::icompare(getTransferEncoding(), CHUNKED_TRANSFER_ENCODING) == 0;
}


void Message::setContentType(const std::string& contentType)
{
    if (contentType.empty())
        erase(CONTENT_TYPE);
    else
        set(CONTENT_TYPE, contentType);
}


const std::string& Message::getContentType() const
{
    return get(CONTENT_TYPE, UNKNOWN_CONTENT_TYPE);
}


void Message::setKeepAlive(bool keepAlive)
{
    if (keepAlive)
        set(CONNECTION, CONNECTION_KEEP_ALIVE);
    else
        set(CONNECTION, CONNECTION_CLOSE);
}


bool Message::getKeepAlive() const
{
    const std::string& connection = get(CONNECTION, EMPTY);
    if (!connection.empty())
        return util::icompare(connection, CONNECTION_CLOSE) != 0;
    else
        return getVersion() == HTTP_1_1;
}


const std::string& Message::getVersion() const
{
    return _version;
}


bool Message::hasContentLength() const
{
    return has(CONTENT_LENGTH);
}


void Message::write(std::ostream& ostr) const
{
    for (const auto& [name, value] : *this) {
        ostr << name << ": " << value << "\r\n";
    }
}


void Message::write(std::string& str) const
{
    for (const auto& [name, value] : *this) {
        str.append(name);
        str.append(": ");
        str.append(value);
        str.append("\r\n");
    }
}


} // namespace http
} // namespace scy


/// @\}
