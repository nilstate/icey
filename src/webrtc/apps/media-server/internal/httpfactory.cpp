#include "httpfactory.h"

#include "icy/json/json.h"

#include <fstream>


namespace icy {
namespace media_server {
namespace {

constexpr const char* kDemoTurnUsername = "icey";
constexpr const char* kDemoTurnCredential = "icey";


class StaticFileResponder : public http::ServerResponder
{
public:
    StaticFileResponder(http::ServerConnection& conn, const std::string& webRoot)
        : http::ServerResponder(conn)
        , _webRoot(webRoot)
    {
    }

    void onRequest(http::Request& request, http::Response& response) override
    {
        std::string path = request.getURI();

        if (path == "/" || path.empty())
            path = "/index.html";

        if (path.find("..") != std::string::npos) {
            response.setStatus(http::StatusCode::Forbidden);
            connection().sendHeader();
            return;
        }

        std::string filePath = _webRoot + path;
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            response.setStatus(http::StatusCode::NotFound);
            connection().sendHeader();
            return;
        }

        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string body(static_cast<size_t>(size), '\0');
        file.read(body.data(), size);

        response.setContentType(mimeType(path));
        response.setContentLength(body.size());
        connection().sendHeader();
        connection().send(body.data(), body.size());
    }

private:
    static std::string mimeType(const std::string& path)
    {
        auto dot = path.rfind('.');
        if (dot == std::string::npos)
            return "application/octet-stream";
        auto ext = path.substr(dot + 1);
        if (ext == "html") return "text/html; charset=utf-8";
        if (ext == "js") return "application/javascript; charset=utf-8";
        if (ext == "css") return "text/css; charset=utf-8";
        if (ext == "json") return "application/json";
        if (ext == "png") return "image/png";
        if (ext == "svg") return "image/svg+xml";
        if (ext == "ico") return "image/x-icon";
        if (ext == "woff2") return "font/woff2";
        if (ext == "woff") return "font/woff";
        return "application/octet-stream";
    }

    std::string _webRoot;
};

} // namespace


HttpFactory::HttpFactory(const std::string& webRoot, RuntimeConfig runtimeConfig)
    : _webRoot(webRoot)
    , _runtimeConfig(std::move(runtimeConfig))
{
}


std::unique_ptr<http::ServerResponder> HttpFactory::createResponder(
    http::ServerConnection& conn)
{
    auto& uri = conn.request().getURI();

    if (uri.substr(0, 5) == "/api/")
        return createApiResponder(conn);

    return std::make_unique<StaticFileResponder>(conn, _webRoot);
}


std::unique_ptr<http::ServerResponder> HttpFactory::createApiResponder(
    http::ServerConnection& conn)
{
    class ApiResponder : public http::ServerResponder
    {
    public:
        ApiResponder(http::ServerConnection& conn,
                     RuntimeConfig runtimeConfig)
            : http::ServerResponder(conn)
            , _runtimeConfig(std::move(runtimeConfig))
        {
        }

        void onRequest(http::Request& request, http::Response& response) override
        {
            json::Value j;
            if (request.getURI() == "/api/config") {
                j["status"] = "ok";
                j["turn"]["enabled"] = _runtimeConfig.enableTurn;
                j["turn"]["host"] = _runtimeConfig.turnExternalIP.empty()
                    ? _runtimeConfig.host
                    : _runtimeConfig.turnExternalIP;
                j["turn"]["port"] = _runtimeConfig.turnPort;
                j["turn"]["username"] = kDemoTurnUsername;
                j["turn"]["credential"] = kDemoTurnCredential;
                j["stun"]["urls"] = json::Value::array({
                    "stun:stun.l.google.com:19302"
                });
            }
            else {
                j["status"] = "ok";
                j["version"] = "1.0.0";
            }
            auto body = j.dump();
            response.setContentType("application/json");
            response.setContentLength(body.size());
            connection().sendHeader();
            connection().send(body.data(), body.size());
        }

    private:
        RuntimeConfig _runtimeConfig;
    };

    return std::make_unique<ApiResponder>(conn, _runtimeConfig);
}


} // namespace media_server
} // namespace icy
