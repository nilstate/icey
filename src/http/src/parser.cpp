///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/parser.h"
#include "scy/crypto/crypto.h"
#include "scy/http/connection.h"
#include "scy/http/util.h"
#include "scy/logger.h"
#include <stdexcept>


using std::endl;


namespace scy {
namespace http {


Parser::Parser(http::Response* response)
    : _observer(nullptr)
    , _request(nullptr)
    , _response(response)
    , _type(HTTP_RESPONSE)
{
    init();
    reset();
}


Parser::Parser(http::Request* request)
    : _observer(nullptr)
    , _request(request)
    , _response(nullptr)
    , _type(HTTP_REQUEST)
{
    init();
    reset();
}


Parser::Parser(http_parser_type type)
    : _observer(nullptr)
    , _request(nullptr)
    , _response(nullptr)
    , _type(type)
{
    init();
    reset();
}


Parser::~Parser()
{
}


void Parser::init()
{
    // LTrace("Init: ", _type)

    ::http_parser_init(&_parser, _type);
    _parser.data = this;
    _settings.on_message_begin = on_message_begin;
    _settings.on_url = on_url;
    _settings.on_status = on_status;
    _settings.on_header_field = on_header_field;
    _settings.on_header_value = on_header_value;
    _settings.on_headers_complete = on_headers_complete;
    _settings.on_body = on_body;
    _settings.on_message_complete = on_message_complete;

    reset();
}


size_t Parser::parse(const char* data, size_t len)
{
    // LTrace("Parse: ", len)

    if (_complete) {
        throw std::runtime_error("HTTP parser already complete");
    }

    size_t nparsed = ::http_parser_execute(&_parser, &_settings, data, len);

    if (_parser.upgrade) {
        // The parser has only parsed the HTTP headers, there
        // may still be unread data from the request body in the buffer.
    }
    else if (nparsed != len) { // parser.http_errno == HPE_OK && !parser.upgrade
        LWarn("HTTP parse failed: ",  len, " != ",  nparsed)

        // Handle error. Usually just close the connection.
        onError(_parser.http_errno);
        //assert(0);
    }

    return nparsed;
}


void Parser::reset()
{
    _complete = false;
    _upgrade = false;
    //_shouldKeepAlive = false;
    _error.reset();
}


void Parser::setRequest(http::Request* request)
{
    assert(!_request);
    assert(!_response);
    assert(_type == HTTP_REQUEST);
    _request = request;
}


void Parser::setResponse(http::Response* response)
{
    assert(!_request);
    assert(!_response);
    assert(_type == HTTP_RESPONSE);
    _response = response;
}


void Parser::setObserver(ParserObserver* observer)
{
    _observer = observer;
}


http::Message* Parser::message()
{
    return _request ? reinterpret_cast<http::Message*>(_request)
                    : _response ? reinterpret_cast<http::Message*>(_response)
                                : nullptr;
}


ParserObserver* Parser::observer() const
{
    return _observer;
}


bool Parser::complete() const
{
    return _complete;
}


bool Parser::upgrade() const
{
    return _upgrade; // parser.upgrade > 0;
}


//
// Callbacks


void Parser::onURL(const std::string& value)
{
    // LTrace("onURL: ", value)

    if (_request)
        _request->setURI(value);
}


void Parser::onHeader(const std::string& name, const std::string& value)
{
    // LTrace("On header: ",  name,  ":", value)

    if (message())
        message()->add(name, value);
    if (_observer)
        _observer->onParserHeader(name, value);
}


void Parser::onHeadersEnd()
{
    _upgrade = _parser.upgrade > 0;

    if (_observer)
        _observer->onParserHeadersEnd(_upgrade);
}


void Parser::onBody(const char* buf, size_t len)
{
    // LTrace("On body")
    if (_observer)
        _observer->onParserChunk(buf, len);
}


void Parser::onMessageEnd()
{
    // LTrace("On message end")
    _complete = true;
    if (_observer)
        _observer->onParserEnd();
}



void Parser::onError(unsigned errorno, const std::string& message)
{
    assert(errorno != HPE_OK);
    SDebug << "Parse error: "
        << ::http_errno_name((::http_errno)errorno) << ": "
        << ::http_errno_description((::http_errno)errorno) << endl;

    _complete = true;
    _error.err = (http_errno)errorno; // HTTP_PARSER_ERRNO((http_errno)errno);
    _error.message = message.empty() ? http_errno_name((::http_errno)errorno) : message;
    if (_observer)
        _observer->onParserError(_error);
}


//
// http_parser callbacks
//


int Parser::on_message_begin(http_parser* parser)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    self->reset();
    return 0;
}


int Parser::on_url(http_parser* parser, const char* at, size_t len)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);
    assert(at && len);

    self->onURL(std::string(at, len));
    return 0;
}


int Parser::on_status(http_parser* parser, const char* at, size_t length)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    // Handle response status line
    if (self->_response)
        self->_response->setStatus((http::StatusCode)parser->status_code);

    return 0;
}


int Parser::on_header_field(http_parser* parser, const char* at, size_t len)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    if (self->_wasHeaderValue) {
        if (!self->_lastHeaderField.empty()) {
            self->onHeader(self->_lastHeaderField, self->_lastHeaderValue);
            self->_lastHeaderValue.clear();
        }
        self->_lastHeaderField = std::string(at, len);
        self->_wasHeaderValue = false;
    } else {
        self->_lastHeaderField += std::string(at, len);
    }

    return 0;
}


int Parser::on_header_value(http_parser* parser, const char* at, size_t len)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    if (!self->_wasHeaderValue) {
        self->_lastHeaderValue = std::string(at, len);
        self->_wasHeaderValue = true;
    } else {
        self->_lastHeaderValue += std::string(at, len);
    }

    return 0;
}


int Parser::on_headers_complete(http_parser* parser)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    // Add last entry if any
    if (!self->_lastHeaderField.empty()) {
        self->onHeader(self->_lastHeaderField, self->_lastHeaderValue);
    }

    // HTTP version
    // start_line_.version(parser_.http_major, parser_.http_minor);

    // KeepAlive
    // self->_message->setKeepAlive(http_should_keep_alive(&parser) > 0);
    // self->_shouldKeepAlive = http_should_keep_alive(&parser) > 0;

    // Request HTTP method
    if (self->_request)
        self->_request->setMethod(http_method_str(static_cast<http_method>(parser->method)));

    self->onHeadersEnd();
    return 0;
}


int Parser::on_body(http_parser* parser, const char* at, size_t len)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    self->onBody(at, len);
    return 0;
}


int Parser::on_message_complete(http_parser* parser)
{
    auto self = reinterpret_cast<Parser*>(parser->data);
    assert(self);

    // Signal message complete when the http_parser
    // has finished receiving the message.
    self->onMessageEnd();
    return 0;
}


} // namespace http
} // namespace scy


/// @\}
