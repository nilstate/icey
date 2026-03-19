#include "httptests.h"

#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/buffer.h"

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

    //
    /// Default HTTP Client Connection Test
    //

    describe("client connection download", []() {
        std::string path(ICY_BUILD_DIR);
        fs::addnode(path, "zlib-1.2.8.tar.gz");

        auto conn = http::Client::instance().createConnection("http://zlib.net/fossils/zlib-1.2.8.tar.gz");
        conn->Complete += [&](const http::Response& response) {
            // std::cout << "Lerver response: "(response, )
        };
        conn->request().setMethod("GET");
        conn->request().setKeepAlive(false);
        conn->setReadStream(new std::ofstream(path, std::ios_base::out | std::ios_base::binary));
        conn->send();

        uv::runLoop();

        expect(fs::exists(path));
        expect(crypto::checksum("MD5", path) == "44d667c142d7cda120332623eab69f40");
        fs::unlink(path);
    });

    // describe("secure client connection download", []() {
    //     auto conn = http::Client::instance().createConnection("https://anionu.com/assets/download/25/SpotInstaller.exe");
    //     conn->Complete += sdelegate(&context, &CallbackContext::onClientConnectionDownloadComplete);
    //     conn->request().setMethod("GET");
    //     conn->request().setKeepAlive(false);
    //     conn->setReadStream(new std::ofstream("SpotInstaller.exe", std::ios_base::out | std::ios_base::binary));
    //     conn->send();
    //     uv::runLoop();
    // });

    describe("client connection", []() {
        auto conn = http::Client::instance().createConnection("http://google.com/");
        // conn->Complete += sdelegate(&context, &CallbackContext::onClientConnectionComplete);
        conn->Complete += [&](const http::Response& response) {
            // std::cout << "Lerver response: "(response, )
        };
        conn->request().setKeepAlive(false);
        // conn->setReadStream(new std::stringstream);
        conn->send();
        uv::runLoop();
        expect(conn->closed());
        expect(!conn->error().any());
    });

    describe("secure client connection", []() {
        auto conn = http::Client::instance().createConnection("https://google.com/");
        // conn->Complete += sdelegate(&context, &CallbackContext::onClientConnectionComplete);
        conn->Complete += [&](const http::Response& response) {
            // std::cout << "Lerver response: "(response, )
        };
        conn->request().setKeepAlive(false);
        // conn->setReadStream(new std::stringstream);
        conn->send();
        uv::runLoop();
        expect(conn->closed());
        expect(!conn->error().any());
    });

    describe("replace connection adapter", []() {
        auto url = http::URL("https://google.com");
        auto conn = http::Client::instance().createConnection(url);
        conn->Headers += [&](http::Response& response) {
            std::cout << "On request headers: " << conn->request() << '\n';
            std::cout << "On response headers: " << response << '\n';
        };
        conn->Payload += [&](const MutableBuffer& buffer) {
            // std::cout << "On payload: " << buffer.size() << ": " << buffer.str() << endl;
        };
        conn->Complete += [&](const http::Response& response) {
            // std::cout << "On response complete: " << response << endl;
            conn->close();
        };

        conn->replaceAdapter(std::make_unique<http::ConnectionAdapter>(conn.get(), HTTP_RESPONSE));
        conn->request().setKeepAlive(false);
        conn->send();

        uv::runLoop();

        expect(conn->closed());
        expect(!conn->error().any());
    });

    //
    /// Standalone HTTP Client Connection Test
    //

    describe("standalone client connection", []() {
        http::ClientConnection conn("https://0state.com");
        conn.Headers += [&](http::Response& response) {
            // std::cout << "On response headers: " << response << endl;
        };
        conn.Payload += [&](const MutableBuffer& buffer) {
            // std::cout << "On payload: " << buffer.size() << ": " << buffer.str() << endl;
        };
        conn.Complete += [&](const http::Response& response) {
            // std::cout << "On response complete: " << response
            //     << conn.readStream<std::stringstream>().str() << endl;

            // Force connection closure if the other side hasn't already
            conn.close();
        };
        conn.setReadStream(new std::stringstream);
        conn.send(); // send default GET /

        uv::runLoop();

        expect(conn.closed());
        expect(!conn.error().any());
    });

    //
    /// Server and Client Echo Tests
    //

    describe("websocket client and server", []() {
        HTTPEchoTest test(100);
        test.raiseServer();
        auto conn = test.createConnection("ws", "/websocket");
        conn->send("PING", 4);

        uv::runLoop();

        expect(conn->closed());
        expect(!conn->error().any());
    });

    //
    /// Google Drive Upload Test
    //

    //     describe("google drive multipart upload", []() {
    //
    //         // https://developers.google.com/drive/web/manage-uploads
    //         // Need a current OAuth2 access_token with https://www.googleapis.com/auth/drive.file
    //         // access scope for this to work
    //         std::string accessToken("ya29.1.AADtN_WY53y0jEgN_SWcmfp6VvAQ6asnYqbDi5CKEfzwL7lfNqtbUiLeL4v07b_I");
    //         std::string metadata("{ \"title\": \"My File\" }");
    //
    // #if 0
    //         auto conn = http::Client::instance().createConnection("https://www.googleapis.com/drive/v2/files");
    //         conn->Complete += sdelegate(&context, &CallbackContext::onAssetUploadComplete);
    //         conn->OutgoingProgress += sdelegate(&context, &CallbackContext::onAssetUploadProgress);
    //         conn->request().setMethod("POST");
    //         conn->request().setContentType("application/json");
    //         conn->request().setContentLength(2);
    //         conn->request().add("Authorization", "Bearer " + accessToken);
    //
    //         // Send the request
    //         conn->send("{}", 2);
    // #endif
    //
    //         // Create the transaction
    //         auto conn = http::Client::instance().createConnection("https://www.googleapis.com/upload/drive/v2/files?uploadType=multipart");
    //         conn->request().setMethod("POST");
    //         conn->request().setChunkedTransferEncoding(false);
    //         conn->request().add("Authorization", "Bearer " + accessToken);
    //         conn->Complete += sdelegate(&context, &CallbackContext::onAssetUploadComplete);
    //         conn->OutgoingProgress += sdelegate(&context, &CallbackContext::onAssetUploadProgress);
    //
    //         // Attach a HTML form writer for uploading files
    //         auto form = http::FormWriter::create(*conn,
    //         http::FormWriter::ENCODING_MULTIPART_RELATED);
    //
    //         form->addPart("metadata", new http::StringPart(metadata, "application/json; charset=UTF-8"));
    //         //form->addPart("file", new http::StringPart("jew", "text/plain"));
    //         //form->addPart("file", new http::FilePart("D:/test.txt", "text/plain"));
    //         form->addPart("file", new http::FilePart("D:/test.jpg", "image/jpeg"));
    //
    //         // Send the request
    //         conn->send();
    //
    //         uv::runLoop();
    //     });


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
        NVCollection nvc;
        nvc.add("name", "sid");
        nvc.add("value", "12345");
        nvc.add("path", "/");
        nvc.add("domain", ".example.com");

        http::Cookie cookie(nvc);
        expect(cookie.getName() == "sid");
        expect(cookie.getValue() == "12345");
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
        parser.parse(raw.c_str(), raw.size());

        expect(parsed.getMethod() == "GET");
        expect(parsed.getURI() == "/api/users?page=1");
        expect(parsed.getHost() == "example.com:8080" || parsed.getHost() == "example.com");
        expect(parsed.get("User-Agent") == "Icey/2.0");
        expect(parsed.get("Accept") == "application/json");
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
        parser.parse(raw.c_str(), raw.size());

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


    // =========================================================================
    // WebSocket Frame Encoding/Decoding Tests
    //

    describe("websocket text frame encoding", []() {
        // Server-side framer (no masking)
        http::ws::WebSocketFramer framer(http::ws::ServerSide);
        // Force handshake complete state for testing
        wsFramerTestAccess(framer, 2);

        const char* payload = "Hello, WebSocket!";
        size_t payloadLen = strlen(payload);

        Buffer frameBuf;
        frameBuf.reserve(payloadLen + 14);
        BitWriter writer(frameBuf);

        framer.writeFrame(payload, payloadLen, http::ws::SendFlags::Text, writer);

        // Frame should be larger than payload (has header)
        expect(writer.position() > payloadLen);

        // First byte: FIN + Text opcode = 0x81
        expect(static_cast<uint8_t>(frameBuf[0]) == 0x81);

        // Second byte: no mask + length
        expect(static_cast<uint8_t>(frameBuf[1]) == payloadLen);

        // Read it back
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = framer.readFrame(reader, readPayload);

        expect(readLen == payloadLen);
        expect(std::string(readPayload, static_cast<size_t>(readLen)) == "Hello, WebSocket!");
    });

    describe("websocket binary frame encoding", []() {
        http::ws::WebSocketFramer framer(http::ws::ServerSide);
        wsFramerTestAccess(framer, 2);

        const char binaryData[] = {0x00, 0x01, 0x02, static_cast<char>(0xFF), static_cast<char>(0xFE)};

        Buffer frameBuf;
        frameBuf.reserve(sizeof(binaryData) + 14);
        BitWriter writer(frameBuf);

        framer.writeFrame(binaryData, sizeof(binaryData), http::ws::SendFlags::Binary, writer);

        // First byte: FIN + Binary opcode = 0x82
        expect(static_cast<uint8_t>(frameBuf[0]) == 0x82);

        // Read back
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = framer.readFrame(reader, readPayload);

        expect(readLen == sizeof(binaryData));
        expect(memcmp(readPayload, binaryData, sizeof(binaryData)) == 0);
    });

    describe("websocket client masking", []() {
        // Client-side framer applies masking
        http::ws::WebSocketFramer clientFramer(http::ws::ClientSide);
        wsFramerTestAccess(clientFramer, 2);

        // Server-side framer reads masked frames
        http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
        wsFramerTestAccess(serverFramer, 2);

        const char* payload = "Masked data";
        size_t payloadLen = strlen(payload);

        Buffer frameBuf;
        frameBuf.reserve(payloadLen + 14);
        BitWriter writer(frameBuf);

        clientFramer.writeFrame(payload, payloadLen, http::ws::SendFlags::Text, writer);

        // Mask bit should be set (second byte & 0x80)
        expect((static_cast<uint8_t>(frameBuf[1]) & 0x80) != 0);

        // Server should be able to read it (unmasking happens in readFrame)
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = serverFramer.readFrame(reader, readPayload);

        expect(readLen == payloadLen);
        expect(std::string(readPayload, static_cast<size_t>(readLen)) == "Masked data");
    });

    describe("websocket control frames", []() {
        http::ws::WebSocketFramer framer(http::ws::ServerSide);
        wsFramerTestAccess(framer, 2);

        // Write a Ping frame
        const char* pingData = "ping";
        int pingFlags = unsigned(http::ws::FrameFlags::Fin) | unsigned(http::ws::Opcode::Ping);

        Buffer frameBuf;
        frameBuf.reserve(20);
        BitWriter writer(frameBuf);
        framer.writeFrame(pingData, 4, pingFlags, writer);

        // First byte: FIN + Ping = 0x89
        expect(static_cast<uint8_t>(frameBuf[0]) == 0x89);

        // Read back
        BitReader reader(mutableBuffer(frameBuf.data(), writer.position()));
        char* readPayload = nullptr;
        uint64_t readLen = framer.readFrame(reader, readPayload);

        // Verify opcode from frameFlags
        int opcode = wsFramerGetFlags(framer) & unsigned(http::ws::Opcode::Bitmask);
        expect(opcode == unsigned(http::ws::Opcode::Ping));
        expect(readLen == 4);
    });


    test::runAll();

    http::Client::destroy();
    return test::finalize();
}
