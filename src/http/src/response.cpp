///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/response.h"
#include "icy/datetime.h"
#include "icy/http/util.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>


namespace icy {
namespace http {


Response::Response()
    : _status(StatusCode::OK)
    , _reason(getStatusCodeReason(StatusCode::OK))
{
}


Response::Response(StatusCode status, const std::string& reason)
    : _status(status)
    , _reason(reason)
{
}


Response::Response(const std::string& version, StatusCode status,
                   const std::string& reason)
    : http::Message(version)
    , _status(status)
    , _reason(reason)
{
}


Response::Response(StatusCode status)
    : _status(status)
    , _reason(getStatusCodeReason(status))
{
}


Response::Response(const std::string& version, StatusCode status)
    : http::Message(version)
    , _status(status)
    , _reason(getStatusCodeReason(status))
{
}


Response::~Response()
{
}


void Response::setStatus(StatusCode status)
{
    _status = status;
    _reason = getStatusCodeReason(status);
}


void Response::setReason(const std::string& reason)
{
    _reason = reason;
}


void Response::setStatusAndReason(StatusCode status, const std::string& reason)
{
    _status = status;
    _reason = reason;
}


void Response::setDate(const Timestamp& dateTime)
{
    set("Date", DateTimeFormatter::format(dateTime, DateTimeFormat::HTTP_FORMAT));
}


Timestamp Response::getDate() const
{
    const std::string& dateTime = get("Date");
    int tzd;
    return DateTimeParser::parse(dateTime, tzd).timestamp();
}


void Response::addCookie(const Cookie& cookie)
{
    add("Set-Cookie", cookie.toString());
}


void Response::getCookies(std::vector<Cookie>& cookies) const
{
    cookies.clear();
    for (const auto& [name, value] : *this) {
        if (util::icompare(name, "Set-Cookie") == 0) {
            NVCollection nvc;
            http::splitParameters(value.begin(), value.end(), nvc);
            cookies.push_back(Cookie(nvc));
        }
    }
}


void Response::write(std::ostream& ostr) const
{
    ostr << _version << " " << static_cast<int>(_status) << " " << _reason << "\r\n";
    http::Message::write(ostr);
    ostr << "\r\n";
}


namespace {

struct StatusLine { const char* data; size_t len; };

/// Pre-formatted HTTP/1.1 status lines for common codes.
/// Avoids per-response string concatenation for the status line.
const StatusLine* getStatusLine(StatusCode status)
{
    // Most common codes as static constexpr data
    static const StatusLine ok = {"HTTP/1.1 200 OK\r\n", 17};
    static const StatusLine created = {"HTTP/1.1 201 Created\r\n", 22};
    static const StatusLine noContent = {"HTTP/1.1 204 No Content\r\n", 25};
    static const StatusLine movedPerm = {"HTTP/1.1 301 Moved Permanently\r\n", 32};
    static const StatusLine found = {"HTTP/1.1 302 Found\r\n", 20};
    static const StatusLine notModified = {"HTTP/1.1 304 Not Modified\r\n", 27};
    static const StatusLine badRequest = {"HTTP/1.1 400 Bad Request\r\n", 26};
    static const StatusLine unauthorized = {"HTTP/1.1 401 Unauthorized\r\n", 27};
    static const StatusLine forbidden = {"HTTP/1.1 403 Forbidden\r\n", 24};
    static const StatusLine notFound = {"HTTP/1.1 404 Not Found\r\n", 24};
    static const StatusLine serverError = {"HTTP/1.1 500 Internal Server Error\r\n", 36};
    static const StatusLine unavailable = {"HTTP/1.1 503 Service Unavailable\r\n", 34};

    switch (status) {
        case StatusCode::OK: return &ok;
        case StatusCode::Created: return &created;
        case StatusCode::NoContent: return &noContent;
        case StatusCode::MovedPermanently: return &movedPerm;
        case StatusCode::Found: return &found;
        case StatusCode::NotModified: return &notModified;
        case StatusCode::BadRequest: return &badRequest;
        case StatusCode::Unauthorized: return &unauthorized;
        case StatusCode::Forbidden: return &forbidden;
        case StatusCode::NotFound: return &notFound;
        case StatusCode::InternalServerError: return &serverError;
        case StatusCode::Unavailable: return &unavailable;
        default: return nullptr;
    }
}

} // anonymous namespace


void Response::write(std::string& str) const
{
    // Fast path: use pre-formatted status line for HTTP/1.1 + common codes
    const StatusLine* sl = (_version == Message::HTTP_1_1) ? getStatusLine(_status) : nullptr;

    // Compute total size for headers + trailing CRLF
    size_t total = 0;
    if (sl) {
        total = sl->len;
    } else {
        const char* code = getStatusCodeString(_status);
        total = _version.size() + 1 + (code ? std::strlen(code) : 3) + 1 + _reason.size() + 2;
    }
    for (const auto& [name, value] : *this) {
        total += name.size() + 2 + value.size() + 2;
    }
    total += 2; // trailing \r\n

    str.reserve(str.size() + total);

    if (sl) {
        str.append(sl->data, sl->len);
    } else {
        str.append(_version);
        str.append(" ", 1);
        const char* code = getStatusCodeString(_status);
        if (code) {
            str.append(code, std::strlen(code));
        } else {
            char codeBuf[12];
            auto n = std::snprintf(codeBuf, sizeof(codeBuf), "%d", static_cast<int>(_status));
            str.append(codeBuf, static_cast<size_t>(n));
        }
        str.append(" ", 1);
        str.append(_reason);
        str.append("\r\n", 2);
    }
    http::Message::write(str);
    str.append("\r\n", 2);
}


void Response::write(Buffer& buf) const
{
    const StatusLine* sl = (_version == Message::HTTP_1_1) ? getStatusLine(_status) : nullptr;

    size_t total = 0;
    if (sl) {
        total = sl->len;
    } else {
        const char* code = getStatusCodeString(_status);
        total = _version.size() + 1 + (code ? std::strlen(code) : 3) + 1 + _reason.size() + 2;
    }
    for (const auto& [name, value] : *this) {
        total += name.size() + 2 + value.size() + 2;
    }
    total += 2;

    buf.reserve(buf.size() + total);

    if (sl) {
        buf.insert(buf.end(), sl->data, sl->data + sl->len);
    } else {
        buf.insert(buf.end(), _version.begin(), _version.end());
        buf.push_back(' ');
        const char* code = getStatusCodeString(_status);
        if (code) {
            buf.insert(buf.end(), code, code + std::strlen(code));
        } else {
            char codeBuf[12];
            auto n = std::snprintf(codeBuf, sizeof(codeBuf), "%d", static_cast<int>(_status));
            buf.insert(buf.end(), codeBuf, codeBuf + n);
        }
        buf.push_back(' ');
        buf.insert(buf.end(), _reason.begin(), _reason.end());
        buf.push_back('\r');
        buf.push_back('\n');
    }
    http::Message::write(buf);
    buf.push_back('\r');
    buf.push_back('\n');
}


bool Response::success() const
{
    return getStatus() < StatusCode::BadRequest; // < 400
}


StatusCode Response::getStatus() const
{
    return _status;
}


const std::string& Response::getReason() const
{
    return _reason;
}


const char* getStatusCodeString(StatusCode status)
{
    switch (status) {
        case StatusCode::Continue: return "100";
        case StatusCode::SwitchingProtocols: return "101";
        case StatusCode::OK: return "200";
        case StatusCode::Created: return "201";
        case StatusCode::Accepted: return "202";
        case StatusCode::NonAuthoritative: return "203";
        case StatusCode::NoContent: return "204";
        case StatusCode::ResetContent: return "205";
        case StatusCode::PartialContent: return "206";
        case StatusCode::MultipleChoices: return "300";
        case StatusCode::MovedPermanently: return "301";
        case StatusCode::Found: return "302";
        case StatusCode::SeeOther: return "303";
        case StatusCode::NotModified: return "304";
        case StatusCode::UseProxy: return "305";
        case StatusCode::TemporaryRedirect: return "307";
        case StatusCode::BadRequest: return "400";
        case StatusCode::Unauthorized: return "401";
        case StatusCode::PaymentRequired: return "402";
        case StatusCode::Forbidden: return "403";
        case StatusCode::NotFound: return "404";
        case StatusCode::MethodNotAllowed: return "405";
        case StatusCode::NotAcceptable: return "406";
        case StatusCode::ProxyAuthRequired: return "407";
        case StatusCode::RequestTimeout: return "408";
        case StatusCode::Conflict: return "409";
        case StatusCode::Gone: return "410";
        case StatusCode::LengthRequired: return "411";
        case StatusCode::PreconditionFailed: return "412";
        case StatusCode::EntityTooLarge: return "413";
        case StatusCode::UriTooLong: return "414";
        case StatusCode::UnsupportedMediaType: return "415";
        case StatusCode::RangeNotSatisfiable: return "416";
        case StatusCode::ExpectationFailed: return "417";
        case StatusCode::UnprocessableEntity: return "422";
        case StatusCode::UpgradeRequired: return "426";
        case StatusCode::InternalServerError: return "500";
        case StatusCode::NotImplemented: return "501";
        case StatusCode::BadGateway: return "502";
        case StatusCode::Unavailable: return "503";
        case StatusCode::GatewayTimeout: return "504";
        case StatusCode::VersionNotSupported: return "505";
    }
    return nullptr;
}


const char* getStatusCodeReason(StatusCode status)
{
    switch (status) {
        case StatusCode::Continue:
            return "Continue";
        case StatusCode::SwitchingProtocols:
            return "Switching Protocols";

        case StatusCode::OK:
            return "OK";
        case StatusCode::Created:
            return "Created";
        case StatusCode::Accepted:
            return "Accepted";
        case StatusCode::NonAuthoritative:
            return "Non-Authoritative Information";
        case StatusCode::NoContent:
            return "No Content";
        case StatusCode::ResetContent:
            return "Reset Content";
        case StatusCode::PartialContent:
            return "Partial Content";

        // 300 range: redirects
        case StatusCode::MultipleChoices:
            return "Multiple Choices";
        case StatusCode::MovedPermanently:
            return "Moved Permanently";
        case StatusCode::Found:
            return "Found";
        case StatusCode::SeeOther:
            return "See Other";
        case StatusCode::NotModified:
            return "Not Modified";
        case StatusCode::UseProxy:
            return "Use Proxy";
        case StatusCode::TemporaryRedirect:
            return "Temporary Redirect";

        // 400 range: client errors
        case StatusCode::BadRequest:
            return "Bad Request";
        case StatusCode::Unauthorized:
            return "Unauthorized";
        case StatusCode::PaymentRequired:
            return "Payment Required";
        case StatusCode::Forbidden:
            return "Forbidden";
        case StatusCode::NotFound:
            return "Not Found";
        case StatusCode::MethodNotAllowed:
            return "Method Not Allowed";
        case StatusCode::NotAcceptable:
            return "Not Acceptable";
        case StatusCode::ProxyAuthRequired:
            return "Proxy Authentication Required";
        case StatusCode::RequestTimeout:
            return "Request Time-out";
        case StatusCode::Conflict:
            return "Conflict";
        case StatusCode::Gone:
            return "Gone";
        case StatusCode::LengthRequired:
            return "Length Required";
        case StatusCode::PreconditionFailed:
            return "Precondition Failed";
        case StatusCode::EntityTooLarge:
            return "Request Entity Too Large";
        case StatusCode::UriTooLong:
            return "Request-URI Too Large";
        case StatusCode::UnsupportedMediaType:
            return "Unsupported Media Type";
        case StatusCode::RangeNotSatisfiable:
            return "Requested range not satisfiable";
        case StatusCode::ExpectationFailed:
            return "Expectation Failed";
        case StatusCode::UnprocessableEntity:
            return "Unprocessable Entity";
        case StatusCode::UpgradeRequired:
            return "Upgrade Required";

        // 500 range: server errors
        case StatusCode::InternalServerError:
            return "Internal Server Error";
        case StatusCode::NotImplemented:
            return "Not Implemented";
        case StatusCode::BadGateway:
            return "Bad Gateway";
        case StatusCode::Unavailable:
            return "Service Unavailable";
        case StatusCode::GatewayTimeout:
            return "Gateway Time-out";
        case StatusCode::VersionNotSupported:
            return "Version Not Supported";
    }
    return "Unknown";
}


} // namespace http
} // namespace icy


/// @}
