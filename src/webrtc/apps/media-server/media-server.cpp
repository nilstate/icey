///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Media Server
//
// Single binary: WebRTC media streaming + Symple signalling + TURN relay
// + web UI. No Node.js, no third-party services. One binary, two ports.
//
// The media server registers as a virtual peer in the Symple network.
// Browsers discover it via presence, click Call, and receive video + audio.
//
// Pipeline (stream mode):
//   MediaCapture -> VideoPacketEncoder -> WebRtcTrackSender -> [browser]
//                -> AudioPacketEncoder -> WebRtcTrackSender -> [browser]
//
// Pipeline (record mode):
//   WebRtcTrackReceiver -> VideoDecoder -> MultiplexPacketEncoder -> MP4
//
// Pipeline (relay mode):
//   [browser source] -> WebRtcTrackReceiver -> relay controller -> WebRtcTrackSender -> [browser viewers]
//
// Usage:
//   media-server --source video.mp4 --web-root web/dist
//
/// @addtogroup webrtc
/// @{


#include "icy/application.h"
#include "icy/logger.h"
#include "internal/app.h"
#include "internal/config.h"

#include <memory>
#include <string>


using namespace icy;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    std::string configPath = "./config.json";

    for (int i = 1; i + 1 < argc; i += 2) {
        if (std::string(argv[i]) == "-c" || std::string(argv[i]) == "--config")
            configPath = argv[i + 1];
    }

    media_server::Config config = media_server::loadConfig(configPath);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::string val = (i + 1 < argc) ? argv[i + 1] : "";

        if ((arg == "-c" || arg == "--config") && !val.empty()) {
            ++i;
        }
        else if (arg == "--port" && !val.empty()) {
            config.port = static_cast<uint16_t>(std::stoi(val));
            ++i;
        }
        else if (arg == "--turn-port" && !val.empty()) {
            config.turnPort = static_cast<uint16_t>(std::stoi(val));
            ++i;
        }
        else if (arg == "--mode" && !val.empty()) {
            config.mode = media_server::Config::parseMode(val);
            ++i;
        }
        else if (arg == "--source" && !val.empty()) {
            config.source = val;
            ++i;
        }
        else if (arg == "--record-dir" && !val.empty()) {
            config.recordDir = val;
            ++i;
        }
        else if (arg == "--web-root" && !val.empty()) {
            config.webRoot = val;
            ++i;
        }
        else if (arg == "--no-turn") {
            config.enableTurn = false;
        }
    }

    media_server::MediaServerApp app(config);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<media_server::MediaServerApp*>(opaque)->shutdown();
    }, &app);

    Logger::destroy();
    return 0;
}

/// @}
