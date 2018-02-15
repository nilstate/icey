#include "mediaserver.h"
#include "relayresponder.h"
#include "snapshotresponder.h"
#include "streamingresponder.h"
#include "websocketresponder.h"
#include "scy/av/formatregistry.h"
#include "scy/av/devicemanager.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/collection.h"

#include "scy/http/packetizers.h"
#include "scy/http/util.h"
#include "scy/util/base64packetencoder.h"


using namespace std;
using namespace scy;


namespace scy {


//
// HTTP Media Server
//


MediaServer::MediaServer(uint16_t port)
    : http::Server(net::Address("0.0.0.0", port), 
                   net::makeSocket<net::TCPSocket>(),
                   new HTTPStreamingConnectionFactory(this))
{
    LDebug("Create")

    // Register the media formats we will be using
    // FormatRegistry& formats = MediaFactory::instance().formats();

    // Adobe Flash Player requires that audio files be 16bit and have a sample
    // rate of 44.1khz.
    // Flash Player can handle MP3 files encoded at 32kbps, 48kbps, 56kbps,
    // 64kbps, 128kbps, 160kbps or 256kbps.
    // NOTE: 128000 works fine for 44100, but 64000 is borked!
    formats.registerFormat(av::Format("MP3", "mp3", 
        av::AudioCodec("MP3", "libmp3lame", 2, 44100, 128000, "s16p")));

    formats.registerFormat(av::Format("FLV", "flv",
        av::VideoCodec("FLV", "flv", 320, 240)));

    formats.registerFormat(av::Format("FLV-Speex", "flv",
        av::VideoCodec("FLV", "flv", 320, 240),
        av::AudioCodec("Speex", "libspeex", 1, 16000)));

    formats.registerFormat(av::Format("Speex", "flv",
        av::AudioCodec("Speex", "libspeex", 1, 16000)));

    formats.registerFormat(av::Format("MJPEG", "mjpeg",
        av::VideoCodec("MJPEG", "mjpeg", 480, 320, 20)));

    // TODO: Add h264 and newer audio formats
}


MediaServer::~MediaServer()
{
    LDebug("Destroy")
}


void MediaServer::setupPacketStream(PacketStream& stream, const StreamingOptions& options,
                                    bool freeCaptures, bool attachPacketizers)
{
    LDebug("Setup Packet Stream")

    // Attach capture sources

    assert(options.oformat.video.enabled || options.oformat.audio.enabled);
    if (options.oformat.video.enabled) {
        assert(options.videoCapture);

        // assert(dynamic_cast<av::VideoCapture*>(options.videoCapture.get()));
        // assert(dynamic_cast<av::ICapture*>(options.videoCapture.get()));

        // auto source = dynamic_cast<PacketSource*>(options.videoCapture.get());
        // assert(source);
        // if (!source) throw std::runtime_error("Cannot attach incompatible packet source.");

        stream.attachSource<av::VideoCapture>(options.videoCapture, true); // freeCaptures,
    }
    if (options.oformat.audio.enabled) {
        assert(options.audioCapture);
        stream.attachSource<av::AudioCapture>(options.audioCapture, true); // freeCaptures,
    }

    // Attach an FPS limiter to the stream
    // stream.attach(new FPSLimiter(5), 1, true);

    // Attach an async queue so we don't choke
    // the video capture while encoding.
    // auto async = new AsyncPacketQueue<>(2048);
    // stream.attach(async, 3, true);

    // Attach the video encoder
    auto encoder = new av::MultiplexPacketEncoder(options);
    // encoder->init();
    stream.attach(encoder, 5, true);

    // Add format specific framings
    if (options.oformat.name == "MJPEG") {

        // Base64 encode the MJPEG stream for old browsers
        if (options.encoding.empty() || options.encoding == "none" || options.encoding == "None") {
            // no default encoding
        } else if (options.encoding == "Base64") {
            auto base64 = new Base64PacketEncoder();
            stream.attach(base64, 10, true);
        } else
            throw std::runtime_error("Unsupported encoding method: " + options.encoding);
    } 
    else if (options.oformat.name == "FLV") {

        // Allow mid-stream flash client connection
        // FIXME: Broken in latest flash
        // auto injector = new FLVMetadataInjector(options.oformat);
        // stream.attach(injector, 10);
    }

    // Attach the HTTP output framing
    IPacketizer* framing = nullptr;
    if (options.framing.empty() || options.framing == "none" || options.framing == "None")
        ;
        // framing = new http::StreamingAdapter("image/jpeg");

    else if (options.framing == "chunked")
        framing = new http::ChunkedAdapter("image/jpeg");

    else if (options.framing == "multipart")
        framing = new http::MultipartAdapter("image/jpeg", options.encoding == "Base64"); // false,

    else
        throw std::runtime_error("Unsupported framing method: " + options.framing);

    if (framing)
        stream.attach(framing, 15, true);

     // Attach a sync queue to synchronize output with the event loop
     auto sync = new SyncPacketQueue<>;
     stream.attach(sync, 20, true);
}


//
// HTTP Streaming Connection Factory
//


HTTPStreamingConnectionFactory::HTTPStreamingConnectionFactory(MediaServer* server)
    : _server(server)
{
}


HTTPStreamingConnectionFactory:: ~HTTPStreamingConnectionFactory() 
{
}


StreamingOptions HTTPStreamingConnectionFactory::createStreamingOptions(http::ServerConnection& conn)
{
    auto& request = conn.request();

    // Parse streaming options from query
    StreamingOptions options(_server);
    NVCollection params;
    request.getURIParameters(params);

    // An exception will be thrown if no format was provided,
    // or if the request format is not registered.
    options.oformat = _server->formats.get(params.get("format", "MJPEG"));
    if (params.has("width"))
        options.oformat.video.width = util::strtoi<uint32_t>(params.get("width"));
    if (params.has("height"))
        options.oformat.video.height = util::strtoi<uint32_t>(params.get("height"));
    if (params.has("fps"))
        options.oformat.video.fps = util::strtoi<uint32_t>(params.get("fps"));
    if (params.has("quality"))
        options.oformat.video.quality = util::strtoi<uint32_t>(params.get("quality"));

    // Response encoding and framing options
    options.encoding = params.get("encoding", "");
    options.framing = params.get("framing", "");

    // Open video and audio captures.
    av::Device dev;
    av::DeviceManager devman;
    if (options.oformat.video.enabled) {
        devman.getDefaultCamera(dev);
        LInfo("Default video capture ", dev.id)
        options.videoCapture = std::make_shared<av::VideoCapture>(dev.id, options.oformat.video);
        //options.videoCapture->openVideo(dev.id, options.oformat.video.width, 
        //                                   options.oformat.video.height, 
        //                                   options.oformat.video.fps);
        options.videoCapture->getEncoderFormat(options.iformat);
    }
    if (options.oformat.audio.enabled) {
        devman.getDefaultMicrophone(dev);
        LInfo("Default audio capture ", dev.id)
        options.audioCapture = std::make_shared<av::AudioCapture>(dev.id, options.oformat.audio);
        //options.audioCapture->open(dev.id, options.oformat.audio.channels, 
        //                                   options.oformat.audio.sampleRate);
        options.audioCapture->getEncoderFormat(options.iformat);
    }

    if (!options.videoCapture && !options.audioCapture) {
        throw std::runtime_error("No audio or video devices are available for capture");
    }

    return options;
}


http::ServerResponder* HTTPStreamingConnectionFactory::createResponder(http::ServerConnection& conn)
{
    try {
        auto& request = conn.request();

        // Log incoming requests
        SInfo << "Incoming connection from " << conn.socket()->peerAddress()
              << ": URI:\n" << request.getURI() 
              << ": Request:\n" << request << endl;

        // Handle websocket connections
        if (request.getURI().find("/websocket") == 0 || request.has("Sec-WebSocket-Key")) {
            return new WebSocketRequestHandler(conn, createStreamingOptions(conn));
        }

        // Handle HTTP streaming
        if (request.getURI().find("/streaming") == 0) {
            return new StreamingRequestHandler(conn, createStreamingOptions(conn));
        }

        // Handle relayed media requests
        if (request.getURI().find("/relay") == 0) {
            return new RelayedStreamingResponder(conn, createStreamingOptions(conn));
        }

#ifdef HAVE_OPENCV
        // Handle HTTP snapshot requests
        if (request.getURI().find("/snapshot") == 0) {
            return new SnapshotRequestHandler(conn, createStreamingOptions(conn));
        }
#endif
    } 
    catch (std::exception& exc) {
        LError("Request error: ", exc.what())
    }

    LWarn("Bad Request")
    conn.response().setStatus(http::StatusCode::BadRequest);
    conn.sendHeader();
    conn.close();
    return nullptr;
}


//
// HTTP Streaming Options
//


StreamingOptions::StreamingOptions(MediaServer* server,
                                   av::VideoCapture::Ptr videoCapture,
                                   av::AudioCapture::Ptr audioCapture)
    : server(server)
    , videoCapture(videoCapture)
    , audioCapture(audioCapture)
{
}
 

StreamingOptions::~StreamingOptions()
{
}


} // namespace scy