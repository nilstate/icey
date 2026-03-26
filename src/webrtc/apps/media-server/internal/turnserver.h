#pragma once


#include "config.h"
#include "icy/turn/server/server.h"

#include <memory>
#include <string>


namespace icy {
namespace media_server {


class EmbeddedTurn : public turn::ServerObserver
{
public:
    explicit EmbeddedTurn(const Config& config);

    void start();
    void stop();

    turn::AuthenticationState authenticateRequest(
        turn::Server*, turn::Request& request) override;

    void onServerAllocationCreated(
        turn::Server*, turn::IAllocation* alloc) override;

    void onServerAllocationRemoved(
        turn::Server*, turn::IAllocation* alloc) override;

private:
    std::string _realm;
    std::unique_ptr<turn::Server> _server;
};


} // namespace media_server
} // namespace icy
