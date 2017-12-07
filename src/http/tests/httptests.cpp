#include "httptests.h"


using std::endl;
using namespace scy;
using namespace scy::test;


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
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

        http::URL url2("HTTP", "localhost", "/home/sourcey/foo.bar");
        expect(url2.scheme() == "http");
        expect(url2.host() == "localhost");
        expect(url2.path() == "/home/sourcey/foo.bar");

        http::URL url3("http", "www.sourcey.com", "/index.html");
        expect(url3.scheme() == "http");
        expect(url3.authority() == "www.sourcey.com");
        expect(url3.host() == "www.sourcey.com");
        expect(url3.path() == "/index.html");

        http::URL url4("http", "www.sourcey.com:8000", "/index.html");
        expect(url4.scheme() == "http");
        expect(url4.authority() == "www.sourcey.com:8000");
        expect(url4.host() == "www.sourcey.com");
        expect(url4.path() == "/index.html");

        http::URL url5("http", "user@www.sourcey.com:8000", "/index.html");
        expect(url5.scheme() == "http");
        expect(url5.userInfo() == "user");
        expect(url5.host() == "www.sourcey.com");
        expect(url5.port() == 8000);
        expect(url5.authority() == "user@www.sourcey.com:8000");
        expect(url5.path() == "/index.html");

        http::URL url6("http", "user@www.sourcey.com:80", "/index.html");
        expect(url6.scheme() == "http");
        expect(url6.userInfo() == "user");
        expect(url6.host() == "www.sourcey.com");
        expect(url6.port() == 80);
        expect(url6.authority() == "user@www.sourcey.com:80");
        expect(url6.path() == "/index.html");

        http::URL url7("http", "www.sourcey.com", "/index.html", "query=test", "fragment");
        expect(url7.scheme() == "http");
        expect(url7.authority() == "www.sourcey.com");
        expect(url7.path() == "/index.html");
        expect(url7.pathEtc() == "/index.html?query=test#fragment");
        expect(url7.query() == "query=test");
        expect(url7.fragment() == "fragment");

        http::URL url8("http", "www.sourcey.com", "/index.html?query=test#fragment");
        expect(url8.scheme() == "http");
        expect(url8.authority() == "www.sourcey.com");
        expect(url8.path() == "/index.html");
        expect(url8.pathEtc() == "/index.html?query=test#fragment");
        expect(url8.query() == "query=test");
        expect(url8.fragment() == "fragment");
    });

    //
    /// HTTP URL Parameters Tests
    //

    describe("url query parameters", []() {
        NVCollection params;
        http::splitURIParameters("/streaming?format=MJPEG&width=400&height=300&"
            "encoding=Base64&packetizer=chunked&"
            "rand=0.09983996045775712", params);
        for (NVCollection::ConstIterator it = params.begin(); it != params.end(); ++it) {
            // std::cout << "URL Parameter: " << it->first << ": " << it->second << endl;
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
        std::string path(SCY_BUILD_DIR);
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
             std::cout << "On request headers: " << conn->request() << endl;
             std::cout << "On response headers: " << response << endl;
        };
        conn->Payload += [&](const MutableBuffer& buffer) {
            // std::cout << "On payload: " << buffer.size() << ": " << buffer.str() << endl;
        };
        conn->Complete += [&](const http::Response& response) {
            // std::cout << "On response complete: " << response << endl;
            conn->close();
        };

        conn->replaceAdapter(new http::ConnectionAdapter(conn.get(), HTTP_RESPONSE));
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
        http::ClientConnection conn("https://sourcey.com");
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

    test::runAll();

    http::Client::destroy();
    return test::finalize();
}
