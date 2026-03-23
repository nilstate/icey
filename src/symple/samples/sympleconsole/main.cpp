///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Symple Console Client
//
// Interactive console client for the Symple real-time messaging protocol.
// Demonstrates client connection, message routing, presence, and room
// management over native WebSocket.
//
// Requires a running Symple server (https://github.com/nicedoc/symple-server).
//
// Examples:
//   sympleconsole -host localhost -port 4500 -token <token> -user 42 -name Somedude
//
/// @addtogroup symple
/// @{


#include "icy/application.h"
#include "icy/filesystem.h"
#include "icy/ipc.h"
#include "icy/net/sslmanager.h"
#include "icy/symple/client.h"
#include "icy/util.h"

#include <iostream>
#include <stdexcept>
#include <string_view>


using namespace icy;

#define USE_SSL 0


/// Wraps the Symple client with a console UI for interactive messaging.
/// Uses an IPC sync queue to safely pass user input from the console thread
/// to the libuv event loop thread.
class SympleApplication : public icy::Application
{
public:
    icy::smpl::Client client;
    icy::ipc::SyncQueue<> ipc;  // thread-safe bridge: console thread -> event loop
    bool showHelp;
    bool shuttingDown;

    SympleApplication()
        : showHelp(false)
        , shuttingDown(false)
    {
    }

    virtual ~SympleApplication() {}

    void printHelp()
    {
        std::cout
            << "\nSymple Console Client v0.1.0"
               "\n(c) Icey"
               "\nhttps://0state.com/symple"
               "\n"
               "\nGeneral options:"
               "\n  -help           Print help"
               "\n  -logfile        Log file path"
               "\n"
               "\nClient options:"
               "\n  -host           Symple server hostname or IP address"
               "\n  -port           Symple server port"
               "\n  -token          Session token to authenticate with"
               "\n  -user           User ID to register on the server with"
               "\n  -name           Display name to register on the server with"
               "\n  -type           User type to register on the server with (optional)"
            << '\n';
    }

    void parseOptions(int argc, char* argv[])
    {
        OptionParser optparse(argc, argv, "-");
        for (auto& kv : optparse.args) {
            const std::string& key = kv.first;
            const std::string& value = kv.second;
            LDebug("Setting option: ", key, ": ", value);

            if (key == "help") {
                showHelp = true;
            } else if (key == "host") {
                client.options().host = value;
            } else if (key == "port") {
                client.options().port = icy::util::strtoi<uint16_t>(value);
            } else if (key == "token") {
                client.options().token = value;
            } else if (key == "user") {
                client.options().user = value;
            } else if (key == "name") {
                client.options().name = value;
            } else if (key == "type") {
                client.options().type = value;
            } else if (key == "logfile") {
                auto log = dynamic_cast<icy::FileChannel*>(icy::Logger::instance().get("Symple"));
                log->setPath(value);
            } else {
                LWarn("Unknown option: ", key, "=", value);
            }
        }
    }

    void shutdown()
    {
        if (shuttingDown)
            return;
        shuttingDown = true;

        ipc.close();
        client.close();
        Application::stop();
    }

