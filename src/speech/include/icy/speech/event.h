///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup speech
/// @{


#pragma once


#include "icy/av/packet.h"
#include "icy/json/json.h"
#include "icy/speech/speech.h"

#include <cstddef>
#include <cstdint>
#include <string>


namespace icy {
namespace speech {


struct Speech_API AudioRef
{
    uint64_t sequence = 0;
    int64_t timeUsec = 0;
    int64_t durationUsec = 0;
    int sampleRate = 0;
    int channels = 0;
    size_t numSamples = 0;
    std::string sampleFmt;
};


struct Speech_API SpeechEvent
{
    std::string type = "speech:update";
    std::string source;
    std::string detector;
    int64_t emittedAtUsec = 0;
    AudioRef audio;
    bool active = false;
    float level = 0.0f;
    float peak = 0.0f;
    json::Value data = json::Value::object();
};


[[nodiscard]] Speech_API AudioRef makeAudioRef(const av::PlanarAudioPacket& packet,
                                               uint64_t sequence = 0,
                                               int sampleRateHint = 0);
[[nodiscard]] Speech_API json::Value toJson(const AudioRef& audio);
[[nodiscard]] Speech_API json::Value toJson(const SpeechEvent& event);


} // namespace speech
} // namespace icy


/// @}
