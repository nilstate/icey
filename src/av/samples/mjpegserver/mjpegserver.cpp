///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// MJPEG Streaming Server
//
// Demonstrates HTTP MJPEG streaming using the AV pipeline. Each connected
// client gets an independent encoding pipeline: shared capture source ->
// MJPEG encoder -> multipart HTTP framing -> socket.
//
// Set USE_AVDEVICE_CAPTURE to 1 to stream from a camera, or leave at 0
// to stream from the bundled test file (no hardware required).
//
// Test: open http://localhost:328 in a browser that supports MJPEG streams.
//
/// @addtogroup av
/// @{


#include "scy/application.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/av/mediacapture.h"
#include "scy/http/packetizers.h"
#include "scy/http/server.h"
#include "scy/logger.h"
#include "scy/packetstream.h"

#include <iostream>
#include <memory>


using namespace scy;

#define USE_AVDEVICE_CAPTURE 0

#if USE_AVDEVICE_CAPTURE
#include "scy/av/videocapture.h"
#endif


/// Shared video capture source. All client connections read from this
/// single instance, which is thread-safe.
static std::shared_ptr<av::MediaCapture> gCapture;


/// Each HTTP connection gets its own MJPEGResponder, which builds an
/// independent encoding pipeline from the shared capture source.
class MJPEGResponder : public http::ServerResponder
{
public:
    MJPEGResponder(http::ServerConnection& conn)
        : http::ServerResponder(conn)
        , _stream("mjpeg")
    {
        // Bypass default HTTP headers; the multipart adapter sends its own
        conn.shouldSendHeader(false);

        // All connections share the same capture instance
        _stream.attachSource(gCapture.get(), false, true);

        // Configure MJPEG output at 400x300
        av::EncoderOptions options;
        options.oformat = av::Format(
            "MJPEG", "mjpeg",
            av::VideoCodec("mjpeg", 400, 300, 25));
        gCapture->getEncoderFormat(options.iformat);

        // Encoder converts raw frames to MJPEG
        auto encoder = new av::MultiplexPacketEncoder(options);
        encoder->init();
        _stream.attach(encoder, 5, true);

        // Multipart adapter wraps each JPEG frame with HTTP boundaries
        // so the browser renders it as a live MJPEG stream
        auto packetizer = new http::MultipartAdapter("image/jpeg");
        _stream.attach(packetizer, 10, true);

        // Send encoded packets to the HTTP client
        _stream.emitter += packetSlot(this, &MJPEGResponder::onPacket);
        _stream.start();

        std::cout << "Client connected" << std::endl;
    }

    ~MJPEGResponder()
    {
        _stream.emitter -= packetSlot(this, &MJPEGResponder::onPacket);
        _stream.stop();
        std::cout << "Client disconnected" << std::endl;
    }

    void onClose() override
    {
        _stream.stop();
    }

    void onPacket(RawPacket& packet)
    {
        try {
            connection().send(packet.data(), packet.size());
        }
        catch (std::exception& exc) {
            std::cerr << "Send error: " << exc.what() << std::endl;
            connection().close();
        }
    }

private:
    PacketStream _stream;
};


/// Factory that creates an MJPEGResponder for each new HTTP connection.
class MJPEGConnectionFactory : public http::ServerConnectionFactory
{
public:
    std::unique_ptr<http::ServerResponder> createResponder(http::ServerConnection& conn) override
    {
        return std::make_unique<MJPEGResponder>(conn);
    }
};


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

#if USE_AVDEVICE_CAPTURE
    // Stream from the default camera
    auto capture = std::make_shared<av::VideoCapture>();
    capture->openVideo("0", 640, 480);
    gCapture = capture;
#else
    // Stream from a video file (no hardware required)
    gCapture = std::make_shared<av::MediaCapture>();
    gCapture->openFile(SCY_DATA_DIR "/test.mp4");
    gCapture->setLoopInput(true);
    gCapture->setLimitFramerate(true);
#endif

    gCapture->start();

    http::Server server("0.0.0.0", 328,
                        net::makeSocket<net::TCPSocket>(),
                        std::make_unique<MJPEGConnectionFactory>());
    server.start();

    std::cout << "MJPEG server listening on http://localhost:328" << std::endl;

    waitForShutdown([](void* opaque) {
        reinterpret_cast<http::Server*>(opaque)->shutdown();
    }, &server);

    gCapture->stop();
    gCapture.reset();

    Logger::destroy();
    return 0;
}
