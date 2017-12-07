#include <scy/crypto/hash.h>
#include "websocketserver.h"
#include "scy/http/packetizers.h"
#include "scy/http/util.h"


using namespace std;
using namespace scy;


namespace scy {
    namespace wrtc {

        WebSocketServer::WebSocketServer(const net::Address &address, ThreadSafeQueue<nlohmann::json> *detectionQueue)
                : http::Server(address, net::makeSocket<net::TCPSocket>()) {
            LDebug("Create WebSocketServer");
            _detectionQueue = detectionQueue;
            _timer = nullptr;
            this->Connection += [this](http::ServerConnection::Ptr conn) {
                SInfo << "Incoming WebSocket connection from " << conn->socket()->peerAddress()
                      << ": URI:\n" << conn->request().getURI()
                      << ": Request:\n" << conn->request() << endl;

                // this is a workaround for sending detection data from different thread that was associated with
                // WebSocket initially, it has cons that we have slight delay in sending detection data to clients, depends on timer interval
                // we start only a single timer per all WebSocket connections
                // at the moment we suppose that there will be only single webRTC source, and all clients
                // will need to receive the same object detection data
                // TODO: extend it in future to support different detection streams for different clients depends on query params
                if (this->_timer == nullptr){
                    this->_timer = new Timer(1000, 250, conn->socket()->loop());
                    this->_timer->Timeout += slot(this, &WebSocketServer::sendPayload);
                    this->_timer->start();
                }
            };
        }

        WebSocketServer::~WebSocketServer() {
            LDebug("Destroy WebSocketServer")
            if (this->_timer != nullptr)
                this->_timer->stop();
            this->shutdown();
        }


        void WebSocketServer::sendPayload() {
            if (!_connections.empty()) {
                while(!_detectionQueue->isEmpty()) {
                    LDebug("Sending detection to all connections")
                    auto detectionPayload = _detectionQueue->pop().dump().c_str();
                    for (auto &_connection : _connections) {
                        try {
                            if (!_connection->socket()->active()) {
                                _connection->close();
                                continue;
                            }
                            _connection->adapter()->send(detectionPayload,
                                                         strlen(detectionPayload),
                                                         http::ws::Text);

                        } catch (std::exception &exc) {
                            LError("Error: ", exc.what())
                            _connection->close();
                        }
                    }
                }
            }
        }
    }

} // namespace scy
