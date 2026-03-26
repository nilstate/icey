#include "config.h"

#include "icy/filesystem.h"
#include "icy/json/json.h"

#include <cctype>
#include <chrono>
#include <fstream>


namespace icy {
namespace media_server {


Config::Mode Config::parseMode(const std::string& s)
{
    if (s == "record")
        return Mode::Record;
    if (s == "relay")
        return Mode::Relay;
    return Mode::Stream;
}


Config loadConfig(const std::string& path)
{
    Config c;
    std::ifstream file(path);
    if (!file.is_open())
        return c;

    json::Value j;
    file >> j;

    if (j.contains("http")) {
        auto& h = j["http"];
        c.host = h.value("host", c.host);
        c.port = h.value("port", c.port);
    }
    if (j.contains("media")) {
        auto& m = j["media"];
        c.mode = Config::parseMode(m.value("mode", "stream"));
        c.source = m.value("source", c.source);
        c.recordDir = m.value("recordDir", c.recordDir);
        c.loop = m.value("loop", c.loop);
        if (m.contains("video")) {
            auto& v = m["video"];
            c.videoCodec = v.value("codec", c.videoCodec);
            c.videoWidth = v.value("width", c.videoWidth);
            c.videoHeight = v.value("height", c.videoHeight);
            c.videoFps = v.value("fps", c.videoFps);
            c.videoBitRate = v.value("bitrate", c.videoBitRate);
        }
        if (m.contains("audio")) {
            auto& a = m["audio"];
            c.audioCodec = a.value("codec", c.audioCodec);
            c.audioChannels = a.value("channels", c.audioChannels);
            c.audioSampleRate = a.value("sampleRate", c.audioSampleRate);
            c.audioBitRate = a.value("bitrate", c.audioBitRate);
        }
    }
    if (j.contains("turn")) {
        auto& t = j["turn"];
        c.enableTurn = t.value("enabled", c.enableTurn);
        c.turnPort = t.value("port", c.turnPort);
        c.turnRealm = t.value("realm", c.turnRealm);
        c.turnExternalIP = t.value("externalIp", c.turnExternalIP);
    }
    if (j.contains("webRoot"))
        c.webRoot = j["webRoot"].get<std::string>();

    return c;
}


static std::string sanitizePathComponent(std::string_view input)
{
    std::string result;
    result.reserve(input.size());
    for (unsigned char ch : input) {
        if (std::isalnum(ch) || ch == '-' || ch == '_')
            result.push_back(static_cast<char>(ch));
        else
            result.push_back('_');
    }
    if (result.empty())
        result = "peer";
    return result;
}


std::string makeRecordingPath(const std::string& recordDir,
                              std::string_view peerId)
{
    const auto stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return fs::makePath(
        recordDir,
        sanitizePathComponent(peerId) + "-" + std::to_string(stamp) + ".mp4");
}


} // namespace media_server
} // namespace icy
