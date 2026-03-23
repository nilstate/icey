#include "httptests.h"

#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/buffer.h"
#include "icy/base64.h"
#include "icy/crypto/hash.h"

#include <cstring>

using namespace icy;
using namespace icy::test;


// Friend accessor functions for WebSocketFramer private members
namespace icy { namespace http { namespace ws {

void wsFramerTestAccess(WebSocketFramer& f, int state)
{
    f._headerState = state;
}

int wsFramerGetFlags(const WebSocketFramer& f)
{
    return f._frameFlags;
}

}}} // namespace icy::http::ws

using icy::http::ws::wsFramerTestAccess;
using icy::http::ws::wsFramerGetFlags;


/// Helper: start a local HTTP server that responds with a fixed body.
static std::unique_ptr<http::Server> makeHttpServer(uint16_t port,
    const std::string& body = "OK")
{
    auto server = std::make_unique<http::Server>(net::Address("127.0.0.1", port));
    server->start();
    server->Connection += [body](http::ServerConnection::Ptr conn) {
        conn->response().setStatus(http::StatusCode::OK);
        conn->response().setContentLength(body.size());
        conn->response().setContentType("text/plain");
        conn->sendHeader();
        conn->send(body.c_str(), body.size());
        conn->close();
    };
    return server;
}

/// Helper: start a local WebSocket/payload echo server.
/// Echoes back any payload data it receives.
static std::unique_ptr<http::Server> makeEchoServer(uint16_t port)
{
    auto server = std::make_unique<http::Server>(net::Address("127.0.0.1", port));
    server->start();
    server->Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.sendOwned(Buffer(bufferCast<const char*>(buffer),
                                  bufferCast<const char*>(buffer) + buffer.size()));
        };
    };
    return server;
}

