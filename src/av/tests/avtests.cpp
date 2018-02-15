#include "avtests.h"


using namespace std;
using namespace scy;
using namespace scy::av;
using namespace scy::test;


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Debug)); // Level::Trace, Level::Debug
    // Logger::instance().setWriter(new AsyncLogWriter);
    test::init();

    // Define class based tests
#ifdef HAVE_FFMPEG
    describe("audio encoder", new AudioEncoderTest);
    describe("audio resampler", new AudioResamplerTest);
    describe("audio fifo buffer", new AudioBufferTest);
    describe("h264 video file transcoder", new VideoFileTranscoderTest);
    describe("h264 multiplex capture encoder", new MultiplexCaptureEncoderTest);
    // describe("realtime encoder media queue", new RealtimeMediaQueueEncoderTest);
    // describe("audio capture", new AudioCaptureTest);
    // describe("audio capture encoder", new AudioCaptureEncoderTest);
    // describe("audio capture resampler", new AudioCaptureResamplerTest);
    // describe("device capture multiplex encoder", new DeviceCaptureMultiplexEncoderTest);
#endif

    describe("realtime media queue", new RealtimeMediaQueueTest);

    test::runAll();

    return test::finalize();
}


// //
// /// Device Manager Tests
// //
//
// describe("device manager", []() {
//     LDebug("Starting")
//     auto& deviceManager = av::MediaFactory::instance().devices();
//
//     av::Device device;
//     if (deviceManager.getDefaultCamera(device)) {
//         LDebug("Default video device: ", device.id, ": ", device.name)
//     }
//     if (deviceManager.getDefaultMicrophone(device)) {
//         LDebug("Default audio device: ", device.id, ": ", device.name)
//     }
//
//     std::vector<av::Device> devices;
//     if (deviceManager.getCameras(devices)) {
//         LDebug("Num video devices: ", devices.size())
//         for (auto& device : devices) {
//             SDebug << "Printing video device: " << device.id << ": " <<
//             device.name << endl;
//         }
//     }
//     else {
//         LDebug("No video devices detected!")
//     }
//     if (deviceManager.getMicrophones(devices)) {
//         LDebug("Num audio devices: ", devices.size())
//         for (auto& device : devices) {
//             SDebug << "Printing audio device: " << device.id << ": " <<
//             device.name << endl;
//         }
//     }
//     else {
//         LDebug("No video devices detected!")
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
//     LDebug("Starting")
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
//     LDebug("Complete")
// }
//
// describe("video capture stream", []() {
//     LDebug("Starting")
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
//     LDebug("Complete")
// });
