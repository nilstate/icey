#include "scy/av/videoanalyzer.h"
#include "scy/logger.h"

#include <conio.h>


using namespace std;
using namespace Sourcey;
using namespace Sourcey::Media;


/*
// Detect Memory Leaks
#ifdef _DEBUG
#include "MemLeakDetect/MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif<DataPacket>
*/


int main(int argc, char** argv)
{
    try {
        Logger::instance().add(std::make_unique<ConsoleChannel>("debug", STraceevel));

        VideoAnalyzer analyzer;

        std::string ifile = "d:/Harold Kumar 3 Christmas tablet.mp4";
        std::string ofile = "output.csv";

        char o = 0;
        while (o != 'Q') {
            cout << "COMMANDS:\n\n"
                    "  I    Set Input File.\n"
                    "  O    Set Output File.\n"
                    "  A    Start Analyzing Video.\n"
                    "  S    Stop Analyzing Video.\n"
                    "  Q    Quit.\n\n";

            o = toupper(getch());

            // Set Input File
            if (o == 'I') {
                cout << "Enter input file path: " << endl;
                getline(cin, ifile);
            }

            // Set Output File
            if (o == 'O') {
                cout << "Enter output file path: " << endl;
                getline(cin, ofile);
            }

            // Start Analyzing Video
            if (o == 'A') {
                analyzer.start(ifile, ofile);
            }

            // Stop Analyzing Video
            if (o == 'S') {
                analyzer.stop();
            }
        }
    } catch (Poco::Exception& exc) {
        Log("error") << "[WaveformOutput] Error: " << exc.displayText() << endl;
    }

    system("pause");

    return 0;
}


// using namespace Poco::Net;


/*
#include "AVFileReader.h"
#include "FFT.h"
#include "MatrixPacketizer.h"
#include "scy/packetstream.h"
#include "scy/runner.h"
#include "scy/util.h"

#include <assert.h>
#include <fstream>
#include <string>
#include <vector>


using namespace Poco;
using namespace Sourcey;
using namespace Sourcey::Util;
const int FFT_POINTS        = 1024; //2048; //256;
const int SAMPLES_PER_SEC    = 11025;
const int BITS_PER_SAMPLE    = 16;        // 8 / 16
const int NUM_CHANNELS        = 2;        // 1 = mono, 2 = sterio

int fftPoints = FFT_POINTS * 4;
int samplesPerSecond = 11025; // WAVE

int bitsPerSample = 16;
int numChannels = 2;
struct WaveformInfo
{
    std::string channel;
    double time;
    double min;
    double max;
    double avg;
    double sum;

    WaveformInfo() :
        time(0.0),
        min(99999.9),
        max(-99999.9),
        avg(0.0),
        sum(0.0)
    {
    }
};


class WaveformOutput
{
public:
    WaveformOutput() :
        _fft(FFT_POINTS),
        _fftData(FFT_POINTS)
    {
    }

    ~WaveformOutput()
    {
        stop();
    }

    void start(const std::string& input, const std::string& output)
    {
        // Open the input file decoder
        _reader.open(input);

        // Open the output file
        _file.open(output.data(), ios::out | ios::binary);

        stream.attach(&_reader, false);

        // Attach the matrix packetizer to convert decoded
        // video packets to OpenCV matrix images for sending
        // to the motion detector.
        stream.attach(&_packetizer, 1, false);

        // Attach the motion detector to the packet stream.
        stream.attach(&_detector, 2, false);

        // Receive MHI output images directly from the motion
        // detector. The motion level will represent the
        // output waveform level.
        _detector.attach(packetSlot(this, &WaveformOutput::onVideo));
        //stream.attach(packetSlot(this, &WaveformOutput::onVideo));

        // Receive decoded audio from the file decoder and
        // run it through the FFT algorithm to gather
        // visualisation data.
        _reader.attach(packetSlot(this, &WaveformOutput::onAudio));
        //stream.attach(packetSlot(this, &WaveformOutput::onAudio));

        // Kick off the packet stream...
        stream.start();
    }

    void stop()
    {
        stream.close();
        _file.close();
    }

    void writeLine(const std::string& channel, double timestamp, double value,
double min = 0, double max = 0, double avg = 0)
    {
        Log("trace") << "[WaveformOutput:" << this << "] Writing:"
            << channel << "," << timestamp << "," << value << "," << min << ","
<< max << "," << avg << endl;

        _file << channel << "," << timestamp << "," << value << "," << min <<
"," << max << "," << avg << "\r\n";
    }

    void onVideo(void* sender, VideoPacket& packet)
    {
        writeLine("video", packet.time, _detector.motionLevel());

        //cv::imshow("hi", *packet.mat);
        //cv::waitKey(20);
    }

    void onAudio(void* sender, AudioPacket& packet)
    {
        cout << "onAudio: " << packet.size() << ": " << packet.time << endl;
        short const* data16 = reinterpret_cast<short*>(packet.data());

        // Fill the FFT buffer
        for (int i = 0; i < FFT_POINTS; ++i) {
            //int audioLeft = (int)data16[i * 2];
            //int audioRight = (int)data16[i * 2 + 1];
            //(audioLeft + audioRight) * (1.0f / 65536.0f); //packet.data[i];
//buf[i];
            _fftData[i] = packet.data()[i];
        }

        // Run Fourier Transform
        vector<FFT::Complex> frequencies = _fft.transform(_fftData);

        // Compute frequency magnitude
        double level = 0.0;
        double sum = 0.0;
        double min = 99999.9;
        double max = -99999.9;
        double avg = 0.0;
        for (int i = 0; i < FFT_POINTS / 2; ++i) {
            level = _fft.getIntensity(frequencies[i]);
//getDecibels(frequencies[i]);
            if (level > max)
                max = level;
            if (level < min)
                min = level;
            sum += level;
        }

        // TODO: Better way of gathering avg?
        // Not all FFT values are useful.
        avg = sum / frequencies.size();

        writeLine("audio", packet.time, sum, min, max, avg);
    }
*/


