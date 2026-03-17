#include "scy/application.h"
#include "scy/net/sslmanager.h"
#include "scy/socketio/client.h"
#include "scy/socketio/transaction.h"
#include "scy/test.h"
#include "scy/util.h"


namespace scy {
namespace sockio {


#define SERVER_HOST "localhost"
#define SERVER_PORT 4444 // 443
#define USE_SSL 0        // 1


// ----------------------------------------------------------------------------
// SocketIO Client Test
//
class Tests
{
    Application app;

public:
    Tests()
    {
        testClient();
    }

    ~Tests() { app.finalize(); }

    void testClient()
    {
        sockio::Client::Options options;
        options.host = SERVER_HOST;
        options.port = SERVER_PORT;

#if USE_SSL
        sockio::SSLClient client(options, app.loop);
#else
        sockio::TCPClient client(options, app.loop);
#endif

        client.StateChange += slot(this, &Tests::onClientStateChange);
        client.connect();

        app.waitForShutdown(
            [](void* opaque) {
                static_cast<sockio::Client*>(opaque)->close();
            },
            &client);
    }

    void onClientStateChange(void* sender, sockio::ClientState& state,
                             const sockio::ClientState& oldState)
    {
        auto client = static_cast<sockio::Client*>(sender);
        LDebug("Connection state changed: ", state.toString());

        switch (state.id()) {
            case sockio::ClientState::Connecting:
                break;
            case sockio::ClientState::Connected:
                LDebug("Connected");
                break;
            case sockio::ClientState::Online:
                // Send a test event with ack
                client->send("ping", "\"hello\"", true);
                break;
            case sockio::ClientState::Error:
                LError("Error: ", client->error().message);
                break;
        }
    }
};


} // namespace sockio
} // namespace scy


int main(int argc, char** argv)
{
    using namespace scy;
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();

#if USE_SSL
    scy::net::SSLManager::initNoVerifyClient();
#endif

    // =========================================================================
    // Packet Type Strings
    //
    scy::test::describe("packet type strings", []() {
        scy::sockio::Packet pkt;
        expect(std::string(pkt.frameString()) != "");
        expect(std::string(pkt.typeString()) != "");
    });

    // =========================================================================
    // Packet Event Construction
    //
    scy::test::describe("packet event", []() {
        scy::sockio::Packet pkt(scy::sockio::Packet::Frame::Message,
                                scy::sockio::Packet::Type::Event,
                                scy::sockio::Packet::nextID(), "/",
                                "chat message", "\"hello world\"", true);

        expect(pkt.frame() == scy::sockio::Packet::Frame::Message);
        expect(pkt.type() == scy::sockio::Packet::Type::Event);
        expect(pkt.event() == "chat message");
        expect(pkt.message() == "\"hello world\"");
        expect(pkt.valid());
        expect(pkt.size() > 0);
    });

    // =========================================================================
    // Packet Write and Read Round-trip
    //
    scy::test::describe("packet write and read", []() {
        scy::sockio::Packet original(scy::sockio::Packet::Frame::Message,
                                     scy::sockio::Packet::Type::Event,
                                     -1, "/", "test", "[1,2,3]", false);

        scy::Buffer buf;
        original.write(buf);
        expect(buf.size() > 0);

        scy::sockio::Packet parsed;
        parsed.read(scy::constBuffer(buf));

        expect(parsed.frame() == scy::sockio::Packet::Frame::Message);
        expect(parsed.type() == scy::sockio::Packet::Type::Event);
    });

    // =========================================================================
    // Packet Namespace
    //
    scy::test::describe("packet namespace", []() {
        scy::sockio::Packet pkt;
        pkt.setNamespace("/chat");
        expect(pkt.nsp() == "/chat");
    });

    // =========================================================================
    // Packet ID
    //
    scy::test::describe("packet id", []() {
        scy::sockio::Packet pkt;
        pkt.setID(42);
        expect(pkt.id() == 42);
    });

    // =========================================================================
    // Packet Next ID
    //
    scy::test::describe("packet next id", []() {
        int id1 = scy::sockio::Packet::nextID();
        int id2 = scy::sockio::Packet::nextID();
        expect(id2 == id1 + 1);
    });

    // =========================================================================
    // Packet Connect Type
    //
    scy::test::describe("packet connect type", []() {
        scy::sockio::Packet pkt(scy::sockio::Packet::Frame::Message,
                                scy::sockio::Packet::Type::Connect);
        expect(pkt.frame() == scy::sockio::Packet::Frame::Message);
        expect(pkt.type() == scy::sockio::Packet::Type::Connect);
    });

    // =========================================================================
    // Packet toString
    //
    scy::test::describe("packet toString", []() {
        scy::sockio::Packet pkt(scy::sockio::Packet::Frame::Message,
                                scy::sockio::Packet::Type::Event,
                                -1, "/", "hello", "\"data\"", false);
        std::string str = pkt.toString();
        expect(!str.empty());
    });


    scy::test::runAll();

#if USE_SSL
    scy::net::SSLManager::instance().shutdown();
#endif
    scy::Logger::destroy();
    return scy::test::finalize();
}
