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
#include "icy/signal.h"
#include "icy/speech/event.h"
#include "icy/speech/speech.h"

#include <cstdint>
#include <string>


namespace icy {
namespace speech {


struct Speech_API VoiceActivityDetectorConfig
{
    std::string source;
    std::string detectorName = "energy_vad";
    int sampleRateHint = 0;
    float startThreshold = 0.045f;
    float stopThreshold = 0.020f;
    int64_t minSilenceUsec = 250000;
    int64_t updateIntervalUsec = 250000;
};


struct Speech_API VoiceActivityDetectorStats
{
    uint64_t seen = 0;
    uint64_t emitted = 0;
    bool active = false;
    float lastLevel = 0.0f;
    float lastPeak = 0.0f;
};


class Speech_API VoiceActivityDetector
{
public:
    struct Levels
    {
        float level = 0.0f;
        float peak = 0.0f;
        bool valid = false;
    };

    Signal<void(const SpeechEvent&)> Event;

    explicit VoiceActivityDetector(VoiceActivityDetectorConfig config = {});

    void setConfig(VoiceActivityDetectorConfig config);
    [[nodiscard]] VoiceActivityDetectorConfig config() const;
    [[nodiscard]] VoiceActivityDetectorStats stats() const;
    void reset();

    void process(const av::PlanarAudioPacket& packet);

private:
    static VoiceActivityDetectorConfig sanitize(VoiceActivityDetectorConfig config);
    [[nodiscard]] Levels measure(const av::PlanarAudioPacket& packet) const;
    void emitEvent(const char* type,
                   const av::PlanarAudioPacket& packet,
                   const Levels& levels,
                   bool active);

    VoiceActivityDetectorConfig _config;
    uint64_t _sequence = 0;
    uint64_t _seen = 0;
    uint64_t _emitted = 0;
    int64_t _lastUpdateUsec = 0;
    int64_t _lastBelowThresholdUsec = 0;
    bool _active = false;
    float _lastLevel = 0.0f;
    float _lastPeak = 0.0f;
};


} // namespace speech
} // namespace icy


/// @}
