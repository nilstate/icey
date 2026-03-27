///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup speech
/// @{


#include "icy/speech/voiceactivitydetector.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string_view>
#include <utility>


namespace icy {
namespace speech {
namespace {


bool isPlanar(std::string_view sampleFmt)
{
    return !sampleFmt.empty() && sampleFmt.back() == 'p';
}


template <typename T, typename Decode>
VoiceActivityDetector::Levels measureSamples(const av::PlanarAudioPacket& packet,
                                             Decode&& decode)
{
    VoiceActivityDetector::Levels levels;
    if (packet.numSamples == 0)
        return levels;

    const int channels = std::max(packet.channels, 1);
    const bool planar = isPlanar(packet.sampleFmt);
    long double sumAbs = 0.0L;
    long double peak = 0.0L;
    uint64_t count = 0;

    if (planar) {
        const int planeCount = std::min(channels, 4);
        for (int channel = 0; channel < planeCount; ++channel) {
            auto* plane = reinterpret_cast<const T*>(packet.buffer[channel]);
            if (!plane)
                continue;
            for (size_t index = 0; index < packet.numSamples; ++index) {
                const long double sample = std::abs(decode(plane[index]));
                sumAbs += sample;
                peak = std::max(peak, sample);
                ++count;
            }
        }
    }
    else {
        auto* plane = reinterpret_cast<const T*>(packet.buffer[0]);
        if (!plane)
            return levels;
        const size_t totalSamples = packet.numSamples * static_cast<size_t>(channels);
        for (size_t index = 0; index < totalSamples; ++index) {
            const long double sample = std::abs(decode(plane[index]));
            sumAbs += sample;
            peak = std::max(peak, sample);
            ++count;
        }
    }

    if (count == 0)
        return levels;

    levels.level = static_cast<float>(sumAbs / static_cast<long double>(count));
    levels.peak = static_cast<float>(peak);
    levels.valid = true;
    return levels;
}


} // namespace


VoiceActivityDetector::VoiceActivityDetector(VoiceActivityDetectorConfig config)
    : _config(sanitize(std::move(config)))
{
}


void VoiceActivityDetector::setConfig(VoiceActivityDetectorConfig config)
{
    _config = sanitize(std::move(config));
    reset();
}


VoiceActivityDetectorConfig VoiceActivityDetector::config() const
{
    return _config;
}


VoiceActivityDetectorStats VoiceActivityDetector::stats() const
{
    return {
        .seen = _seen,
        .emitted = _emitted,
        .active = _active,
        .lastLevel = _lastLevel,
        .lastPeak = _lastPeak,
    };
}


void VoiceActivityDetector::reset()
{
    _sequence = 0;
    _seen = 0;
    _emitted = 0;
    _lastUpdateUsec = 0;
    _lastBelowThresholdUsec = 0;
    _active = false;
    _lastLevel = 0.0f;
    _lastPeak = 0.0f;
}


void VoiceActivityDetector::process(const av::PlanarAudioPacket& packet)
{
    const auto levels = measure(packet);
    ++_seen;
    ++_sequence;
    _lastLevel = levels.level;
    _lastPeak = levels.peak;

    if (!levels.valid)
        return;

    const int64_t timeUsec = packet.time;
    const bool aboveStart = levels.level >= _config.startThreshold;
    const bool belowStop = levels.level < _config.stopThreshold;

    if (!_active) {
        if (!aboveStart)
            return;

        _active = true;
        _lastBelowThresholdUsec = 0;
        _lastUpdateUsec = timeUsec;
        emitEvent("speech:start", packet, levels, true);
        return;
    }

    if (belowStop) {
        if (_lastBelowThresholdUsec == 0)
            _lastBelowThresholdUsec = timeUsec;

        if (_config.minSilenceUsec <= 0 ||
            (timeUsec - _lastBelowThresholdUsec) >= _config.minSilenceUsec) {
            emitEvent("speech:end", packet, levels, false);
            _active = false;
            _lastBelowThresholdUsec = 0;
            _lastUpdateUsec = timeUsec;
            return;
        }
    }
    else {
        _lastBelowThresholdUsec = 0;
    }

    if (_config.updateIntervalUsec <= 0 ||
        (timeUsec - _lastUpdateUsec) >= _config.updateIntervalUsec) {
        _lastUpdateUsec = timeUsec;
        emitEvent("speech:update", packet, levels, true);
    }
}


VoiceActivityDetectorConfig
VoiceActivityDetector::sanitize(VoiceActivityDetectorConfig config)
{
    if (config.stopThreshold < 0.0f)
        config.stopThreshold = 0.0f;
    if (config.startThreshold < config.stopThreshold)
        config.startThreshold = config.stopThreshold;
    if (config.sampleRateHint < 0)
        config.sampleRateHint = 0;
    return config;
}


VoiceActivityDetector::Levels
VoiceActivityDetector::measure(const av::PlanarAudioPacket& packet) const
{
    if (packet.sampleFmt == "flt" || packet.sampleFmt == "fltp") {
        return measureSamples<float>(packet, [](float value) {
            return std::clamp(static_cast<long double>(value), -1.0L, 1.0L);
        });
    }
    if (packet.sampleFmt == "dbl" || packet.sampleFmt == "dblp") {
        return measureSamples<double>(packet, [](double value) {
            return std::clamp(static_cast<long double>(value), -1.0L, 1.0L);
        });
    }
    if (packet.sampleFmt == "s16" || packet.sampleFmt == "s16p") {
        return measureSamples<int16_t>(packet, [](int16_t value) {
            return static_cast<long double>(value) / 32768.0L;
        });
    }
    if (packet.sampleFmt == "s32" || packet.sampleFmt == "s32p") {
        return measureSamples<int32_t>(packet, [](int32_t value) {
            return static_cast<long double>(value) / 2147483648.0L;
        });
    }
    if (packet.sampleFmt == "u8" || packet.sampleFmt == "u8p") {
        return measureSamples<uint8_t>(packet, [](uint8_t value) {
            return (static_cast<long double>(value) - 128.0L) / 128.0L;
        });
    }

    return {};
}


void VoiceActivityDetector::emitEvent(const char* type,
                                      const av::PlanarAudioPacket& packet,
                                      const Levels& levels,
                                      bool active)
{
    SpeechEvent event;
    event.type = type;
    event.source = _config.source;
    event.detector = _config.detectorName;
    event.emittedAtUsec = packet.time;
    event.audio = makeAudioRef(packet, _sequence, _config.sampleRateHint);
    event.active = active;
    event.level = levels.level;
    event.peak = levels.peak;
    event.data["startThreshold"] = _config.startThreshold;
    event.data["stopThreshold"] = _config.stopThreshold;
    Event.emit(event);
    ++_emitted;
}


} // namespace speech
} // namespace icy


/// @}