/*

    //AudioDecoder* audio = reinterpret_cast<AudioDecoder*>(packet.opaque);
    //int samples_per_second = audio->sampleRate;
    //int N = 1024; //1024; //64; //1024; //packet.size / 2;
    //vector<FFT::Complex> fftData(FFT_POINTS);
    //audioResults.push_back(WaveformInfo());
    //WaveformInfo& info = audioResults.back();
    //channel = "audio";
    //time = packet.time;
    //FFT dft(N);
void extractAudio(AudioContext* audio, AudioPacket& packet, std::vector<int>&
outBuf)
{
    outBuf.resize(packet.size);
    if (audio->bitsPerSample == 8) {
        unsigned char const * data8 = reinterpret_cast<unsigned char
*>(packet.data); //GetData8 ();
        if (audio->channels == 1) { // mono
            for (int i = 0; i != packet.size; ++i)
                outBuf[i] = (data8 [i] - 128) * 64;
        }
        else {
/// stereo
            for (int i = 0; i != packet.size; ++i)
                outBuf[i] = (data8[2*i] + data8[2*i+1] - 2 * 128) * 128;
        }
    }
    else if (audio->bitsPerSample == 16) {
        short const * data16 = reinterpret_cast<short *>(packet.data);
//GetData16 ();
        if (audio->channels == 1)
        {
            for (unsigned i = 0; i != packet.size; ++i)
                outBuf [i] = data16[i];
        }
        else // stereo
        {
            for (unsigned i = 0; i != packet.size; ++i)
                outBuf [i] = (data16[2*i] +  data16[2*i+1]) / 2;
        }
    }
    else
        throw Poco::Exception("Unsupported format");
}

PacketStream stream;
AVFileReader _reader;
MatrixPacketizer _packetizer;
MotionDetector    _detector;
std::ofstream _file;
FFT _fft;
vector<FFT::Complex> _fftData;

// Testing
//vector<WaveformInfo> audioResults;
};
*/


/*

// A standard 16-bit stereo WAVE file, but with a long header
// 22050 Hz, 16-bit, 1.32 s
int samples_per_second = 22050;
FILE* f = fopen("d:/test.wav", "rb");
fseek(f, 0, SEEK_END);
int n = ftell(f) / 2;
rewind(f);
short* buf = new short[n];
fread(buf, n, 2, f);
fclose(f);

n = 256;

vector<FFT::Complex> fftData(n);
for (int i = 0; i < n; ++i)
    fftData[i] = buf[i];
delete[] buf;

Log("debug") << "[AVFileReader] Audio Intensity:"
    << "\n\tn: " << n
    << "\n\tValue: " << fftData.size()
    << endl;

FFT dft(n);
vector<FFT::Complex> frequencies = dft.transform(fftData);

for (int k = 0; k < (n >> 1); ++k)
    //if (dft.getIntensity(frequencies[k]) > 100)
        cout << (k * samples_per_second / n) << " => "
                << dft.getIntensity(frequencies[k]) << endl;

    system("pause");

*/


/*
WaveformOutput app;
//string input = "d:/test.wav"; //("GetAccount");test.mp4
std::string input = "d:/Harold Kumar 3 Christmas tablet.mp4";
//("GetAccount");test.mp4
//string input = "C:/movies/Fear And Loathing In Las Vegas 1998 720p x264 BRRip
GokU61[Z Warriors Release]/Fear And Loathing In Las Vegas 1998 720p x264 BRRip
GokU61[Z Warriors Release].mp4"; //("GetAccount");test.mp4

app.start(input, "data.csv");
//app.start(input, "data.wav");
system("pause");

for (int i = 0; i < app.audioResults.size(); ++i)
    Log("debug") << "[AVFileReader] Audio Info:"
        //<< "\n\tChannel: " << app.audioResults[i].channel
        << "\n\tTotal: " << app.audioResults[i].sum
        << "\n\tMin: " << app.audioResults[i].min
        << "\n\tMax: " << app.audioResults[i].max
        << "\n\tTime: " << app.audioResults[i].time
        << "\n\tAverage: " << app.audioResults[i].avg
        << endl;

*/
