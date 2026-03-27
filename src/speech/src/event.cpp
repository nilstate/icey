///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup speech
/// @{


#include "icy/speech/event.h"


namespace icy {
namespace speech {


AudioRef makeAudioRef(const av::PlanarAudioPacket& packet,
                      uint64_t sequence,
                      int sampleRateHint)
{
    const int sampleRate = sampleRateHint > 0 ? sampleRateHint : 0;
    int64_t durationUsec = 0;
    if (sampleRate > 0 && packet.numSamples > 0) {
        durationUsec = static_cast<int64_t>(
            (static_cast<long double>(packet.numSamples) * 1000000.0L) /
            static_cast<long double>(sampleRate));
    }

    return {
        .sequence = sequence,
        .timeUsec = packet.time,
        .durationUsec = durationUsec,
        .sampleRate = sampleRate,
        .channels = packet.channels,
        .numSamples = packet.numSamples,
        .sampleFmt = packet.sampleFmt,
    };
}


json::Value toJson(const AudioRef& audio)
{
    json::Value value;
    value["sequence"] = audio.sequence;
    value["time"] = audio.timeUsec;
    value["duration"] = audio.durationUsec;
    value["sampleRate"] = audio.sampleRate;
    value["channels"] = audio.channels;
    value["numSamples"] = audio.numSamples;
    value["sampleFmt"] = audio.sampleFmt;
    return value;
}


json::Value toJson(const SpeechEvent& event)
{
    json::Value value;
    value["type"] = event.type;
    value["source"] = event.source;
    value["detector"] = event.detector;
    value["time"] = event.emittedAtUsec;
    value["audio"] = toJson(event.audio);
    value["active"] = event.active;
    value["level"] = event.level;
    value["peak"] = event.peak;
    value["data"] = event.data;
    return value;
}


} // namespace speech
} // namespace icy


/// @}
