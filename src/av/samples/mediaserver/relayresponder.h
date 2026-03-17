#include "mediaserver.h"

#include "scy/streammanager.h"
#include "scy/turn/client/tcpclient.h"


namespace scy {


#define RELAY_USERNAME "user"
#define RELAY_PASSWORD "illusion"


//
// Relayed Streaming Client Allocation
//


class RelayedStreamingAllocation : public turn::TCPClientObserver
{
public:
    turn::TCPClient client;
    StreamManager streams;
    StreamingOptions options;
    std::string peerIP;
    int frameNumber;
    bool connected;
    bool deleted;

    Signal<void(turn::Client&)> AllocationCreated;
    // Signal<turn::Client&> AllocationFailed;
    Signal<void(turn::Client&, const net::Address&)> ConnectionCreated;

    RelayedStreamingAllocation(const StreamingOptions& options,
                               const turn::Client::Options& clientOptions,
                               const std::string& peerIP)
        : client(*this, clientOptions)
        , options(options)
        , peerIP(peerIP)
        , frameNumber(0)
        , connected(false)
        , deleted(false)
    {
    }

    virtual ~RelayedStreamingAllocation()
    {
        assert(streams.empty());
        assert(deleted); // NOTE: We are responsible for deleting the audioCapture.
        // if (options.audioCapture)    //    delete options.audioCapture;
    }

    void initiate()
    {
        LDebug("Initiating");
        try {
            // Initiate the TRUN client allocation
            client.addPermission(peerIP);
            client.addPermission("127.0.0.1");   // for proxy
            client.addPermission("192.168.1.1"); // for proxy
            client.initiate();
        } catch (std::exception& exc) {
            LError("Error: ", exc.what());
            assert(0);
        }
    }

    void dispose()
    {
        LDebug("Terminating");
        if (deleted)
            ;
        {
            // assert(0);
            return;
        }

        client.shutdown();

        // Free all managed packet streams
        streams.closeAll();

        // Destroy the client when the allocation is lost
        delete this;
        deleted = true;
    }

protected:
    void onClientStateChange(turn::Client&, turn::ClientState& state, const turn::ClientState&)
    {
        LDebug("Relay state changed: ", state.toString());

        switch (state.id()) {
            case turn::ClientState::None:
                break;
            case turn::ClientState::Allocating:
                break;
            case turn::ClientState::Authorizing:
                break;
            case turn::ClientState::Success:
                AllocationCreated.emit(this->client);
                break;
            case turn::ClientState::Failed:
                // assert(0 && "Allocation failed");
                LWarn("Relay connection lost");
                // AllocationFailed.emit(this->client);
                // dispose();
                break;
                // case turn::ClientState::Terminated:
                //    break;
        }
    }

    void onRelayConnectionCreated(turn::TCPClient&, const net::TCPSocket::Ptr& socket,
                                  const net::Address& peerAddr) // uint32_t connectionID,
    {
        LDebug("Connection created: ", peerAddr);
        // Just allow one stream for now
        if (this->streams.size() == 1) {
            LDebug("Rejecting connection");
            return;
        }

        try {
            // Notify the outside application
            ConnectionCreated.emit(client, peerAddr);

            // Create an output media stream for the new connection
            auto stream = new PacketStream(peerAddr.toString());

            // Setup the packet stream ensuring the audio capture isn't
            // destroyed with the stream, as it may be reused while the
            // allocation is active.
            // NOTE: Setting freeCaptures to true for testing.
            MediaServer::setupPacketStream(*stream, options, true, true);

            // Feed the packet stream directly into the connection
            stream->emitter += packetSlot(reinterpret_cast<net::SocketAdapter*>(socket.get()),
                                          &net::SocketAdapter::sendPacket);
            //stream->emitter += packetSlot(static_cast<net::SocketAdapter*>(
            //                                  const_cast<net::TCPSocket*>(socket.get())),
            //                                      &net::SocketAdapter::sendPacket);

            // Start the stream
            stream->start();

            this->streams.addStream(stream);
        } catch (std::exception& exc) {
            LError("Stream error: ", exc.what());
            assert(0);
        }
    }

