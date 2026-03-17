#include "scy/av/flvmetadatainjector.h"
#include "scy/av/mediafactory.h"
#include "scy/av/motiondetector.h"
#include "scy/av/multiplexencoder.h"
#include "scy/http/multipartpacketizer.h"
#include "scy/logger.h"
#include "scy/net/tcpservice.h"
#include "scy/packetstream.h"
#include "scy/runner.h"
#include "scy/turn/client/tcpclient.h"
#include "scy/util.h"

#include <conio.h>


using namespace std;
using namespace Scy;
using namespace Scy::Net;
using namespace Scy::Media;
using namespace Scy::TURN;
using namespace Scy::Util;




/*
// Detect Memory Leaks
#ifdef _DEBUG
#include "MemLeakDetect/MemLeakDetect.cpp"
#include "MemLeakDetect/MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif
*/


namespace Scy {
namespace TURN {


// ----------------------------------------------------------------------------
// Media Formats
//
Format MP344100 =
    Format("MP3", "mp3", VideoCodec(),
           AudioCodec("MP3", "libmp3lame", 2, 44100, 128000, "s16p"));
// Adobe Flash Player requires that audio files be 16bit and have a sample rate
// of 44.1khz.
// Flash Player can handle MP3 files encoded at 32kbps, 48kbps, 56kbps, 64kbps,
// 128kbps, 160kbps or 256kbps.
// NOTE: 128000 works fine for 44100, but 64000 is broken!

Format MP38000 = Format("MP3", "mp3", VideoCodec(),
                        AudioCodec("MP3", "libmp3lame", 1, 8000, 64000));

Format MP311000 = Format("MP3", "mp3", VideoCodec(),
                         AudioCodec("MP3", "libmp3lame", 1, 11000, 16000));

Format MP348000 =
    Format("MP3", "mp3", VideoCodec(), AudioCodec("MP3", "libmp3lame", 2, 48000, 128000, "s16p")); //, "s16p")

Format FLVNoAudio =
    Format("FLV", "flv", VideoCodec("FLV", "flv", 320, 240)); //, 6, 9000, 64000

Format FLVSpeex16000 = Format("FLV", "flv", VideoCodec("FLV", "flv", 320, 240),
                              AudioCodec("Speex", "libspeex", 1, 16000));

Format FLVSpeex16000NoVideo =
    Format("FLV", "flv", VideoCodec(),
           AudioCodec("Speex", "libspeex", 1, 16000)); //, 16800

Format MJPEG =
    Format("MJPEG", "mjpeg", VideoCodec("MJPEG", "mjpeg", 640, 480, 25));


// Global for now
class TURNMediaProvider;
Format ourMediaFormat = MJPEG;
VideoCapture* ourVideoCapture = NULL;
AudioCapture* ourAudioCapture = NULL;
TURNMediaProvider* ourMediaProvider = NULL;


// ----------------------------------------------------------------------------
// TCP TURN Initiator
//
class TURNMediaProvider : public Scy::TURN::TCPClientObserver
{
public:
    TURN::TCPClient client;
    Net::IP peerIP;
    Net::Address currentPeerAddr;
    PacketStream stream;

    Signal<void(TURN::Client&)> AllocationCreated;
    Signal<void(TURN::Client&, const Net::Address&)> ConnectionCreated;

    TURNMediaProvider(Net::Reactor& reactor, Runner& runner,
                      const TURN::Client::Options& options,
                      const Net::IP& peerIP)
        : client(*this, reactor, runner, options)
        , peerIP(peerIP)
    {
    }

    virtual ~TURNMediaProvider()
    {
        client.terminate();
        stopMedia();
    }

    void initiate()
    {
        LogDebug() << "[TURNMediaProvider: " << this << "] Initializing"
                   << endl;
        terminate();

        try {
            client.addPermission(peerIP);
            client.initiate();
        } catch (Poco::Exception& exc) {
            LogError() << "[TURNMediaProvider: " << this
                       << "] Error: " << exc.displayText() << std::endl;
        }
    }

    void terminate()
    {
        client.terminate();
        stopMedia();
    }

