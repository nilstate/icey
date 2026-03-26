#include "turnserver.h"

#include "icy/crypto/hash.h"
#include "icy/logger.h"


namespace icy {
namespace media_server {
namespace {

constexpr const char* kDemoTurnUsername = "icey";
constexpr const char* kDemoTurnCredential = "icey";

} // namespace


EmbeddedTurn::EmbeddedTurn(const Config& config)
    : _realm(config.turnRealm)
{
    turn::ServerOptions opts;
    opts.software = "icey Media Server TURN [rfc5766]";
    opts.realm = _realm;
    opts.listenAddr = net::Address(config.host, config.turnPort);
    opts.externalIP = config.turnExternalIP;
    opts.enableTCP = true;
    opts.enableUDP = true;
    opts.enableLocalIPPermissions = true;

    _server = std::make_unique<turn::Server>(*this, opts);
}


void EmbeddedTurn::start()
{
    _server->start();
}


void EmbeddedTurn::stop()
{
    _server->stop();
}


turn::AuthenticationState EmbeddedTurn::authenticateRequest(
    turn::Server*, turn::Request& request)
{
    if (request.methodType() == stun::Message::Binding ||
        request.methodType() == stun::Message::SendIndication) {
        return turn::AuthenticationState::Authorized;
    }

    auto usernameAttr = request.get<stun::Username>();
    auto realmAttr = request.get<stun::Realm>();
    auto nonceAttr = request.get<stun::Nonce>();
    auto integrityAttr = request.get<stun::MessageIntegrity>();
    if (!usernameAttr || !realmAttr || !nonceAttr || !integrityAttr) {
        LDebug("TURN auth challenge: method=",
               request.methodString(),
               " user=",
               usernameAttr ? usernameAttr->asString() : "<none>",
               " realm=",
               realmAttr ? realmAttr->asString() : "<none>",
               " nonce=",
               nonceAttr ? "<present>" : "<none>",
               " integrity=",
               integrityAttr ? "<present>" : "<none>");
        return turn::AuthenticationState::NotAuthorized;
    }

    const std::string username = usernameAttr->asString();
    const std::string realm = realmAttr->asString();
    if (username != kDemoTurnUsername || realm != _realm) {
        LWarn("TURN auth mismatch: user=", username, " realm=", realm);
        return turn::AuthenticationState::NotAuthorized;
    }

    crypto::Hash engine("md5");
    engine.update(username + ":" + realm + ":" + kDemoTurnCredential);
    request.hash = engine.digestStr();

    const bool ok = integrityAttr->verifyHmac(request.hash);
    if (!ok)
        LWarn("TURN integrity check failed for user=", username);
    return ok ? turn::AuthenticationState::Authorized
              : turn::AuthenticationState::NotAuthorized;
}


void EmbeddedTurn::onServerAllocationCreated(
    turn::Server*, turn::IAllocation*)
{
    LInfo("TURN allocation created");
}


void EmbeddedTurn::onServerAllocationRemoved(
    turn::Server*, turn::IAllocation*)
{
    LDebug("TURN allocation removed");
}


} // namespace media_server
} // namespace icy
