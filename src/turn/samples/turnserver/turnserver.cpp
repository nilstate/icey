///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// TURN Server
//
// Demonstrates an RFC 5766 TURN relay server with long-term credential
// authentication (RFC 5389). Implements the ServerObserver interface
// for auth and allocation lifecycle events.
//
// Listens on port 3478 (standard TURN port). Replace the hardcoded
// credentials with a real auth backend for production use.
//
// Test with coturn's turnutils_uclient:
//   turnutils_uclient -u username -w password 127.0.0.1
//
/// @addtogroup turn
/// @{


#include "scy/application.h"
#include "scy/crypto/hash.h"
#include "scy/turn/server/server.h"


using namespace std;
using namespace scy;
using namespace scy::net;
using namespace scy::turn;


// Standard TURN port per RFC 5766
const std::string SERVER_BIND_IP("0.0.0.0");
const int SERVER_BIND_PORT(3478);
const std::string SERVER_EXTERNAL_IP("127.0.0.1"); // Set to public IP in production

// Hardcoded credentials for demonstration - replace with a real auth backend
const std::string SERVER_USERNAME("username");
const std::string SERVER_PASSWORD("password");
const std::string SERVER_REALM("sourcey.com");


/// Implements ServerObserver to handle TURN authentication and allocation events.
/// In production you would look up credentials from a database instead of
/// using hardcoded values.
class RelayServer : public ServerObserver
{
public:
    Server server;

    RelayServer(const ServerOptions& so)
        : server(*this, so)
    {
    }

    virtual ~RelayServer() {}

    void start()
    {
        server.start();
    }

    /// Called for every incoming STUN/TURN request to decide whether to allow it.
    /// Implements the long-term credential mechanism from RFC 5389.
    virtual AuthenticationState authenticateRequest(Server*, Request& request)
    {
        LDebug("Authenticating: ", request.transactionID());

        // The authentication information (e.g., username, password, realm, and
        // nonce) is used to both verify subsequent requests and to compute the
        // message integrity of responses.  The username, realm, and nonce
        // values are initially those used in the authenticated Allocate request
        // that creates the allocation, though the server can change the nonce
        // value during the lifetime of the allocation using a 438 (Stale Nonce)
        // reply.  Note that, rather than storing the password explicitly, for
        // security reasons, it may be desirable for the server to store the key
        // value, which is an MD5 hash over the username, realm, and password
        // (see [RFC5389]).

        // Indications and binding requests bypass auth (can't be challenged)
        // Note that the long-term credential mechanism cannot be used to
        // protect indications, since indications cannot be challenged. Usages
        // utilizing indications must either use a short-term credential or omit
        // authentication and message integrity for them.
        if (request.methodType() == stun::Message::SendIndication ||
            request.methodType() == stun::Message::Binding)
            return AuthenticationState::Authorized;

        // If required auth attributes are missing, respond with 401
        // The initial packet from the client does not include the USERNAME,
        // REALM, NONCE,
        // or MESSAGE-INTEGRITY attributes. If these attributes are not provided
        // we return
        // a 401 (Unauthorized) response.
        auto usernameAttr = request.get<stun::Username>();
        auto realmAttr = request.get<stun::Realm>();
        auto nonceAttr = request.get<stun::Nonce>();
        auto integrityAttr = request.get<stun::MessageIntegrity>();
        if (!usernameAttr || !realmAttr || !nonceAttr || !integrityAttr) {
            LDebug("Authenticating: Unauthorized STUN Request");
            return turn::AuthenticationState::NotAuthorized;
        }

        // Compute the MD5 key = username:realm:password per RFC 5389
        // Determine authentication status and return either Authorized,
        // Unauthorized or Authenticating.
        std::string credentials(SERVER_USERNAME + ":" + SERVER_REALM + ":" + SERVER_PASSWORD);
        crypto::Hash engine("md5");
        engine.update(credentials);
        request.hash = engine.digestStr();

#if ENABLE_AUTHENTICATION
        SDebug << "Generating HMAC: data=" << credentials
               << ", key=" << request.hash << endl;

        // Verify the message integrity HMAC against our computed key
        if (integrityAttr->verifyHmac(request.hash))
            return turn::AuthenticationState::Authorized;
        return turn::AuthenticationState::NotAuthorized;
#else
        // Since no authentication is required we just return Authorized.
        return turn::AuthenticationState::Authorized;
#endif
    }

    virtual void onServerAllocationCreated(Server*, IAllocation* alloc)
    {
        LDebug("Allocation Created");
    }

    virtual void onServerAllocationRemoved(Server*, IAllocation* alloc)
    {
        LDebug("Allocation Removed");
    }
};


int main(void)
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    Logger::instance().setWriter(std::make_unique<AsyncLogWriter>());
    {
        Application app;
        {
            // Configure the TURN server options
            ServerOptions opts;
            opts.software = "Sourcey STUN/TURN Server [rfc5766]";
            opts.realm = "sourcey.com";
            opts.listenAddr = net::Address(SERVER_BIND_IP, SERVER_BIND_PORT);
            opts.externalIP = SERVER_EXTERNAL_IP;
            opts.allocationDefaultLifetime = 2 * 60 * 1000;   // 2 minutes
            opts.allocationMaxLifetime = 10 * 60 * 1000;      // 10 minutes
            opts.timerInterval = 5 * 1000;
            // opts.enableUDP = false;

            RelayServer srv(opts);
            srv.start();

            // Block until SIGINT/SIGTERM, then stop the server
            app.waitForShutdown([](void* opaque) {
                reinterpret_cast<RelayServer*>(opaque)->server.stop();
            },
                                &srv);
        }
    }
    Logger::destroy();
    return 0;
}