    void playMedia()
    {
        LogDebug() << "[TURNMediaProvider: " << this << "] Play Media" << endl;

        RecorderOptions options;
        options.oformat = ourMediaFormat;
        if (options.oformat.video.enabled && ourVideoCapture) {
            AllocateOpenCVInputFormat(ourVideoCapture, options.iformat);
            stream.attach(ourVideoCapture, false);
        }
        if (options.oformat.audio.enabled && ourAudioCapture) {
            AllocateRtAudioInputFormat(ourAudioCapture, options.iformat);
            stream.attach(ourAudioCapture, false);
        }

        MultiplexEncoder* encoder = new MultiplexEncoder(options);
        encoder->initialize();
        stream.attach(encoder, 5, true);

        if (options.oformat.label == "MJPEG") {
            HTTP::MultipartPacketizer* packetizer =
                new HTTP::MultipartPacketizer("image/jpeg", false);
            stream.attach(packetizer, 10, true);
        }

        stream += packetSlot(this, &TURNMediaProvider::onMediaEncoded);
        stream.start();
        LogDebug() << "[TURNMediaProvider: " << this << "] Play Media: OK"
                   << endl;
    }

    void stopMedia()
    {
        LogDebug() << "[TURNMediaProvider: " << this << "] Stop Media" << endl;
        stream -= packetSlot(this, &TURNMediaProvider::onMediaEncoded);
        stream.close();
        LogDebug() << "[TURNMediaProvider: " << this << "] Stop Media: OK"
                   << endl;
    }

protected:
    void onRelayStateChange(TURN::Client& client, TURN::ClientState& state,
                            const TURN::ClientState&)
    {
        LogDebug() << "[TURNMediaProvider: " << this
                   << "] Ltate Changed: "(state.toString(), )

                          switch (state.id())
        {
            case TURN::ClientState::Waiting:
                break;
            case TURN::ClientState::Allocating:
                break;
            case TURN::ClientState::Authorizing:
                break;
            case TURN::ClientState::Success:
                AllocationCreated.emit(this->client);
                break;
            case TURN::ClientState::Failed:
                assert(0 && "Allocation failed");
                break;
            case TURN::ClientState::Terminated:
                break;
        }
    }

    void onClientConnectionCreated(
        TURN::TCPClient& client, Net::IPacketSocket* sock,
        const Net::Address& peerAddr) // std::uint32_t connectionID,
    {
        LogDebug() << "[TURNMediaProvider: " << this
                   << "] Connection Created: " << peerAddr << endl;
        currentPeerAddr = peerAddr; // Current peer

        ConnectionCreated.emit(this->client, peerAddr);

        // Restart the media steram for each new peer
        stopMedia();
        playMedia();
        LogDebug() << "[TURNMediaProvider: " << this
                   << "] Connection Created: OK: " << peerAddr << endl;
    }

    void onClientConnectionState(TURN::TCPClient& client, Net::IPacketSocket*,
                                 Net::SocketState& state,
                                 const Net::SocketState& oldState)
    {
        LogDebug() << "[TURNMediaProvider: " << this
                   << "] Connection Ltate: "(state.toString(), )
    }

    void onRelayedData(TURN::Client& client, const char* data, int size,
                       const Net::Address& peerAddr)
    {
        LogDebug() << "[TURNMediaProvider: " << this
                   << "] Received Data: " << string(data, size) << ": "
                   << peerAddr << endl;
    }

    void onAllocationPermissionsCreated(TURN::Client& client,
                                        const TURN::PermissionList& permissions)
    {
        LogDebug() << "[TURNMediaProvider: " << this << "] Permissions Created"
                   << endl;
    }

    void onMediaEncoded(void* sender, DataPacket& packet)
    {
        Log("trace") << "[TURNMediaProvider: " << this
                     << "] Lending Packet: "(packet.size(), )

        try {
            // Send the media to our peer
            client.sendData((const char*)packet.data(), packet.size(),
                            currentPeerAddr);
        } catch (std::exception /*Exception*/& exc) {
            LogError() << "[TURNMediaProvider: " << this
                       << "] Lend Error: "(exc.displayText(), )
                              terminate();
        }
    }
};


// ----------------------------------------------------------------------------
// Media Connection
//
class AddressRequestHandler : public Poco::Net::TCPServerConnection
{
public:
    AddressRequestHandler(const Poco::Net::StreamSocket& sock)
        : Poco::Net::TCPServerConnection(sock)
    {
        Log("trace") << "[AddressRequestHandler] Creating" << endl;
    }

    ~AddressRequestHandler()
    {
        Log("trace") << "[AddressRequestHandler] Destroying" << endl;
    }

    void run()
    {
        Log("trace") << "[AddressRequestHandler] Running" << endl;

        // Reinitiate the provider session
        ourMediaProvider->initiate();
        ourMediaProvider->AllocationCreated +=
            sdelegate(this, &AddressRequestHandler::onAllocationCreated);

        wakeUp.wait();

        ourMediaProvider->AllocationCreated -=
            sdelegate(this, &AddressRequestHandler::onAllocationCreated);

        Log("trace") << "[AddressRequestHandler] Exiting" << endl;
    }

