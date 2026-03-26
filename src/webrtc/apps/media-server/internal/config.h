#pragma once


#include <cstdint>
#include <string>
#include <string_view>


namespace icy {
namespace media_server {


struct Config
{
    std::string host = "0.0.0.0";
    uint16_t port = 4500;
    std::string webRoot = "./web/dist";

    enum class Mode { Stream, Record, Relay };
    Mode mode = Mode::Stream;
    std::string source;
    std::string recordDir = "./recordings";
    bool loop = true;

    std::string videoCodec = "libx264";
    int videoWidth = 1280;
    int videoHeight = 720;
    int videoFps = 30;
    int videoBitRate = 2000000;

    std::string audioCodec = "libopus";
    int audioChannels = 2;
    int audioSampleRate = 48000;
    int audioBitRate = 128000;

    bool enableTurn = true;
    uint16_t turnPort = 3478;
    std::string turnRealm = "0state.com";
    std::string turnExternalIP;

    static Mode parseMode(const std::string& s);
};


Config loadConfig(const std::string& path);
std::string makeRecordingPath(const std::string& recordDir, std::string_view peerId);


} // namespace media_server
} // namespace icy
