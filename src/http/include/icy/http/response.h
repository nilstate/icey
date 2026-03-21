///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "icy/datetime.h"
#include "icy/http/cookie.h"
#include "icy/http/message.h"

#include <sstream>


namespace icy {
namespace http {


/// HTTP Response Status Codes
enum class StatusCode
{
    Continue = 100,
    SwitchingProtocols = 101,

    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritative = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,

    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    // SwitchProxy = 306, not used
    TemporaryRedirect = 307,

    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    EntityTooLarge = 413,
    UriTooLong = 414,
    UnsupportedMediaType = 415,
    RangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    UnprocessableEntity = 422,
    UpgradeRequired = 426,

    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    Unavailable = 503,
    GatewayTimeout = 504,
    VersionNotSupported = 505
};


/// This class encapsulates an HTTP response message.
class HTTP_API Response : public http::Message
{
public:
    /// Creates the Response with OK status.
    Response();

    /// Creates the Response with the given status  and reason phrase.
    Response(StatusCode status, const std::string& reason);

    /// Creates the Response with the given version, status and reason phrase.
    Response(const std::string& version, StatusCode status, const std::string& reason);

    /// Creates the Response with the given status
    /// an an appropriate reason phrase.
    Response(StatusCode status);

    /// Creates the Response with the given version, status
    /// an an appropriate reason phrase.
    Response(const std::string& version, StatusCode status);

    /// Destroys the Response.
    virtual ~Response();

    /// Sets the HTTP status code.
    ///
    /// The reason phrase is set according to the status code.
    void setStatus(StatusCode status);

    /// Returns the HTTP status code.
    [[nodiscard]] StatusCode getStatus() const;

    /// Sets the HTTP reason phrase.
    void setReason(const std::string& reason);

    /// Returns the HTTP reason phrase.
    [[nodiscard]] const std::string& getReason() const;

    /// Sets the HTTP status code and reason phrase.
    void setStatusAndReason(StatusCode status, const std::string& reason);

    /// Sets the Date header to the given date/time value.
    void setDate(const Timestamp& dateTime);

    /// Returns the value of the Date header.
    [[nodiscard]] Timestamp getDate() const;

    /// Adds the cookie to the response by
    /// adding a Set-Cookie header.
    void addCookie(const Cookie& cookie);

    /// Returns a vector with all the cookies set in the response header.
    ///
    /// May throw an exception in case of a malformed Set-Cookie header.
    void getCookies(std::vector<Cookie>& cookies) const;

    /// Writes the HTTP response headers to the given output stream.
    virtual void write(std::ostream& ostr) const;

    /// Writes the HTTP response headers to the given output string.
    virtual void write(std::string& str) const;

    /// Returns true if the HTTP response code was successful (< 400).
    [[nodiscard]] virtual bool success() const;

    friend std::ostream& operator<<(std::ostream& stream, const Response& res)
    {
        res.write(stream);
        return stream;
    }

private:
    StatusCode _status;
    std::string _reason;
};


/// Returns the standard reason phrase for the given HTTP status code
/// (e.g. "OK" for StatusCode::OK, "Not Found" for StatusCode::NotFound).
/// @param status HTTP status code.
/// @return Null-terminated reason phrase string.
[[nodiscard]] const char* getStatusCodeReason(StatusCode status);

/// Returns a combined "NNN Reason" string for the given HTTP status code
/// (e.g. "200 OK").
/// @param status HTTP status code.
/// @return Null-terminated status code string.
[[nodiscard]] const char* getStatusCodeString(StatusCode status);


} // namespace http
} // namespace icy


/// @}
