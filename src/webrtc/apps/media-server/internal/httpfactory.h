#pragma once


#include "icy/http/server.h"

#include <memory>
#include <string>


namespace icy {
namespace media_server {


class HttpFactory : public http::ServerConnectionFactory
{
public:
    struct RuntimeConfig
    {
        bool enableTurn = true;
        uint16_t turnPort = 3478;
        std::string turnExternalIP;
        std::string host;
    };

    HttpFactory(const std::string& webRoot, RuntimeConfig runtimeConfig);

    std::unique_ptr<http::ServerResponder> createResponder(
        http::ServerConnection& conn) override;

private:
    std::unique_ptr<http::ServerResponder> createApiResponder(
        http::ServerConnection& conn);

    std::string _webRoot;
    RuntimeConfig _runtimeConfig;
};


} // namespace media_server
} // namespace icy