    void onRelayConnectionClosed(turn::TCPClient&, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress)
    {
        LDebug("Connection closed: ", peerAddress);

        try {
            // Destroy the media stream for the closed connection (if any).
            // this->streams.free(peerAddress.toString());
            PacketStream* stream = streams.remove(peerAddress.toString());
            if (stream) {
                stream->emitter -= packetSlot(reinterpret_cast<net::SocketAdapter*>(socket.get()),
                                              &net::SocketAdapter::sendPacket);
                //stream->emitter -= packetSlot(static_cast<net::SocketAdapter*>(
                //                                  const_cast<net::TCPSocket*>(socket.get())),
                //                                      &net::SocketAdapter::sendPacket);
                delete stream;
                // stream->destroy();
            }
        } catch (std::exception& exc) {
            LError("Stream error: ", exc.what());
            assert(0);
        }

        if (streams.empty())
            dispose();
    }

    void onRelayDataReceived(turn::Client&, const char* data, std::size_t size, const net::Address& peerAddr)
    {
        SDebug << "Received data from peer: " << std::string(data, size)
               << ": " << peerAddr << std::endl;
        // If the remove peer is a web browser then the HTTP request sent
        // to the relayed address will be the first thing we see here...
    }

    void onAllocationPermissionsCreated(turn::Client&, const turn::PermissionList& permissions)
    {
        LDebug("Permissions created");
    }
};


//
// Relayed Streaming Connection Handler
//


class RelayedStreamingResponder : public http::ServerResponder
{
public:
    RelayedStreamingResponder(http::ServerConnection& connection, const StreamingOptions& options)
        : http::ServerResponder(connection)
        , options(options)
        , allocation(nullptr)
    {
    }

    virtual ~RelayedStreamingResponder()
    {
        // Note: The RelayedStreamingAllocation is self managing
    }

    void onRequest(http::Request& request, http::Response& response)
    {
        SDebug << "Running: "
               << "\n\tOutput Format: " << options.oformat.name
               << "\n\tOutput Encoding: " << options.encoding
               << "\n\tOutput Packetizer: " << options.framing
               << std::endl;

        turn::Client::Options co;
        co.serverAddr = net::Address(kRelayServerIP, 3478);
        co.lifetime = 120 * 1000; // 2 minutes
        co.timeout = 10 * 1000;
        co.timerInterval = 3 * 1000;
        co.username = RELAY_USERNAME;
        co.password = RELAY_PASSWORD;

        allocation = new RelayedStreamingAllocation(
            options, co, connection().socket()->peerAddress().host());
        // allocation->AllocationFailed += sdelegate(this, &RelayedStreamingResponder::onAllocationFailed);
        allocation->AllocationCreated += slot(this, &RelayedStreamingResponder::onAllocationCreated);

        allocation->initiate();
    }

    void onAllocationCreated(turn::Client&)
    {
        allocation->AllocationCreated -= slot(this, &RelayedStreamingResponder::onAllocationCreated);
        std::string address(allocation->client.relayedAddress().toString());

        LDebug("Allocation Created: ", address);
        // Send the relay address response to the initiator
        connection()
            .response()
            .set("Access-Control-Allow-Origin", "*");
        connection().send(address.c_str(), address.length());
        connection().close();
    }

    // void onAllocationFailed(void* sender, turn::Client&)
    // {
    //     allocation->AllocationFailed -= sdelegate(this, &RelayedStreamingResponder::onAllocationFailed);
    //
    //     LDebug("Allocation Failed");
    //
    //     // Send the relay address response to the initiator
    //     //connection().socket()->send(address.c_str(), address.length());
    //     //connection().response().set("Access-Control-Allow-Origin", "*");
    //
    //     connection().close();
    // }

    StreamingOptions options;
    RelayedStreamingAllocation* allocation;
    av::FPSCounter fpsCounter;
};


} // namespace scy