    void onAllocationCreated(void* sender, TURN::Client& client) //
    {
        LogDebug() << "[AddressRequestHandler] Allocation Created" << endl;

        // Write the relay address
        assert(ourMediaProvider);
        std::string data = ourMediaProvider->client.relayedAddress().toString();
        socket().sendBytes(data.data(), data.size());

        LogDebug() << "[AddressRequestHandler] Allocation Created 1" << endl;
        wakeUp.set();
        LogDebug() << "[AddressRequestHandler] Allocation Created 2" << endl;
    }

    Poco::Event wakeUp;
};


// ----------------------------------------------------------------------------
// HTTP Connection Factory
//
class AddressRequestHandlerFactory
    : public Poco::Net::TCPServerConnectionFactory
{
public:
    Poco::Net::TCPServerConnection*
    createConnection(const Poco::Net::StreamSocket& socket)
    {
        Log("trace") << "[AddressRequestHandlerFactory] Creating Connection"
                     << endl;

        Poco::Net::StreamSocket sock(socket);

        try {
            // Wait until we have some data in the sock read buffer
            // then parse and redirect accordingly.
            char buffer[4096];
            sock.setReceiveTimeout(Poco::Timespan(2, 0));
            int size = sock.receiveBytes(buffer, sizeof(buffer));
            std::string request(buffer, size);

            Log("trace") << "HTTP Connection:\n"
                         << "\n\tClient Address: "
                         << sock.peerAddress().toString()
                         << "\n\tRequest: " << request << endl;

            // Flash policy-file-request
            if ((request.find("policy-file-request") != string::npos) ||
                (request.find("crossdomain") != string::npos)) {
                Log("trace") << "[AddressRequestHandlerFactory] Sending Flash "
                                "Crossdomain Policy"
                             << endl;
                return new FlashPolicyRequestHandler(sock);
            }

            return new AddressRequestHandler(sock);
        } catch (std::exception /*Exception*/& exc) {
            LogError() << exc.displayText() << endl;
        }

        return new BadRequestHandler(sock);
    }
};


// ----------------------------------------------------------------------------
// HTTP Relay Address Service
//
class RelayAddressService : public TCPService
{
public:
    RelayAddressService(unsigned short port)
        : TCPService(new AddressRequestHandlerFactory(), port)
    {
    }

    ~RelayAddressService() {}
};


} // namespace TURN
} // namespace Scy


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", STraceevel));

    // MediaFactory::initialize();
    // MediaFactory::instance()->loadVideo();
    // MediaFactory::instance()->loadAudio();

    // Init input devices
    ourVideoCapture = new VideoCapture(0);
    // ourAudioCapture = MediaFactory::instance()->audio.getCapture(0,
    //    ourMediaFormat.audio.channels,
    //    ourMediaFormat.audio.sampleRate);

    // Create client
    Runner runner;
    Net::Reactor reactor;
    Net::IP peerIP("127.0.0.1");
    TURN::Client::Options co;
    co.serverAddr = Net::Address("127.0.0.1", 3478); // "173.230.150.125"
    co.lifetime = 120 * 1000;                        // 2 minute
    co.timeout = 10 * 1000;
    co.timerInterval = 3 * 1000;
    co.username = Anionu_API_USERNAME;
    co.password = Anionu_API_KEY;
    ourMediaProvider = new TURNMediaProvider(reactor, runner, co, peerIP);

    // Create address service
    RelayAddressService service(800);
    service.start();

    char o = 0;
    while (o != 'Q') {
        cout << "COMMANDS:\n\n"
                "  A    Create the Allocation\n"
                "  Z    Print the Allocation Address\n"
                "  Q    Quit\n\n";

        o = toupper(getch());

        // Create the Allocation
        if (o == 'A') {
            ourMediaProvider->terminate();
            ourMediaProvider->initiate();
        }

        // Print the Allocation Address
        else if (o == 'Z') {
            cout << "########################################"
                 << "\n\n"
                 << "  Allocation Address: "
                 << (ourMediaProvider
                         ? ourMediaProvider->client.relayedAddress().toString()
                         : "None")
                 << "\n\n"
                 << "########################################\n\n";
        }
    }

    // Wait for user input...
    Util::pause();

    if (ourMediaProvider)
        delete ourMediaProvider;
    MediaFactory::uninitialize();

    return 0;
}