    void start()
    {
        try {
            // Print help
            if (showHelp) {
                printHelp();
                return;
            }

            // Wire up signal handlers for incoming packets and client lifecycle events
            client += packetSlot(this, &SympleApplication::onRecvMessage);
            client += packetSlot(this, &SympleApplication::onRecvPresence);
            client += packetSlot(this, &SympleApplication::onRecvEvent);
            client.Announce += slot(this, &SympleApplication::onClientAnnounce);
            client.StateChange += slot(this, &SympleApplication::onClientStateChange);
            client.CreatePresence += slot(this, &SympleApplication::onCreatePresence);
            client.connect();

            // Console input runs on a separate thread because std::getchar()
            // blocks, and we can't block the libuv event loop
            icy::Thread console([](void* arg) {
                auto app = static_cast<SympleApplication*>(arg);

                char o = 0;
                while (o != 'Q') {
                    std::cout << "COMMANDS:\n"
                                 "  M    Send a message.\n"
                                 "  J    Join a room.\n"
                                 "  L    Leave a room.\n"
                                 "  C    Print contacts list.\n"
                                 "  Q    Quit.\n";

                    o = char(toupper(std::getchar()));
                    std::cin.ignore();

                    // Send a message
                    if (o == 'M') {
                        std::cout << "Compose your message: " << '\n';
                        std::string data;
                        std::getline(std::cin, data);

                        auto message = new icy::smpl::Message();
                        message->setData("text", std::string_view(data));

                        std::cout << "Sending message: " << data << '\n';

                        // Push to IPC queue so the send happens on the event loop thread
                        app->ipc.push(new icy::ipc::Action(
                            [app](const icy::ipc::Action& a) { app->onSyncMessage(a); },
                            message));
                    }

                    // Join a room
                    else if (o == 'J') {
                        std::cout << "Join a room: " << '\n';
                        auto data = new std::string();
                        std::getline(std::cin, *data);

                        app->ipc.push(new icy::ipc::Action(
                            [app](const icy::ipc::Action& a) { app->onSyncCommand(a); },
                            data, "join"));
                    }

                    // Leave a room
                    else if (o == 'L') {
                        std::cout << "Leave a room: " << '\n';
                        auto data = new std::string();
                        std::getline(std::cin, *data);

                        app->ipc.push(new icy::ipc::Action(
                            [app](const icy::ipc::Action& a) { app->onSyncCommand(a); },
                            data, "leave"));
                    }

                    // List contacts
                    else if (o == 'C') {
                        std::cout << "Listing contacts:" << '\n';
                        app->ipc.push(new icy::ipc::Action(
                            [app](const icy::ipc::Action& a) { app->onSyncCommand(a); },
                            nullptr, "contacts"));
                    }
                }

                std::cout << "Quiting" << '\n';
                app->ipc.push(new icy::ipc::Action(
                    [app](const icy::ipc::Action& a) { app->onSyncCommand(a); },
                    nullptr, "quit"));
            },
                                this);

            // Run the event loop
            waitForShutdown([](void* opaque) {
                static_cast<SympleApplication*>(opaque)->shutdown();
            },
                            this);
            Application::finalize();
        } catch (std::exception& exc) {
            std::cerr << "Symple runtime error: " << exc.what() << '\n';
        }
    }

    /// Called on the event loop thread when a message arrives via IPC.
    void onSyncMessage(const icy::ipc::Action& action)
    {
        auto message = static_cast<icy::smpl::Message*>(action.arg);

        client.send(*message);

        delete message;
    }

    void onSyncCommand(const icy::ipc::Action& action)
    {
        auto arg = static_cast<std::string*>(action.arg);

        if (action.data == "join") {
            client.joinRoom(*arg);
        } else if (action.data == "leave") {
            client.leaveRoom(*arg);
        } else if (action.data == "contacts") {
            client.roster().print(std::cout);
            std::cout << '\n';
        } else if (action.data == "quit") {
            shutdown();
        }

        delete arg;
    }

    void onRecvMessage(icy::smpl::Message& message)
    {
        LDebug("####### On message: ", message.className());

        // Handle incoming Symple messages here
    }

    void onRecvPresence(icy::smpl::Presence& presence)
    {
        LDebug("####### On presence: ", presence.className());

        // Handle incoming Symple presences here
    }

    void onRecvEvent(icy::smpl::Event& event)
    {
        LDebug("####### On event: ", event.className());

        // Handle incoming Symple events here
    }

    void onClientAnnounce(const int& status)
    {
        LDebug("####### On announce: ", status);
        if (status != 200)
            std::cerr << "Announce failed with status: " << status << '\n';
    }

    void onClientStateChange(void*, icy::smpl::ClientState& state, const icy::smpl::ClientState& oldState)
    {
        SDebug << "Client state changed: " << oldState << " -> " << state;

        switch (state.id()) {
            case icy::smpl::ClientState::Connecting:
                break;
            case icy::smpl::ClientState::Authenticating:
                break;
            case icy::smpl::ClientState::Online:
                std::cout << "Client online as " << client.ourID() << '\n';

                // Join the public room
                client.joinRoom("public");
                break;
            case icy::smpl::ClientState::Error:
                std::cout << "Client disconnected" << '\n';
                break;
            case icy::smpl::ClientState::Closed:
                break;
        }
    }

    void onCreatePresence(icy::smpl::Peer& peer)
    {
        LDebug("####### Updating presence data");

        // Update the peer object to be broadcast with presence.
        // Any arbitrary data can be broadcast with presence.
        peer["agent"] = "SympleConsole";
        peer["version"] = "0.1.0";
    }
};


int main(int argc, char** argv)
{
    // Setup the logger
    icy::Logger::instance().add(std::make_unique<icy::ConsoleChannel>("debug", icy::Level::Trace));

    // Init SSL client context
#if USE_SSL
    icy::net::SSLManager::initNoVerifyClient();
#endif

    // Run the application
    {
        SympleApplication app;
        app.parseOptions(argc, argv);
        app.start();
    }

    // Cleanup all singletons
#if USE_SSL
    icy::net::SSLManager::destroy();
#endif
    icy::Logger::destroy();
    return 0;
}
