#include "icy/http/request.h"
#include "icy/net/address.h"
#include "icy/graft/graft.h"

int main()
{
    icy::http::Request request("GET", "/healthz", "HTTP/1.1");
    icy::net::Address address("127.0.0.1", 8080);
    auto plugin = icy::graft::parseRuntimeKind(icy::graft::RUNTIME_NATIVE);

    if (request.getMethod() != "GET")
        return 1;
    if (request.getURI() != "/healthz")
        return 2;
    if (!address.valid())
        return 3;
    if (plugin != icy::graft::RuntimeKind::Native)
        return 4;

    return 0;
}