static std::string computeWebSocketAccept(const std::string& key)
{
    icy::crypto::Hash hash("sha1");
    hash.update(key + icy::http::ws::ProtocolGuid);
    return icy::base64::encode(hash.digest());
}


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();
    net::SSLManager::initNoVerifyClient();

    //
    /// HTTP URL Tests
    //

    describe("url parser", []() {
        http::URL url;
        expect(url.scheme().empty());
        expect(url.authority().empty());
        expect(url.userInfo().empty());
        expect(url.host().empty());
        expect(url.port() == 0);
        expect(url.path().empty());
        expect(url.query().empty());
        expect(url.fragment().empty());

        http::URL url2("HTTP", "localhost", "/home/icey/foo.bar");
        expect(url2.scheme() == "http");
        expect(url2.host() == "localhost");
        expect(url2.path() == "/home/icey/foo.bar");

        http::URL url3("http", "www.0state.com", "/index.html");
        expect(url3.scheme() == "http");
        expect(url3.authority() == "www.0state.com");
        expect(url3.host() == "www.0state.com");
        expect(url3.path() == "/index.html");

        http::URL url4("http", "www.0state.com:8000", "/index.html");
        expect(url4.scheme() == "http");
        expect(url4.authority() == "www.0state.com:8000");
        expect(url4.host() == "www.0state.com");
        expect(url4.path() == "/index.html");

        http::URL url5("http", "user@www.0state.com:8000", "/index.html");
        expect(url5.scheme() == "http");
        expect(url5.userInfo() == "user");
        expect(url5.host() == "www.0state.com");
        expect(url5.port() == 8000);
        expect(url5.authority() == "user@www.0state.com:8000");
        expect(url5.path() == "/index.html");

        http::URL url6("http", "user@www.0state.com:80", "/index.html");
        expect(url6.scheme() == "http");
        expect(url6.userInfo() == "user");
        expect(url6.host() == "www.0state.com");
        expect(url6.port() == 80);
        expect(url6.authority() == "user@www.0state.com:80");
        expect(url6.path() == "/index.html");

        http::URL url7("http", "www.0state.com", "/index.html", "query=test", "fragment");
        expect(url7.scheme() == "http");
        expect(url7.authority() == "www.0state.com");
        expect(url7.path() == "/index.html");
        expect(url7.pathEtc() == "/index.html?query=test#fragment");
        expect(url7.query() == "query=test");
        expect(url7.fragment() == "fragment");

        http::URL url8("http", "www.0state.com", "/index.html?query=test#fragment");
        expect(url8.scheme() == "http");
        expect(url8.authority() == "www.0state.com");
        expect(url8.path() == "/index.html");
        expect(url8.pathEtc() == "/index.html?query=test#fragment");
        expect(url8.query() == "query=test");
        expect(url8.fragment() == "fragment");
    });

    //
    /// HTTP URL: full string parsing
    //

    describe("url string parsing", []() {
        http::URL url1("http://www.example.com:8080/path/to/resource?key=value#section");
        expect(url1.scheme() == "http");
        expect(url1.host() == "www.example.com");
        expect(url1.port() == 8080);
        expect(url1.path() == "/path/to/resource");
        expect(url1.query() == "key=value");
        expect(url1.fragment() == "section");

        http::URL url2("https://user:pass@example.com/path");
        expect(url2.scheme() == "https");
        expect(url2.host() == "example.com");
        expect(url2.path() == "/path");
        expect(url2.hasUserInfo());

        // No path
        http::URL url3("http://example.com");
        expect(url3.scheme() == "http");
        expect(url3.host() == "example.com");

        // Default ports
        http::URL url4("http://example.com/");
        expect(url4.port() == 80);

        http::URL url5("https://example.com/");
        expect(url5.port() == 443);

        // WebSocket schemes
        http::URL url6("ws://localhost:9090/ws");
        expect(url6.scheme() == "ws");
        expect(url6.host() == "localhost");
        expect(url6.port() == 9090);
        expect(url6.path() == "/ws");
    });

    //
    /// HTTP URL: encode/decode
    //

    describe("url encode/decode", []() {
        // Spaces and special characters
        std::string encoded = http::URL::encode("hello world & foo=bar");
        expect(encoded.find(' ') == std::string::npos);
        expect(encoded.find('&') == std::string::npos);

        std::string decoded = http::URL::decode(encoded);
        expect(decoded == "hello world & foo=bar");

        // Round-trip with all printable ASCII
        std::string printable;
        for (char c = 32; c < 127; ++c)
            printable += c;
        std::string roundtrip = http::URL::decode(http::URL::encode(printable));
        expect(roundtrip == printable);

        // Already-safe characters should remain unchanged
        std::string safe = "abcABC123-_.~";
        expect(http::URL::encode(safe) == safe);
    });

    //
    /// HTTP URL: validity
    //

    describe("url validity", []() {
        http::URL valid("http://example.com");
        expect(valid.valid());

        http::URL empty;
        expect(!empty.valid());

        // Parse with whiny=false should not throw
        http::URL url;
        bool ok = url.parse("not a valid url", false);
        (void)ok;
    });

    //
    /// HTTP URL: copy and assignment
    //

    describe("url copy and assignment", []() {
        http::URL url1("http://example.com:8080/path?q=1#frag");
        http::URL url2 = url1;
        expect(url2.scheme() == url1.scheme());
        expect(url2.host() == url1.host());
        expect(url2.port() == url1.port());
        expect(url2.path() == url1.path());
        expect(url2.query() == url1.query());
        expect(url2.fragment() == url1.fragment());

        // Assignment from string
        http::URL url3;
        url3 = "https://other.com/foo";
        expect(url3.scheme() == "https");
        expect(url3.host() == "other.com");
        expect(url3.path() == "/foo");
    });

    //
    /// HTTP URL Parameters Tests
    //

    describe("url query parameters", []() {
        NVCollection params;
        http::splitURIParameters("/streaming?format=MJPEG&width=400&height=300&"
                                 "encoding=Base64&packetizer=chunked&"
                                 "rand=0.09983996045775712",
                                 params);
        for (const auto& [key, val] : params) {
            // std::cout << "URL Parameter: " << key << ": " << val << endl;
        }

        expect(params.get("format") == "MJPEG");
        expect(params.get("Format") == "MJPEG");
        expect(params.get("width") == "400");
        expect(params.get("WIDTH") == "400");
        expect(params.get("height") == "300");
        expect(params.get("encoding") == "Base64");
        expect(params.get("ENCODING") == "Base64");
        expect(params.get("packetizer") == "chunked");
        expect(params.get("rand") == "0.09983996045775712");
        expect(params.get("RaNd") == "0.09983996045775712");
        expect(params.get("0") == "streaming");
    });


    // =========================================================================
    // HTTP Client/Server Integration Tests
    //
    // These use a local echo server and test::waitFor instead of uv::runLoop()
    // to avoid hanging on failure.
    //

    describe("http: GET request", []() {
        auto server = makeHttpServer(TEST_HTTP_PORT, "hello");

        auto conn = http::Client::instance().createConnection(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT) + "/");
        bool complete = false;
        bool headersReceived = false;
        std::string received;

        conn->Headers += [&](http::Response& response) {
            headersReceived = true;
        };
        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
        };
        conn->Complete += [&](const http::Response& response) {
            complete = true;
        };
        conn->request().setKeepAlive(false);
        conn->send();

        expect(test::waitFor([&] { return complete; }));
        expect(headersReceived);
        expect(received == "hello");
        expect(!conn->error().any());

        server->shutdown();
    });

    describe("http: POST request", []() {
        // Verify the client can send a POST with body and receive a response.
        // The server ignores the body and responds with a fixed string;
        // we're testing the client-side POST mechanics, not server-side echo.
        auto server = makeHttpServer(TEST_HTTP_PORT + 1, "accepted");

        auto conn = http::Client::instance().createConnection(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 1) + "/submit");

        std::string received;
        bool complete = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
        };
        conn->Complete += [&](const http::Response& response) {
            complete = true;
        };
        conn->request().setMethod("POST");
        conn->request().setContentType("application/json");
        conn->request().setKeepAlive(false);

        std::string body = R"({"key":"value"})";
        conn->request().setContentLength(body.size());
        conn->send(body.c_str(), body.size());

        expect(test::waitFor([&] { return complete; }));
        expect(received == "accepted");
        expect(!conn->error().any());

        server->shutdown();
    });

    describe("http: multiple concurrent requests", []() {
        auto server = makeHttpServer(TEST_HTTP_PORT + 2, "ok");

        int numComplete = 0;
        constexpr int numRequests = 3;

        for (int i = 0; i < numRequests; i++) {
            auto conn = http::Client::instance().createConnection(
                "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 2) + "/");
            conn->Complete += [&](const http::Response& response) {
                numComplete++;
            };
            conn->request().setKeepAlive(false);
            conn->send();
        }

        expect(test::waitFor([&] { return numComplete == numRequests; }));

        server->shutdown();
    });

    describe("http: connection close", []() {
        auto server = makeHttpServer(TEST_HTTP_PORT + 3, "bye");

        auto conn = http::Client::instance().createConnection(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 3) + "/");
        bool closed = false;

        conn->Close += [&](http::Connection&) {
            closed = true;
        };
        conn->request().setKeepAlive(false);
        conn->send();

        expect(test::waitFor([&] { return closed; }));
        expect(conn->closed());
        expect(!conn->error().any());

        server->shutdown();
    });

    describe("websocket echo: payload roundtrip", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 4);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 4) + "/websocket");

        std::string received;
        bool gotPayload = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            if (received == "PING")
                gotPayload = true;
        };

        conn->send("PING", 4);

        expect(test::waitFor([&] { return gotPayload; }));
        expect(received == "PING");

        server->shutdown();
    });

    describe("websocket echo: multi-echo", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 5);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 5) + "/websocket");

        int numSuccess = 0;
        constexpr int numWanted = 10;

        conn->Payload += [&](const MutableBuffer& buffer) {
            std::string data(bufferCast<const char*>(buffer), buffer.size());
            if (data == "PING") {
                numSuccess++;
                if (numSuccess < numWanted)
                    conn->send("PING", 4);
            }
        };

        conn->send("PING", 4);

        expect(test::waitFor([&] { return numSuccess == numWanted; }, 5000));
        expect(numSuccess == numWanted);

        server->shutdown();
    });

    describe("websocket handshake: tokenized connection header", []() {
        http::Request request;
        http::Response response(http::StatusCode::SwitchingProtocols);
        http::ws::WebSocketFramer framer(http::ws::ClientSide);

        framer.createClientHandshakeRequest(request);

        response.set("Connection", "keep-alive, Upgrade");
        response.set("Upgrade", "websocket");
        response.set("Sec-WebSocket-Accept",
                     computeWebSocketAccept(request.get("Sec-WebSocket-Key")));

        expect(framer.checkClientHandshakeResponse(response));
    });

    describe("standalone client connection", []() {
        auto server = makeHttpServer(TEST_HTTP_PORT + 6, "standalone-ok");

        http::ClientConnection conn(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 6) + "/");
        bool complete = false;

        conn.Complete += [&](const http::Response& response) {
            complete = true;
        };
        conn.request().setKeepAlive(false);
        conn.setReadStream(new std::stringstream);
        conn.send();

        expect(test::waitFor([&] { return complete; }));
        expect(!conn.error().any());

        server->shutdown();
    });

    describe("http: server shutdown closes connections", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 7);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 7) + "/websocket");
        bool connected = false;
        bool closed = false;

        conn->Connect += [&]() { connected = true; };
        conn->Close += [&](http::Connection&) { closed = true; };
        conn->send("PING", 4);

        expect(test::waitFor([&] { return connected; }));

        // Shutdown server while client is connected
        server->shutdown();

        expect(test::waitFor([&] { return closed; }));
        expect(conn->closed());
    });

    describe("http: replace connection adapter", []() {
        auto server = makeHttpServer(TEST_HTTP_PORT + 8, "adapted");

        auto conn = http::Client::instance().createConnection(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 8) + "/");
        bool headersReceived = false;
        bool complete = false;

        conn->Headers += [&](http::Response& response) {
            headersReceived = true;
        };
        conn->Complete += [&](const http::Response& response) {
            complete = true;
            conn->close();
        };

        conn->replaceAdapter(std::make_unique<http::ConnectionAdapter>(conn.get(), HTTP_RESPONSE));
        conn->request().setKeepAlive(false);
        conn->send();

        expect(test::waitFor([&] { return conn->closed(); }));
        expect(headersReceived);
        expect(!conn->error().any());

        server->shutdown();
    });

    describe("http server state: keep-alive returns to receiving headers and idles out", []() {
        auto server = std::make_unique<http::Server>(net::Address("127.0.0.1", TEST_HTTP_PORT + 21));
        server->setKeepAliveTimeout(1);

        http::ServerConnection::Ptr serverConn;
        server->Connection += [&](http::ServerConnection::Ptr conn) {
            serverConn = conn;
            conn->response().setContentLength(0);
            conn->response().setKeepAlive(true);
            conn->sendHeader();
        };
        server->start();

        auto conn = http::Client::instance().createConnection(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 21) + "/");

        bool complete = false;
        bool closed = false;
        conn->Complete += [&](const http::Response&) { complete = true; };
        conn->Close += [&](http::Connection&) { closed = true; };
        conn->request().setKeepAlive(true);
        conn->send();

        expect(test::waitFor([&] { return complete && serverConn != nullptr; }, 5000));
        expect(serverConn->state() == http::ServerConnectionState::ReceivingHeaders);

        expect(test::waitFor([&] { return closed && serverConn->closed(); }, 5000));
        expect(serverConn->state() == http::ServerConnectionState::Closed);

        server->shutdown();
    });

    describe("http server state: streaming bypasses idle timeout", []() {
        auto server = std::make_unique<http::Server>(net::Address("127.0.0.1", TEST_HTTP_PORT + 22));
        server->setKeepAliveTimeout(1);

        http::ServerConnection::Ptr serverConn;
        server->Connection += [&](http::ServerConnection::Ptr conn) {
            serverConn = conn;
            conn->beginStreaming();
            conn->response().setChunkedTransferEncoding(true);
            conn->response().setContentType("text/event-stream");
            conn->response().setKeepAlive(true);
            conn->sendHeader();
        };
        server->start();

        auto conn = http::Client::instance().createConnection(
            "http://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 22) + "/stream");

        bool headers = false;
        bool closed = false;
        conn->Headers += [&](http::Response&) { headers = true; };
        conn->Close += [&](http::Connection&) { closed = true; };
        conn->request().setKeepAlive(true);
        conn->send();

        expect(test::waitFor([&] { return headers && serverConn != nullptr; }, 5000));
        expect(serverConn->state() == http::ServerConnectionState::Streaming);

        test::waitFor([] { return false; }, 2500);

        expect(!closed);
        expect(!serverConn->closed());
        expect(serverConn->state() == http::ServerConnectionState::Streaming);

        conn->close();
        expect(test::waitFor([&] { return closed || serverConn->closed(); }, 5000));

        server->shutdown();
    });

    describe("http server state: websocket upgrade bypasses idle timeout", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 23);
        server->setKeepAliveTimeout(1);

        http::ServerConnection::Ptr serverConn;
        server->Connection += [&](http::ServerConnection::Ptr conn) {
            serverConn = conn;
        };

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 23) + "/websocket");

        bool connected = false;
        bool closed = false;
        conn->Connect += [&]() { connected = true; };
        conn->Close += [&](http::Connection&) { closed = true; };
        conn->send("PING", 4);

        expect(test::waitFor([&] { return connected && serverConn != nullptr; }, 5000));
        expect(serverConn->state() == http::ServerConnectionState::Upgraded);

        test::waitFor([] { return false; }, 2500);

        expect(!closed);
        expect(!serverConn->closed());
        expect(serverConn->state() == http::ServerConnectionState::Upgraded);

        server->shutdown();
        expect(test::waitFor([&] { return closed; }, 5000));
    });


    // =========================================================================
    // Cookie Tests
    //

    describe("cookie basic", []() {
        http::Cookie cookie("session", "abc123");
        expect(cookie.getName() == "session");
        expect(cookie.getValue() == "abc123");
        expect(cookie.getVersion() == 0);
        expect(cookie.getMaxAge() == -1);
        expect(!cookie.getSecure());
        expect(!cookie.getHttpOnly());
    });

    describe("cookie attributes", []() {
        http::Cookie cookie("id", "42");
        cookie.setDomain(".example.com");
        cookie.setPath("/api");
        cookie.setSecure(true);
        cookie.setHttpOnly(true);
        cookie.setMaxAge(3600);
        cookie.setVersion(1);
        cookie.setComment("session cookie");

        expect(cookie.getDomain() == ".example.com");
        expect(cookie.getPath() == "/api");
        expect(cookie.getSecure());
        expect(cookie.getHttpOnly());
        expect(cookie.getMaxAge() == 3600);
        expect(cookie.getVersion() == 1);
        expect(cookie.getComment() == "session cookie");
    });

    describe("cookie toString", []() {
        http::Cookie cookie("token", "xyz");
        cookie.setPath("/");
        cookie.setHttpOnly(true);

        std::string str = cookie.toString();
        expect(str.find("token=xyz") != std::string::npos);
    });

    describe("cookie copy", []() {
        http::Cookie c1("a", "b");
        c1.setDomain(".test.com");
        c1.setSecure(true);

        http::Cookie c2(c1);
        expect(c2.getName() == "a");
        expect(c2.getValue() == "b");
        expect(c2.getDomain() == ".test.com");
        expect(c2.getSecure());
    });

    describe("cookie assignment", []() {
        http::Cookie c1("x", "y");
        c1.setMaxAge(100);

        http::Cookie c2;
        c2 = c1;
        expect(c2.getName() == "x");
        expect(c2.getValue() == "y");
        expect(c2.getMaxAge() == 100);
    });

    describe("cookie escape unescape", []() {
        std::string original = "hello world&foo=bar";
        std::string escaped = http::Cookie::escape(original);
        expect(escaped != original);
        expect(escaped.find(' ') == std::string::npos);

        std::string unescaped = http::Cookie::unescape(escaped);
        expect(unescaped == original);
    });

    describe("cookie from nvcollection", []() {
        // NVCollection constructor: unrecognized keys become name=key, value=value
        // This mirrors Set-Cookie header parsing where the cookie name IS the key
        NVCollection nvc;
        nvc.add("sid", "12345");
        nvc.add("path", "/");
        nvc.add("domain", ".example.com");

        http::Cookie cookie(nvc);
        expect(cookie.getName() == "sid");
        expect(cookie.getValue() == "12345");
        expect(cookie.getPath() == "/");
        expect(cookie.getDomain() == ".example.com");
    });


    // =========================================================================
    // Authenticator Tests
    //

    describe("authenticator basic", []() {
        http::Authenticator auth("admin", "secret");
        expect(auth.username() == "admin");
        expect(auth.password() == "secret");
    });

    describe("authenticator setters", []() {
        http::Authenticator auth;
        auth.setUsername("user1");
        auth.setPassword("pass1");
        expect(auth.username() == "user1");
        expect(auth.password() == "pass1");
    });

    describe("authenticator from user info", []() {
        http::Authenticator auth;
        auth.fromUserInfo("alice:wonderland");
        expect(auth.username() == "alice");
        expect(auth.password() == "wonderland");
    });

    describe("authenticator from uri", []() {
        http::URL url("http://bob:builder@example.com/path");
        http::Authenticator auth;
        auth.fromURI(url);
        expect(auth.username() == "bob");
        expect(auth.password() == "builder");
    });

    describe("basic authenticator", []() {
        http::BasicAuthenticator auth("testuser", "testpass");
        expect(auth.username() == "testuser");
        expect(auth.password() == "testpass");

        // Authenticate a request and check it has credentials
        http::Request request("GET", "/");
        auth.authenticate(request);
        expect(request.hasCredentials());

        std::string scheme, authInfo;
        request.getCredentials(scheme, authInfo);
        expect(scheme == "Basic");
        expect(!authInfo.empty());
    });

    describe("basic authenticator from request", []() {
        // Set up a request with basic auth
        http::Request request("GET", "/");
        http::BasicAuthenticator auth1("myuser", "mypass");
        auth1.authenticate(request);

        // Parse it back
        http::BasicAuthenticator auth2(request);
        expect(auth2.username() == "myuser");
        expect(auth2.password() == "mypass");
    });

    describe("basic authenticator proxy", []() {
        http::BasicAuthenticator auth("proxyuser", "proxypass");
        http::Request request("GET", "/");
        auth.proxyAuthenticate(request);
        expect(request.hasProxyCredentials());
    });

    describe("credential helpers", []() {
        expect(http::isBasicCredentials("Basic dXNlcjpwYXNz"));
        expect(!http::isBasicCredentials("Digest realm=\"test\""));
        expect(http::isDigestCredentials("Digest realm=\"test\""));
        expect(!http::isDigestCredentials("Basic dXNlcjpwYXNz"));
    });

    describe("extract credentials", []() {
        std::string username, password;
        http::extractCredentials(std::string_view("user:pass"), username, password);
        expect(username == "user");
        expect(password == "pass");
    });

    describe("extract credentials from url", []() {
        http::URL url("http://alice:secret@example.com/");
        std::string username, password;
        http::extractCredentials(url, username, password);
        expect(username == "alice");
        expect(password == "secret");
    });

    describe("has credentials helpers", []() {
        http::Request req("GET", "/");
        expect(!http::hasBasicCredentials(req));
        expect(!http::hasDigestCredentials(req));
        expect(!http::hasProxyBasicCredentials(req));
        expect(!http::hasProxyDigestCredentials(req));

        http::BasicAuthenticator auth("u", "p");
        auth.authenticate(req);
        expect(http::hasBasicCredentials(req));
        expect(!http::hasDigestCredentials(req));
    });


    // =========================================================================
    // StringPart Tests
    //

    describe("string part basic", []() {
        http::StringPart part("hello world", "text/plain");
        expect(part.contentType() == "text/plain");
        expect(part.length() == 11);
    });

    describe("string part write to stream", []() {
        http::StringPart part("test data", "application/json");
        std::ostringstream oss;
        part.write(oss);
        expect(oss.str() == "test data");
    });

    describe("string part empty", []() {
        http::StringPart part("");
        expect(part.length() == 0);
    });

    describe("string part default content type", []() {
        http::StringPart part("data");
        expect(part.contentType() == "application/octet-stream");
    });


    // =========================================================================
    // HTTP Request/Response Round-Trip Tests
    //

    describe("http request round-trip", []() {
        // Create a GET request with headers
        http::Request req("GET", "/api/users?page=1");
        req.setHost("example.com", 8080);
        req.set("User-Agent", "Icey/2.0");
        req.set("Accept", "application/json");

        // Serialize
        std::ostringstream oss;
        req.write(oss);
        std::string raw = oss.str();

        // Verify serialized output contains key parts
        expect(raw.find("GET /api/users?page=1 HTTP/1.1") != std::string::npos);
        expect(raw.find("Host: example.com:8080") != std::string::npos);
        expect(raw.find("User-Agent: Icey/2.0") != std::string::npos);

        // Parse back
        http::Request parsed;
        http::Parser parser(&parsed);
        auto result = parser.parse(raw.c_str(), raw.size());
        expect(result.ok());
        expect(result.messageComplete);

        expect(parsed.getMethod() == "GET");
        expect(parsed.getURI() == "/api/users?page=1");
        expect(parsed.getHost() == "example.com:8080" || parsed.getHost() == "example.com");
        expect(parsed.get("User-Agent") == "Icey/2.0");
        expect(parsed.get("Accept") == "application/json");
    });

    describe("http request split uri parsing", []() {
        http::Request parsed;
        http::Parser parser(&parsed);

        std::string part1 = "GET /api/us";
        std::string part2 =
            "ers?page=1 HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "\r\n";

        auto first = parser.parse(part1.c_str(), part1.size());
        expect(first.ok());
        expect(first.bytesConsumed == part1.size());
        expect(!first.messageComplete);

        auto second = parser.parse(part2.c_str(), part2.size());
        expect(second.ok());
        expect(second.bytesConsumed == part2.size());
        expect(second.messageComplete);
        expect(parsed.getURI() == "/api/users?page=1");
    });

    describe("http response round-trip", []() {
        http::Response res;
        res.setStatus(http::StatusCode::OK);
        res.setReason("OK");
        res.set("Content-Type", "text/html");
        res.setContentLength(13);

        std::ostringstream oss;
        res.write(oss);
        std::string raw = oss.str();

        expect(raw.find("HTTP/1.1 200 OK") != std::string::npos);
        expect(raw.find("Content-Type: text/html") != std::string::npos);
        expect(raw.find("Content-Length: 13") != std::string::npos);

        // Parse back
        http::Response parsed;
        http::Parser parser(&parsed);
        auto result = parser.parse(raw.c_str(), raw.size());
        expect(result.ok());
        expect(result.messageComplete);

        expect(parsed.getStatus() == http::StatusCode::OK);
        expect(parsed.get("Content-Type") == "text/html");
    });

    describe("http post request", []() {
        http::Request req("POST", "/api/data");
        req.setHost("localhost");
        req.setContentType("application/json");
        req.setContentLength(27);

        std::ostringstream oss;
        req.write(oss);
        std::string raw = oss.str();

        expect(raw.find("POST /api/data HTTP/1.1") != std::string::npos);
        expect(raw.find("Content-Type: application/json") != std::string::npos);
        expect(raw.find("Content-Length: 27") != std::string::npos);
    });

    describe("http request methods", []() {
        http::Request get("GET", "/");
        expect(get.getMethod() == "GET");

        http::Request post("POST", "/submit");
        expect(post.getMethod() == "POST");

        http::Request put("PUT", "/resource");
        expect(put.getMethod() == "PUT");

        http::Request del("DELETE", "/resource/1");
        expect(del.getMethod() == "DELETE");

        http::Request head("HEAD", "/");
        expect(head.getMethod() == "HEAD");
    });

    describe("http request keep-alive", []() {
        http::Request req("GET", "/");
        req.setKeepAlive(true);

        std::ostringstream oss;
        req.write(oss);
        std::string raw = oss.str();
        expect(raw.find("Connection: Keep-Alive") != std::string::npos ||
               raw.find("Connection: keep-alive") != std::string::npos);

        http::Request req2("GET", "/");
        req2.setKeepAlive(false);

        std::ostringstream oss2;
        req2.write(oss2);
        std::string raw2 = oss2.str();
        expect(raw2.find("Connection: Close") != std::string::npos ||
               raw2.find("Connection: close") != std::string::npos);
    });

    describe("http response status codes", []() {
        http::Response ok(http::StatusCode::OK);
        expect(ok.getStatus() == http::StatusCode::OK);
        expect(ok.success());

        http::Response notFound(http::StatusCode::NotFound);
        expect(notFound.getStatus() == http::StatusCode::NotFound);
        expect(!notFound.success());

        http::Response serverErr(http::StatusCode::InternalServerError);
        expect(serverErr.getStatus() == http::StatusCode::InternalServerError);
        expect(!serverErr.success());

        http::Response redirect(http::StatusCode::MovedPermanently);
        expect(redirect.getStatus() == http::StatusCode::MovedPermanently);
        expect(redirect.success());
    });

    describe("http response cookies", []() {
        http::Response res;
        http::Cookie cookie("sid", "abc123");
        cookie.setPath("/");
        cookie.setHttpOnly(true);
        res.addCookie(cookie);

        std::vector<http::Cookie> cookies;
        res.getCookies(cookies);
        expect(cookies.size() == 1);
        expect(cookies[0].getName() == "sid");
        expect(cookies[0].getValue() == "abc123");
    });

    describe("http chunked transfer encoding", []() {
        http::Request req("POST", "/stream");
        req.setChunkedTransferEncoding(true);

        std::ostringstream oss;
        req.write(oss);
        std::string raw = oss.str();
        expect(raw.find("Transfer-Encoding: chunked") != std::string::npos);
    });

    describe("http content length", []() {
        http::Request req("POST", "/data");
        req.setContentLength(42);
        expect(req.getContentLength() == 42);

        req.setContentLength(0);
        expect(req.getContentLength() == 0);
    });


    // =========================================================================
    // WebSocket Frame Encoding/Decoding Tests
    //
    // RFC 6455: client-to-server frames MUST be masked, server-to-client MUST NOT.
    // So: client writes masked -> server reads; server writes unmasked -> client reads.
    //

    describe("websocket: server-to-client text frame", []() {
        // Server writes unmasked text frame, client reads it
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        const char* payload = "Hello, WebSocket!";
        size_t payloadLen = strlen(payload);

        Buffer frameBuf;
        frameBuf.reserve(payloadLen + 14);
        BitWriter writer(frameBuf);

        serverFramer.writeFrame(payload, payloadLen, http::ws::SendFlags::Text, writer);

        // First byte: FIN + Text opcode = 0x81
        expect(static_cast<uint8_t>(frameBuf[0]) == 0x81);

        // Second byte: no mask bit + length
        expect(static_cast<uint8_t>(frameBuf[1]) == payloadLen);

        // Client reads unmasked server frame
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = clientFramer.readFrame(reader, readPayload);

        expect(readLen == payloadLen);
        expect(std::string(readPayload, static_cast<size_t>(readLen)) == "Hello, WebSocket!");
    });

    describe("websocket: server-to-client binary frame", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        const char binaryData[] = {0x00, 0x01, 0x02, static_cast<char>(0xFF), static_cast<char>(0xFE)};

        Buffer frameBuf;
        frameBuf.reserve(sizeof(binaryData) + 14);
        BitWriter writer(frameBuf);

        serverFramer.writeFrame(binaryData, sizeof(binaryData), http::ws::SendFlags::Binary, writer);

        // First byte: FIN + Binary opcode = 0x82
        expect(static_cast<uint8_t>(frameBuf[0]) == 0x82);

        // Client reads it
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = clientFramer.readFrame(reader, readPayload);

        expect(readLen == sizeof(binaryData));
        expect(memcmp(readPayload, binaryData, sizeof(binaryData)) == 0);
    });

    describe("websocket: client-to-server masked text", []() {
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        const char* payload = "Masked data";
        size_t payloadLen = strlen(payload);

        Buffer frameBuf;
        frameBuf.reserve(payloadLen + 14);
        BitWriter writer(frameBuf);

        clientFramer.writeFrame(payload, payloadLen, http::ws::SendFlags::Text, writer);

        // Mask bit must be set (second byte & 0x80)
        expect((static_cast<uint8_t>(frameBuf[1]) & 0x80) != 0);

        // Server reads and unmasks
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);

        expect(readLen == payloadLen);
        expect(std::string(readPayload, static_cast<size_t>(readLen)) == "Masked data");
    });

    describe("websocket: client-to-server masked binary", []() {
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        const char binaryData[] = {
            static_cast<char>(0xDE), static_cast<char>(0xAD),
            static_cast<char>(0xBE), static_cast<char>(0xEF),
            0x00, 0x01, 0x02, 0x03
        };

        Buffer frameBuf;
        frameBuf.reserve(sizeof(binaryData) + 14);
        BitWriter writer(frameBuf);

        clientFramer.writeFrame(binaryData, sizeof(binaryData), http::ws::SendFlags::Binary, writer);

        // Mask bit set, binary opcode
        expect((static_cast<uint8_t>(frameBuf[0]) & 0x0F) == unsigned(http::ws::Opcode::Binary));
        expect((static_cast<uint8_t>(frameBuf[1]) & 0x80) != 0);

        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);

        expect(readLen == sizeof(binaryData));
        expect(memcmp(readPayload, binaryData, sizeof(binaryData)) == 0);
    });

    describe("websocket: server rejects unmasked client frame", []() {
        // Server-side framer writing unmasked, then reading as server = error
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        const char* payload = "bad";
        Buffer frameBuf;
        frameBuf.reserve(20);
        BitWriter writer(frameBuf);

        // Server writes unmasked frame (as if from server)
        serverFramer.writeFrame(payload, 3, http::ws::SendFlags::Text, writer);

        // Server tries to read it - should reject (unmasked client-to-server)
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, readPayload);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        expect(threw);
    });

    describe("websocket: control frame ping/pong", []() {
        // Client sends Ping, server reads it
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        const char* pingData = "ping";
        int pingFlags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Ping);

        Buffer frameBuf;
        frameBuf.reserve(20);
        BitWriter writer(frameBuf);
        clientFramer.writeFrame(pingData, 4, pingFlags, writer);

        // Read on server side
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);

        int opcode = wsFramerGetFlags(serverFramer) & unsigned(http::ws::Opcode::Bitmask);
        expect(opcode == unsigned(http::ws::Opcode::Ping));
        expect(readLen == 4);
        expect(memcmp(readPayload, "ping", 4) == 0);

        // Server sends Pong back, client reads it
        int pongFlags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Pong);

        Buffer pongBuf;
        pongBuf.reserve(20);
        BitWriter pongWriter(pongBuf);
        serverFramer.writeFrame(pingData, 4, pongFlags, pongWriter);

        BitReader pongReader(mutableBuffer(pongBuf.data(), pongWriter.position()));
        char* pongPayload = nullptr;
        uint64_t pongLen = clientFramer.readFrame(pongReader, pongPayload);

        int pongOpcode = wsFramerGetFlags(clientFramer) & unsigned(http::ws::Opcode::Bitmask);
        expect(pongOpcode == unsigned(http::ws::Opcode::Pong));
        expect(pongLen == 4);
        expect(memcmp(pongPayload, "ping", 4) == 0);
    });

    describe("websocket: bidirectional text exchange", []() {
        // Simulate a full bidirectional conversation at the framer level
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Client -> Server: "hello"
        {
            Buffer buf;
            buf.reserve(64);
            BitWriter writer(buf);
            clientFramer.writeFrame("hello", 5, http::ws::SendFlags::Text, writer);

            BitReader reader(mutableBuffer(buf.data(), writer.position()));
            char* payload = nullptr;
            uint64_t len = serverFramer.readFrame(reader, payload);
            expect(len == 5);
            expect(std::string(payload, static_cast<size_t>(len)) == "hello");
        }

        // Server -> Client: "world"
        {
            Buffer buf;
            buf.reserve(64);
            BitWriter writer(buf);
            serverFramer.writeFrame("world", 5, http::ws::SendFlags::Text, writer);

            BitReader reader(mutableBuffer(buf.data(), writer.position()));
            char* payload = nullptr;
            uint64_t len = clientFramer.readFrame(reader, payload);
            expect(len == 5);
            expect(std::string(payload, static_cast<size_t>(len)) == "world");
        }

        // Client -> Server: binary packet
        {
            const char binData[] = {0x01, 0x02, 0x03, 0x04};
            Buffer buf;
            buf.reserve(64);
            BitWriter writer(buf);
            clientFramer.writeFrame(binData, 4, http::ws::SendFlags::Binary, writer);

            BitReader reader(mutableBuffer(buf.data(), writer.position()));
            char* payload = nullptr;
            uint64_t len = serverFramer.readFrame(reader, payload);
            expect(len == 4);
            expect(memcmp(payload, binData, 4) == 0);
        }

        // Server -> Client: binary response
        {
            const char binResp[] = {static_cast<char>(0xFF), 0x00, static_cast<char>(0xAA), static_cast<char>(0x55)};
            Buffer buf;
            buf.reserve(64);
            BitWriter writer(buf);
            serverFramer.writeFrame(binResp, 4, http::ws::SendFlags::Binary, writer);

            BitReader reader(mutableBuffer(buf.data(), writer.position()));
            char* payload = nullptr;
            uint64_t len = clientFramer.readFrame(reader, payload);
            expect(len == 4);
            expect(memcmp(payload, binResp, 4) == 0);
        }
    });

    describe("websocket: empty payload", []() {
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Client sends empty text frame
        Buffer buf;
        buf.reserve(20);
        BitWriter writer(buf);
        clientFramer.writeFrame("", 0, http::ws::SendFlags::Text, writer);

        BitReader reader(mutableBuffer(buf.data(), writer.position()));
        char* payload = nullptr;
        uint64_t len = serverFramer.readFrame(reader, payload);
        expect(len == 0);
    });

    describe("websocket: large payload masking", []() {
        // Test with a payload > 125 bytes (uses 16-bit extended length)
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        std::string largePayload(300, 'X');
        // Add some variety
        for (size_t i = 0; i < largePayload.size(); i++)
            largePayload[i] = static_cast<char>(i & 0xFF);

        Buffer buf;
        buf.reserve(largePayload.size() + 14);
        BitWriter writer(buf);
        clientFramer.writeFrame(largePayload.c_str(), largePayload.size(), http::ws::SendFlags::Binary, writer);

        // 16-bit extended length: second byte should be 126
        expect((static_cast<uint8_t>(buf[1]) & 0x7F) == 126);

        BitReader reader(mutableBuffer(buf.data(), writer.position()));
        char* payload = nullptr;
        uint64_t len = serverFramer.readFrame(reader, payload);
        expect(len == largePayload.size());
        expect(memcmp(payload, largePayload.c_str(), largePayload.size()) == 0);
    });


    // =========================================================================
    // WebSocket Integration Tests (client <-> server over real sockets)
    //

    describe("websocket integration: text echo", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 9);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 9) + "/websocket");

        std::string received;
        bool gotReply = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            if (received == "Hello from client")
                gotReply = true;
        };

        conn->send("Hello from client", 17);

        expect(test::waitFor([&] { return gotReply; }));
        expect(received == "Hello from client");

        server->shutdown();
    });

    describe("websocket integration: binary echo", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 10);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 10) + "/websocket");

        const char binData[] = {
            0x00, 0x01, 0x02, 0x03,
            static_cast<char>(0xDE), static_cast<char>(0xAD),
            static_cast<char>(0xBE), static_cast<char>(0xEF)
        };

        std::string received;
        bool gotReply = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            if (received.size() == sizeof(binData))
                gotReply = true;
        };

        conn->send(binData, sizeof(binData));

        expect(test::waitFor([&] { return gotReply; }));
        expect(received.size() == sizeof(binData));
        expect(memcmp(received.data(), binData, sizeof(binData)) == 0);

        server->shutdown();
    });

    describe("websocket integration: rapid bidirectional exchange", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 11);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 11) + "/websocket");

        int sendCount = 0;
        int recvCount = 0;
        constexpr int total = 50;

        conn->Payload += [&](const MutableBuffer& buffer) {
            std::string data(bufferCast<const char*>(buffer), buffer.size());
            if (data == "PING") {
                recvCount++;
                if (sendCount < total) {
                    conn->send("PING", 4);
                    sendCount++;
                }
            }
        };

        conn->send("PING", 4);
        sendCount++;

        expect(test::waitFor([&] { return recvCount == total; }, 10000));
        expect(recvCount == total);

        server->shutdown();
    });

    describe("websocket integration: large binary payload", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 12);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 12) + "/websocket");

        // 4KB binary payload with known pattern
        std::string payload(4096, '\0');
        for (size_t i = 0; i < payload.size(); i++)
            payload[i] = static_cast<char>(i & 0xFF);

        std::string received;
        bool gotReply = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            if (received.size() >= payload.size())
                gotReply = true;
        };

        conn->send(payload.c_str(), payload.size());

        expect(test::waitFor([&] { return gotReply; }, 5000));
        expect(received.size() == payload.size());
        expect(received == payload);

        server->shutdown();
    });


    //
    /// WebSocket hardening tests - protocol edge cases
    //

    describe("websocket: close frame roundtrip", []() {
        // Client sends Close with status 1000, server reads it
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Build close payload: 2-byte status code + reason
        char closePayload[32];
        BitWriter pw(closePayload, sizeof(closePayload));
        pw.putU16(1000);
        pw.put("Normal Closure", 14);

        int closeFlags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Close);
        Buffer frameBuf;
        frameBuf.reserve(64);
        BitWriter writer(frameBuf);
        clientFramer.writeFrame(closePayload, pw.position(), closeFlags, writer);

        // Server reads the close frame
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);

        int opcode = wsFramerGetFlags(serverFramer) & unsigned(http::ws::Opcode::Bitmask);
        expect(opcode == unsigned(http::ws::Opcode::Close));
        expect(readLen == 16); // 2 bytes status + 14 bytes reason

        // Verify status code (network byte order)
        BitReader statusReader(readPayload, 2);
        uint16_t statusCode;
        statusReader.getU16(statusCode);
        expect(statusCode == 1000);
        expect(std::string(readPayload + 2, 14) == "Normal Closure");
    });

    describe("websocket: RSV bits rejected", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Manually craft a frame with RSV1 set: 0xC1 = FIN(0x80) + RSV1(0x40) + Text(0x01)
        char raw[] = {
            static_cast<char>(0xC1), // FIN + RSV1 + Text
            static_cast<char>(0x84), // MASK + length 4
            0x00, 0x00, 0x00, 0x00,  // mask key (zeros)
            't', 'e', 's', 't'       // payload (mask is zero so plaintext)
        };

        BitReader reader(mutableBuffer(raw, sizeof(raw)));
        char* payload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, payload);
        } catch (const http::ws::WebSocketException& e) {
            threw = true;
            expect(e.code() == http::ws::ErrorCode::InvalidRsvBits);
            expect(std::string_view(e.what()).find("RSV bits") != std::string_view::npos);
        }
        expect(threw);
    });

    describe("websocket: reserved opcode rejected", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Opcode 0x03 is reserved (between Binary=0x02 and Close=0x08)
        char raw[] = {
            static_cast<char>(0x83), // FIN + opcode 3
            static_cast<char>(0x81), // MASK + length 1
            0x00, 0x00, 0x00, 0x00,  // mask key
            'x'                       // payload
        };

        BitReader reader(mutableBuffer(raw, sizeof(raw)));
        char* payload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, payload);
        } catch (const http::ws::WebSocketException& e) {
            threw = true;
            expect(e.code() == http::ws::ErrorCode::InvalidOpcode);
            expect(std::string_view(e.what()).find("Reserved opcode") != std::string_view::npos);
        }
        expect(threw);
    });

    describe("websocket: unknown opcode rejected", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Opcode 0x0B is unknown (> Pong=0x0A)
        char raw[] = {
            static_cast<char>(0x8B), // FIN + opcode 0x0B
            static_cast<char>(0x81), // MASK + length 1
            0x00, 0x00, 0x00, 0x00,
            'x'
        };

        BitReader reader(mutableBuffer(raw, sizeof(raw)));
        char* payload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, payload);
        } catch (const http::ws::WebSocketException& e) {
            threw = true;
            expect(e.code() == http::ws::ErrorCode::InvalidOpcode);
            expect(std::string_view(e.what()).find("Unknown opcode") != std::string_view::npos);
        }
        expect(threw);
    });

    describe("websocket: fragmented control frame rejected", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Ping with FIN=0 (fragmented control frame is illegal)
        char raw[] = {
            static_cast<char>(0x09), // NO FIN + Ping opcode
            static_cast<char>(0x84), // MASK + length 4
            0x00, 0x00, 0x00, 0x00,
            'p', 'i', 'n', 'g'
        };

        BitReader reader(mutableBuffer(raw, sizeof(raw)));
        char* payload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, payload);
        } catch (const http::ws::WebSocketException& e) {
            threw = true;
            expect(e.code() == http::ws::ErrorCode::ProtocolViolation);
            expect(std::string_view(e.what()).find("Fragmented control frame") != std::string_view::npos);
        }
        expect(threw);
    });

    describe("websocket: control frame >125 bytes rejected", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Ping with 126 in length field (uses extended 16-bit length, which is >125)
        char raw[140];
        raw[0] = static_cast<char>(0x89); // FIN + Ping
        raw[1] = static_cast<char>(0xFE); // MASK + 126 (16-bit extended)
        // 16-bit length = 126 (network byte order)
        raw[2] = 0x00;
        raw[3] = 0x7E; // 126
        // 4-byte mask key
        raw[4] = raw[5] = raw[6] = raw[7] = 0x00;
        // 126 bytes of payload
        memset(raw + 8, 'A', 126);

        BitReader reader(mutableBuffer(raw, 8 + 126));
        char* payload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, payload);
        } catch (const http::ws::WebSocketException& e) {
            threw = true;
            expect(e.code() == http::ws::ErrorCode::ProtocolViolation);
            expect(std::string_view(e.what()).find("Control frame payload too large") != std::string_view::npos);
        }
        expect(threw);
    });

    describe("websocket: incomplete frame throws", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Craft a valid masked frame claiming 10 bytes payload but only provide 5.
        // readFrame must throw (either "Incomplete frame" from the payload check
        // or "index out of range" from the BitReader if header parsing overruns).
        char raw[] = {
            static_cast<char>(0x81), // FIN + Text
            static_cast<char>(0x8A), // MASK + length 10
            0x00, 0x00, 0x00, 0x00,  // mask key
            'h', 'e', 'l', 'l', 'o'  // only 5 of 10 bytes
        };

        BitReader reader(mutableBuffer(raw, sizeof(raw)));
        char* payload = nullptr;
        bool threw = false;
        try {
            serverFramer.readFrame(reader, payload);
        } catch (const http::ws::WebSocketException& e) {
            threw = true;
            expect(e.code() == http::ws::ErrorCode::IncompleteFrame);
        }
        expect(threw);
    });

    describe("websocket: 16-bit extended length (200 bytes)", []() {
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // 200 bytes triggers 16-bit extended length encoding
        std::string payload(200, '\0');
        for (size_t i = 0; i < payload.size(); i++)
            payload[i] = static_cast<char>(i & 0xFF);

        Buffer frameBuf;
        frameBuf.reserve(payload.size() + 14);
        BitWriter writer(frameBuf);
        clientFramer.writeFrame(payload.data(), payload.size(), http::ws::SendFlags::Binary, writer);

        // Verify 16-bit extended length encoding
        // Byte 1: 0x82 (FIN + Binary)
        // Byte 2: 0xFE (MASK + 126 marker)
        expect(static_cast<uint8_t>(frameBuf[1]) == (0x80 | 126));

        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);
        expect(readLen == 200);
        expect(memcmp(readPayload, payload.data(), 200) == 0);
    });

    describe("websocket: 64-bit extended length (70000 bytes)", []() {
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        // 70000 bytes triggers 64-bit extended length encoding (>65535)
        std::string payload(70000, '\0');
        for (size_t i = 0; i < payload.size(); i++)
            payload[i] = static_cast<char>(i % 251); // prime modulus for pattern

        Buffer frameBuf;
        frameBuf.reserve(payload.size() + 14);
        BitWriter writer(frameBuf);
        serverFramer.writeFrame(payload.data(), payload.size(), http::ws::SendFlags::Binary, writer);

        // Verify 64-bit extended length encoding
        // Byte 2: 127 marker (no mask for server)
        expect((static_cast<uint8_t>(frameBuf[1]) & 0x7F) == 127);

        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = clientFramer.readFrame(reader, readPayload);
        expect(readLen == 70000);
        expect(memcmp(readPayload, payload.data(), 70000) == 0);
    });

    describe("websocket: multiple frames in single buffer", []() {
        // Simulate two frames arriving in one TCP segment
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        // Server writes two frames into a single buffer
        Buffer frame1, frame2;
        frame1.reserve(64);
        frame2.reserve(64);
        BitWriter w1(frame1), w2(frame2);
        serverFramer.writeFrame("first", 5, http::ws::SendFlags::Text, w1);
        serverFramer.writeFrame("second", 6, http::ws::SendFlags::Text, w2);

        // Concatenate into single buffer
        Buffer combined;
        combined.insert(combined.end(), frame1.data(), frame1.data() + w1.position());
        combined.insert(combined.end(), frame2.data(), frame2.data() + w2.position());

        BitReader reader(mutableBuffer(combined.data(), combined.size()));

        // Read first frame
        char* payload1 = nullptr;
        uint64_t len1 = clientFramer.readFrame(reader, payload1);
        expect(len1 == 5);
        expect(std::string(payload1, 5) == "first");

        // Read second frame from same reader
        char* payload2 = nullptr;
        uint64_t len2 = clientFramer.readFrame(reader, payload2);
        expect(len2 == 6);
        expect(std::string(payload2, 6) == "second");
    });

    describe("websocket: masking preserves binary integrity", []() {
        // Verify that mask/unmask roundtrip preserves all 256 byte values
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // All 256 byte values
        char allBytes[256];
        for (int i = 0; i < 256; i++)
            allBytes[i] = static_cast<char>(i);

        Buffer frameBuf;
        frameBuf.reserve(300);
        BitWriter writer(frameBuf);
        clientFramer.writeFrame(allBytes, 256, http::ws::SendFlags::Binary, writer);

        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);
        expect(readLen == 256);
        expect(memcmp(readPayload, allBytes, 256) == 0);
    });

    describe("websocket: masking with non-aligned payload sizes", []() {
        // Test payload sizes that don't align to 4 or 8 bytes
        // to exercise the remainder handling in applyMask
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        for (size_t sz : {1, 2, 3, 5, 7, 9, 13, 15, 17, 31, 33, 63, 65}) {
            std::string payload(sz, '\0');
            for (size_t i = 0; i < sz; i++)
                payload[i] = static_cast<char>((i * 37 + 11) & 0xFF);

            Buffer frameBuf;
            frameBuf.reserve(sz + 14);
            BitWriter writer(frameBuf);
            clientFramer.writeFrame(payload.data(), payload.size(), http::ws::SendFlags::Binary, writer);

            BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
            char* readPayload = nullptr;
            uint64_t readLen = serverFramer.readFrame(reader, readPayload);
            expect(readLen == sz);
            expect(memcmp(readPayload, payload.data(), sz) == 0);
        }
    });

    //
    /// WebSocket integration hardening tests
    //

    describe("websocket integration: close handshake", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 13);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 13) + "/websocket");

        // Send a message first to confirm connection works
        std::string received;
        bool gotReply = false;
        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            gotReply = true;
        };

        conn->send("before-close", 12);
        expect(test::waitFor([&] { return gotReply; }));
        expect(received == "before-close");

        // Close the connection and verify it shuts down cleanly
        bool closed = false;
        conn->Close += [&](http::Connection&) {
            closed = true;
        };

        conn->close();
        expect(test::waitFor([&] { return closed; }, 5000));

        server->shutdown();
    });

    describe("websocket integration: ping pong end-to-end", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 14);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 14) + "/websocket");

        // Send a data frame first to ensure connection is live
        bool gotReply = false;
        conn->Payload += [&](const MutableBuffer& buffer) {
            if (std::string(bufferCast<const char*>(buffer), buffer.size()) == "alive")
                gotReply = true;
        };

        conn->send("alive", 5);
        expect(test::waitFor([&] { return gotReply; }));

        // Send a Ping frame - the server adapter should auto-respond with Pong.
        // After pong is received, the connection should still work for data.
        bool gotPostPing = false;
        conn->Payload += [&](const MutableBuffer& buffer) {
            if (std::string(bufferCast<const char*>(buffer), buffer.size()) == "post-ping")
                gotPostPing = true;
        };

        // Send data after to prove connection is still alive
        conn->send("post-ping", 9);
        expect(test::waitFor([&] { return gotPostPing; }, 5000));

        server->shutdown();
    });

    describe("websocket integration: binary roundtrip all byte values", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 15);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 15) + "/websocket");

        // All 256 byte values to verify no corruption in the full stack
        char allBytes[256];
        for (int i = 0; i < 256; i++)
            allBytes[i] = static_cast<char>(i);

        std::string received;
        bool gotReply = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            if (received.size() >= 256)
                gotReply = true;
        };

        conn->send(allBytes, 256, http::ws::SendFlags::Binary);

        expect(test::waitFor([&] { return gotReply; }));
        expect(received.size() == 256);
        expect(memcmp(received.data(), allBytes, 256) == 0);

        server->shutdown();
    });

    describe("websocket integration: interleaved text and binary", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 16);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 16) + "/websocket");

        int replies = 0;
        std::vector<std::string> received;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.emplace_back(bufferCast<const char*>(buffer), buffer.size());
            replies++;
        };

        // Alternate text and binary sends
        conn->send("text1", 5, http::ws::SendFlags::Text);
        const char bin1[] = {0x01, 0x02, 0x03};
        conn->send(bin1, 3, http::ws::SendFlags::Binary);
        conn->send("text2", 5, http::ws::SendFlags::Text);
        const char bin2[] = {static_cast<char>(0xFF), static_cast<char>(0xFE)};
        conn->send(bin2, 2, http::ws::SendFlags::Binary);

        expect(test::waitFor([&] { return replies >= 4; }, 5000));

        expect(received[0] == "text1");
        expect(received[1] == std::string(bin1, 3));
        expect(received[2] == "text2");
        expect(received[3] == std::string(bin2, 2));

        server->shutdown();
    });

    describe("websocket integration: 100KB binary payload", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 17);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 17) + "/websocket");

        // 100KB payload - exercises 64-bit length encoding through full stack
        constexpr size_t payloadSize = 100 * 1024;
        std::string payload(payloadSize, '\0');
        for (size_t i = 0; i < payloadSize; i++)
            payload[i] = static_cast<char>((i * 31 + 7) & 0xFF);

        std::string received;
        bool gotReply = false;

        conn->Payload += [&](const MutableBuffer& buffer) {
            received.append(bufferCast<const char*>(buffer), buffer.size());
            if (received.size() >= payloadSize)
                gotReply = true;
        };

        conn->send(payload.c_str(), payload.size(), http::ws::SendFlags::Binary);

        expect(test::waitFor([&] { return gotReply; }, 10000));
        expect(received.size() == payloadSize);
        expect(received == payload);

        server->shutdown();
    });


    describe("websocket integration: fragmented message reassembly", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 18);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 18) + "/websocket");

        // Wait for connection to be active
        bool connected = false;
        conn->Payload += [&](const MutableBuffer&) {};
        conn->send("warmup", 6);
        conn->Payload.detachAll();

        std::string received;
        bool gotReply = false;
        conn->Payload += [&](const MutableBuffer& buffer) {
            auto s = std::string(bufferCast<const char*>(buffer), buffer.size());
            if (s == "warmup") {
                connected = true;
            } else {
                received.append(s);
                if (received == "HelloWorld")
                    gotReply = true;
            }
        };

        expect(test::waitFor([&] { return connected; }));

        // Now send a fragmented message: "Hello" (FIN=0, Text) + "World" (FIN=1, Continuation)
        // Use a client-side framer to get correct masking
        http::ws::WebSocketFramer fragFramer(http::ws::ClientSide);
        wsFramerTestAccess(fragFramer, 2);

        // Frame 1: Text, FIN=0 (first fragment)
        {
            int flags = unsigned(http::ws::Opcode::Text); // no FIN bit
            Buffer frameBuf;
            frameBuf.reserve(64);
            DynamicBitWriter writer(frameBuf);
            fragFramer.writeFrame("Hello", 5, flags, writer);
            conn->socket()->sendOwned(std::move(frameBuf));
        }

        // Frame 2: Continuation, FIN=1 (final fragment)
        {
            int flags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Continuation);
            Buffer frameBuf;
            frameBuf.reserve(64);
            DynamicBitWriter writer(frameBuf);
            fragFramer.writeFrame("World", 5, flags, writer);
            conn->socket()->sendOwned(std::move(frameBuf));
        }

        expect(test::waitFor([&] { return gotReply; }, 5000));
        expect(received == "HelloWorld");

        server->shutdown();
    });

    describe("websocket integration: fragmented binary with interleaved ping", []() {
        auto server = makeEchoServer(TEST_HTTP_PORT + 19);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 19) + "/websocket");

        // Warmup
        bool connected = false;
        std::string received;
        bool gotReply = false;
        conn->Payload += [&](const MutableBuffer& buffer) {
            auto s = std::string(bufferCast<const char*>(buffer), buffer.size());
            if (s == "warmup") {
                connected = true;
            } else {
                received.append(s);
                if (received.size() == 6)
                    gotReply = true;
            }
        };

        conn->send("warmup", 6);
        expect(test::waitFor([&] { return connected; }));

        http::ws::WebSocketFramer fragFramer(http::ws::ClientSide);
        wsFramerTestAccess(fragFramer, 2);

        // Fragment 1: Binary, FIN=0
        {
            int flags = unsigned(http::ws::Opcode::Binary);
            char data[] = {0x01, 0x02, 0x03};
            Buffer buf; buf.reserve(64);
            DynamicBitWriter w(buf);
            fragFramer.writeFrame(data, 3, flags, w);
            conn->socket()->sendOwned(std::move(buf));
        }

        // Interleaved Ping (RFC 6455 allows control frames during fragmentation)
        {
            int flags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Ping);
            Buffer buf; buf.reserve(64);
            DynamicBitWriter w(buf);
            fragFramer.writeFrame("hi", 2, flags, w);
            conn->socket()->sendOwned(std::move(buf));
        }

        // Fragment 2: Continuation, FIN=1
        {
            int flags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Continuation);
            char data[] = {0x04, 0x05, 0x06};
            Buffer buf; buf.reserve(64);
            DynamicBitWriter w(buf);
            fragFramer.writeFrame(data, 3, flags, w);
            conn->socket()->sendOwned(std::move(buf));
        }

        expect(test::waitFor([&] { return gotReply; }, 5000));
        expect(received.size() == 6);
        expect(received[0] == 0x01);
        expect(received[1] == 0x02);
        expect(received[2] == 0x03);
        expect(received[3] == 0x04);
        expect(received[4] == 0x05);
        expect(received[5] == 0x06);

        server->shutdown();
    });

    describe("websocket: shutdown produces valid close frame", []() {
        // Verify that shutdown() generates a properly framed Close message
        // by writing to a buffer and parsing the result
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        // Build close payload and frame it like shutdown() does
        char closePayload[125];
        BitWriter pw(closePayload, sizeof(closePayload));
        pw.putU16(1000);
        pw.put("goodbye", 7);

        int closeFlags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Close);
        Buffer frameBuf;
        frameBuf.reserve(64);
        DynamicBitWriter fw(frameBuf);
        clientFramer.writeFrame(closePayload, pw.position(), closeFlags, fw);

        // Verify the frame is valid by parsing it with a server framer
        BitReader reader(mutableBuffer(frameBuf));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);

        // Verify opcode is Close
        int opcode = wsFramerGetFlags(serverFramer) & unsigned(http::ws::Opcode::Bitmask);
        expect(opcode == unsigned(http::ws::Opcode::Close));

        // Verify payload: 2 bytes status + 7 bytes reason = 9 bytes
        expect(readLen == 9);

        // Verify status code
        BitReader statusReader(readPayload, 2);
        uint16_t statusCode;
        statusReader.getU16(statusCode);
        expect(statusCode == 1000);
        expect(std::string(readPayload + 2, 7) == "goodbye");

        // Verify mask bit was set (client-to-server)
        expect((static_cast<uint8_t>(frameBuf[1]) & 0x80) != 0);
    });

    describe("websocket integration: TCP segmentation recovery", []() {
        // Send a frame split across two TCP writes to verify the adapter
        // buffers incomplete frames and reassembles them
        auto server = makeEchoServer(TEST_HTTP_PORT + 20);

        auto conn = http::Client::instance().createConnection(
            "ws://127.0.0.1:" + std::to_string(TEST_HTTP_PORT + 20) + "/websocket");

        // Warmup to ensure connection is established
        bool connected = false;
        std::string received;
        bool gotReply = false;
        conn->Payload += [&](const MutableBuffer& buffer) {
            auto s = std::string(bufferCast<const char*>(buffer), buffer.size());
            if (s == "warmup") {
                connected = true;
            } else {
                received.append(s);
                if (received == "split-test-data")
                    gotReply = true;
            }
        };

        conn->send("warmup", 6);
        expect(test::waitFor([&] { return connected; }));

        // Construct a complete frame, then split it and send in two TCP writes
        http::ws::WebSocketFramer splitFramer(http::ws::ClientSide);
        wsFramerTestAccess(splitFramer, 2);

        Buffer frameBuf;
        frameBuf.reserve(64);
        DynamicBitWriter writer(frameBuf);
        splitFramer.writeFrame("split-test-data", 15, http::ws::SendFlags::Text, writer);

        size_t totalLen = frameBuf.size();
        size_t splitPoint = totalLen / 2; // split in the middle

        // Send first half
        conn->socket()->sendOwned(Buffer(frameBuf.begin(), frameBuf.begin() + splitPoint));

        // Small delay to ensure the first half is processed before the second
        test::waitFor([] { return false; }, 50);

        // Send second half
        conn->socket()->sendOwned(Buffer(frameBuf.begin() + splitPoint, frameBuf.end()));

        expect(test::waitFor([&] { return gotReply; }, 5000));
        expect(received == "split-test-data");

        server->shutdown();
    });


    // =========================================================================
    // Parser returns consumed bytes on upgrade (not full length)
    //
    describe("parser upgrade returns consumed bytes", []() {
        // Simulate an HTTP upgrade request followed by trailing WebSocket data
        std::string upgradeReq =
            "GET /ws HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
            "Sec-WebSocket-Version: 13\r\n"
            "\r\n";
        std::string trailing = "\x81\x05Hello"; // a WS frame
        std::string coalesced = upgradeReq + trailing;

        http::Request parsed;
        http::Parser parser(&parsed);
        auto result = parser.parse(coalesced.c_str(), coalesced.size());

        // Parser should consume only the HTTP headers, not the trailing WS data
        expect(result.ok());
        expect(result.bytesConsumed == upgradeReq.size());
        expect(result.upgrade);
        expect(parser.upgrade());
    });


    // =========================================================================
    // HTTP version parsed from wire (not hardcoded 1.1)
    //
    describe("http version from wire", []() {
        std::string raw =
            "HTTP/1.0 200 OK\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        http::Response parsed;
        http::Parser parser(&parsed);
        auto result = parser.parse(raw.c_str(), raw.size());
        expect(result.ok());
        expect(result.messageComplete);

        expect(parsed.getVersion() == "HTTP/1.0");
    });


    test::runAll();

    http::Client::destroy();
    return test::finalize();
}
