#include "avtests.h"


using namespace std;
using namespace icy;
using namespace icy::av;
using namespace icy::test;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug)); // Level::Trace, Level::Debug
    // Logger::instance().setWriter(std::make_unique<AsyncLogWriter>());
    test::init();

    // Define class based tests
#ifdef HAVE_FFMPEG
    describe("audio encoder", new AudioEncoderTest);
    describe("audio resampler", new AudioResamplerTest);
    describe("audio fifo buffer", new AudioBufferTest);
    describe("video encoder", new VideoEncoderTest);
    describe("video packet encoder timestamps", new VideoPacketEncoderTimestampTest);
    describe("video decoder", new VideoDecoderTest);
    describe("video decoder negotiated stream", new VideoDecoderNegotiatedStreamTest);
    describe("audio decoder", new AudioDecoderTest);
    describe("h264 video file transcoder", new VideoFileTranscoderTest);
    describe("h264 multiplex capture encoder", new MultiplexCaptureEncoderTest);
    // describe("realtime encoder media queue", new RealtimeMediaQueueEncoderTest);
    // describe("audio capture", new AudioCaptureTest);
    // describe("audio capture encoder", new AudioCaptureEncoderTest);
    // describe("audio capture resampler", new AudioCaptureResamplerTest);
    // describe("device capture multiplex encoder", new DeviceCaptureMultiplexEncoderTest);
#endif

    describe("realtime media queue", new RealtimeMediaQueueTest);


    // =========================================================================
    // Format Construction
    //
    describe("format construction", []() {
        av::Format fmt;
        expect(fmt.name.empty());
        expect(fmt.id.empty());
        expect(fmt.type() == av::Format::None);
    });

    describe("format video only", []() {
        av::Format fmt("MJPEG", "mjpeg",
                       av::VideoCodec("MJPEG", "mjpeg", 640, 480, 25));
        expect(fmt.name == "MJPEG");
        expect(fmt.id == "mjpeg");
        expect(fmt.video.enabled);
        expect(!fmt.audio.enabled);
        expect(fmt.video.width == 640);
        expect(fmt.video.height == 480);
        expect(fmt.type() == av::Format::Video);
    });

    describe("format audio only", []() {
        av::Format fmt("MP3", "mp3",
                       av::AudioCodec("MP3", "libmp3lame", 2, 44100, 128000));
        expect(fmt.name == "MP3");
        expect(!fmt.video.enabled);
        expect(fmt.audio.enabled);
        expect(fmt.audio.channels == 2);
        expect(fmt.audio.sampleRate == 44100);
        expect(fmt.type() == av::Format::Audio);
    });

    describe("format multiplex", []() {
        av::Format fmt("FLV", "flv",
                       av::VideoCodec("FLV", "flv", 320, 240),
                       av::AudioCodec("Speex", "libspeex", 1, 16000));
        expect(fmt.video.enabled);
        expect(fmt.audio.enabled);
        expect(fmt.type() == av::Format::Multiplex);
    });

    describe("format priority", []() {
        av::Format f1("A", "a", av::VideoCodec("V", "v", 100, 100));
        av::Format f2("B", "b", av::VideoCodec("V", "v", 100, 100));
        f1.priority = 1;
        f2.priority = 2;
        expect(f1.priority < f2.priority);
    });


    // =========================================================================
    // FormatRegistry
    //
    describe("format registry register and get", []() {
        av::FormatRegistry registry;
        av::Format fmt("TestFmt", "testfmt",
                       av::VideoCodec("V", "v", 100, 100));
        registry.registerFormat(fmt);

        expect(registry.exists("TestFmt"));
        auto got = registry.get("TestFmt");
        expect(got.name == "TestFmt");
        expect(got.id == "testfmt");
    });

    describe("format registry getByID", []() {
        av::FormatRegistry registry;
        registry.registerFormat(av::Format("Foo", "foo_id",
                                           av::VideoCodec("V", "v", 100, 100)));

        auto got = registry.getByID("foo_id");
        expect(got.name == "Foo");
    });

    describe("format registry default", []() {
        av::FormatRegistry registry;
        av::Format f1("First", "first", av::VideoCodec("V", "v", 100, 100));
        av::Format f2("Second", "second", av::VideoCodec("V", "v", 100, 100));
        registry.registerFormat(f1);
        registry.registerFormat(f2);

        registry.setDefault("Second");
        auto def = registry.getDefault();
        expect(def.name == "Second");
    });

    describe("format registry unregister", []() {
        av::FormatRegistry registry;
        registry.registerFormat(av::Format("Del", "del",
                                           av::VideoCodec("V", "v", 100, 100)));
        expect(registry.exists("Del"));

        registry.unregisterFormat("Del");
        expect(!registry.exists("Del"));
    });

    describe("format registry clear", []() {
        av::FormatRegistry registry;
        registry.registerFormat(av::Format("A", "a", av::VideoCodec("V", "v", 100, 100)));
        registry.registerFormat(av::Format("B", "b", av::VideoCodec("V", "v", 100, 100)));
        registry.clear();
        expect(!registry.exists("A"));
        expect(!registry.exists("B"));
    });


    // =========================================================================
    // FPSCounter
    //
    describe("fps counter", []() {
        av::FPSCounter counter;
        expect(counter.fps == 0.0);
        expect(counter.frames == 0);

        // Tick a few times
        for (int i = 0; i < 10; i++) {
            counter.tick();
        }
        expect(counter.frames == 10);

        counter.reset();
        expect(counter.frames == 0);
    });


    test::runAll();

    return test::finalize();
}


// //
// /// Device Manager Tests
// //
//
// describe("device manager", []() {
//     LDebug("Starting");
//     auto& deviceManager = av::MediaFactory::instance().devices();
//
//     av::Device device;
//     if (deviceManager.getDefaultCamera(device)) {
//         LDebug("Default video device: ", device.id, ": ", device.name);
//     }
//     if (deviceManager.getDefaultMicrophone(device)) {
//         LDebug("Default audio device: ", device.id, ": ", device.name);
//     }
//
//     std::vector<av::Device> devices;
//     if (deviceManager.getCameras(devices)) {
//         LDebug("Num video devices: ", devices.size());
//         for (auto& device : devices) {
//             SDebug << "Printing video device: " << device.id << ": " <<
//             device.name << endl;
//         }
//     }
//     else {
//         LDebug("No video devices detected!");
//     }
//     if (deviceManager.getMicrophones(devices)) {
//         LDebug("Num audio devices: ", devices.size());
//         for (auto& device : devices) {
//             SDebug << "Printing audio device: " << device.id << ": " <<
//             device.name << endl;
//         }
//     }
//     else {
//         LDebug("No video devices detected!");
//     }
//
//     // TODO: verify data integrity?
// });
//
//
// //
// /// Video Capture Tests
// //
//
// describe("video capture", []() {
//     LDebug("Starting");
//
//     av::VideoCapture::Ptr capture =
//     MediaFactory::instance().createVideoCapture(0);
//     capture->emitter += packetSlot(&context,
//     &CallbackContext::onVideoCaptureFrame);
//
//     // std::puts("Press any key to continue...");
//     // std::getchar();
//
//     // FIXME: Run loop until x number of frames received
//
//     capture->emitter -= packetSlot(this,
//     &CallbackContext::onVideoCaptureFrame);
//
//     LDebug("Complete");
// }
//
// describe("video capture stream", []() {
//     LDebug("Starting");
//
//     av::VideoCapture::Ptr capture =
//     MediaFactory::instance().createVideoCapture(0);
//     {
//         PacketStream stream;
//         stream.emitter += packetSlot(&context,
//         &CallbackContext::onVideoCaptureStreamFrame);
//         stream.attachSource<av::VideoCapture>(capture, true);
//         stream.start();
//
//         // std::puts("Press any key to continue...");
//         // std::getchar();
//     }
//
//     assert(capture->emitter.nslots() == 0);
//
//     LDebug("Complete");
// });
