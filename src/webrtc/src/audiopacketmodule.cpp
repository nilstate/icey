///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "scy/webrtc/audiopacketmodule.h"

#ifdef HAVE_FFMPEG

#include "webrtc/base/refcount.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/timeutils.h"

#include "scy/logger.h"


using std::endl;


namespace scy {
namespace wrtc {


// Constants correspond to 10ms of sterio audio at 44kHz.
static const uint8_t kNumberOfChannels = 2;
static const int kSamplesPerSecond = 44000;
static const size_t kNumberSamples = 440;
static const size_t kBytesPerSample = sizeof(AudioPacketModule::Sample) * kNumberOfChannels;
static const size_t kBufferBytes = kNumberSamples * kBytesPerSample;

static const int kTimePerFrameMs = 10;
static const int kTotalDelayMs = 0;
static const int kClockDriftMs = 0;
static const uint32_t kMaxVolume = 14392;

// Same value as src/modules/audio_device/main/source/audio_device_config.h in
// https://code.google.com/p/webrtc/
static const int kAdmMaxIdleTimeProcess = 1000;


enum
{
    MSG_START_PROCESS,
    MSG_RUN_PROCESS
};


AudioPacketModule::AudioPacketModule()
    : _lastProcessTimeMS(0)
    , _audioCallback(nullptr)
    , _recording(false)
    , _playing(false)
    , _playIsInitialized(false)
    , _recIsInitialized(false)
    , _currentMicLevel(kMaxVolume)
    , _started(false)
    , _nextFrameTime(0)
    , _sendSamples(kBufferBytes)
{
}

AudioPacketModule::~AudioPacketModule()
{
    if (_processThread) {
        _processThread->Stop();
    }
}

rtc::scoped_refptr<AudioPacketModule> AudioPacketModule::Create()
{
    rtc::scoped_refptr<AudioPacketModule> capture_module(
        new rtc::RefCountedObject<AudioPacketModule>());
    if (!capture_module->Initialize()) {
        return nullptr;
    }
    return capture_module;
}

void AudioPacketModule::onAudioCaptured(av::AudioPacket& packet)
{
    LTrace("Audio frame captured")

    // assert(_processThread->IsCurrent());
    rtc::CritScope cs(&_critCallback);
    if (!_audioCallback || !_recording) {
        return;
    }

    // TODO: Implement planar formats
    auto data = packet.data();
    _sendFifo.write((void**)&data, packet.numSamples);
}

void AudioPacketModule::OnMessage(rtc::Message* msg)
{
    switch (msg->message_id) {
        case MSG_START_PROCESS:
            startProcessP();
            break;
        case MSG_RUN_PROCESS:
            processFrameP();
            break;
        default:
            // All existing messages should be caught. 
            // Getting here should never happen.
            assert(false);
    }
}

bool AudioPacketModule::Initialize()
{
    // Allocate the send audio FIFO buffer
    _sendFifo.close();
    _sendFifo.alloc("s16", kNumberOfChannels);

    _lastProcessTimeMS = rtc::TimeMillis();
    return true;
}

bool AudioPacketModule::shouldStartProcessing()
{
    return _recording || _playing;
    // return _recording;
}

void AudioPacketModule::updateProcessing(bool start)
{
    if (start) {
        if (!_processThread) {
            _processThread.reset(new rtc::Thread());
            _processThread->Start();
        }
        _processThread->Post(RTC_FROM_HERE, this, MSG_START_PROCESS);
    } else {
        if (_processThread) {
            _processThread->Stop();
            _processThread.reset(nullptr);
        }
        _started = false;
    }
}

void AudioPacketModule::startProcessP()
{
    assert(_processThread->IsCurrent());
    if (_started) {
        // Already started.
        return;
    }
    processFrameP();
}

void AudioPacketModule::processFrameP()
{
    assert(_processThread->IsCurrent());
    if (!_started) {
        _nextFrameTime = rtc::TimeMillis();
        _started = true;
    }

    {
        rtc::CritScope cs(&_crit);
        // Receive and send frames every kTimePerFrameMs.
        if (_playing) {
            receiveFrameP();
        }
        if (_recording) {
            sendFrameP();
        }
    }

    _nextFrameTime += kTimePerFrameMs;
    const int64_t current_time = rtc::TimeMillis();
    const int64_t wait_time = (_nextFrameTime > current_time) ? _nextFrameTime - current_time : 0;
    _processThread->PostDelayed(RTC_FROM_HERE, wait_time, this, MSG_RUN_PROCESS);
}


void AudioPacketModule::sendFrameP()
{
    assert(_processThread->IsCurrent());
    rtc::CritScope cs(&_critCallback);
    LDebug("Send audio", _recording)
    if (!_audioCallback) {
        return;
    }

    auto samples = &_sendSamples[0];
    if (!_sendFifo.read((void**)&samples, kNumberSamples)) {
        LDebug("No audio frames in send buffer")
        return;
    }

    bool key_pressed = false;
    uint32_t current_mic_level = 0;
    MicrophoneVolume(&current_mic_level);

    LTrace("Send audio")
    if (_audioCallback->RecordedDataIsAvailable(
            samples, kNumberSamples, kBytesPerSample, kNumberOfChannels,
            kSamplesPerSecond, kTotalDelayMs, kClockDriftMs, current_mic_level,
            key_pressed, current_mic_level) != 0) {
        assert(false);
    }

    SetMicrophoneVolume(current_mic_level);
}

void AudioPacketModule::receiveFrameP()
{
    assert(_processThread->IsCurrent());
    assert(0 && "playout not implemented");

    // {
    //   rtc::CritScope cs(&_critCallback);
    //   if (!_audioCallback) {
    //     return;
    //   }
    //   size_t nSamplesOut = 0;
    //   int64_t elapsed_time_ms = 0;
    //   int64_t ntp_time_ms = 0;
    //   if (_audioCallback->NeedMorePlayData(kNumberSamples,
    //                                        kBytesPerSample,
    //                                        kNumberOfChannels,
    //                                        kSamplesPerSecond,
    //                                        rec_sendFifo_, nSamplesOut,
    //                                        &elapsed_time_ms, &ntp_time_ms) !=
    //                                        0) {
    //     assert(false);
    //   }
    //   assert(nSamplesOut == kNumberSamples);
    // }
}

int64_t AudioPacketModule::TimeUntilNextProcess()
{
    const int64_t current_time = rtc::TimeMillis();
    if (current_time < _lastProcessTimeMS) {
        // TODO: wraparound could be handled more gracefully.
        return 0;
    }
    const int64_t elapsed_time = current_time - _lastProcessTimeMS;
    if (kAdmMaxIdleTimeProcess < elapsed_time) {
        return 0;
    }
    return kAdmMaxIdleTimeProcess - elapsed_time;
}

void AudioPacketModule::Process()
{
    _lastProcessTimeMS = rtc::TimeMillis();
}

int32_t AudioPacketModule::ActiveAudioLayer(AudioLayer* /*audio_layer*/) const
{
    assert(false);
    return 0;
}

webrtc::AudioDeviceModule::ErrorCode AudioPacketModule::LastError() const
{
    assert(false);
    return webrtc::AudioDeviceModule::kAdmErrNone;
}

int32_t AudioPacketModule::RegisterEventObserver(webrtc::AudioDeviceObserver* /*event_callback*/)
{
    // Only used to report warnings and errors. This fake implementation won't
    // generate any so discard this callback.
    return 0;
}

int32_t AudioPacketModule::RegisterAudioCallback(webrtc::AudioTransport* audio_callback)
{
    rtc::CritScope cs(&_critCallback);
    _audioCallback = audio_callback;
    return 0;
}

int32_t AudioPacketModule::Init()
{
    // Initialize is called by the factory method.
    // Safe to ignore this Init call.
    return 0;
}

int32_t AudioPacketModule::Terminate()
{
    // Clean up in the destructor. No action here, just success.
    return 0;
}

bool AudioPacketModule::Initialized() const
{
    assert(false);
    return 0;
}

int16_t AudioPacketModule::PlayoutDevices()
{
    assert(false);
    return 0;
}

int16_t AudioPacketModule::RecordingDevices()
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::PlayoutDeviceName(uint16_t /*index*/,
    char /*name*/[webrtc::kAdmMaxDeviceNameSize],
    char /*guid*/[webrtc::kAdmMaxGuidSize])
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::RecordingDeviceName(uint16_t /*index*/,
    char /*name*/[webrtc::kAdmMaxDeviceNameSize],
    char /*guid*/[webrtc::kAdmMaxGuidSize])
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetPlayoutDevice(uint16_t /*index*/)
{
    // No playout device, just playing from file. Return success.
    return 0;
}

int32_t AudioPacketModule::SetPlayoutDevice(WindowsDeviceType /*device*/)
{
    if (_playIsInitialized) {
        return -1;
    }
    return 0;
}

int32_t AudioPacketModule::SetRecordingDevice(uint16_t /*index*/)
{
    // No recording device, just dropping audio. Return success.
    return 0;
}

int32_t AudioPacketModule::SetRecordingDevice(WindowsDeviceType /*device*/)
{
    if (_recIsInitialized) {
        return -1;
    }
    return 0;
}

int32_t AudioPacketModule::PlayoutIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::InitPlayout()
{
    _playIsInitialized = true;
    return 0;
}

bool AudioPacketModule::PlayoutIsInitialized() const
{
    return _playIsInitialized;
}

int32_t AudioPacketModule::RecordingIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::InitRecording()
{
    _recIsInitialized = true;
    return 0;
}

bool AudioPacketModule::RecordingIsInitialized() const
{
    return _recIsInitialized;
}

int32_t AudioPacketModule::StartPlayout()
{
    LDebug("Start playout")
    if (!_playIsInitialized) {
        return -1;
    }
    {
        rtc::CritScope cs(&_crit);
        _playing = true;
    }
    bool start = true;
    updateProcessing(start);
    return 0;
}

int32_t AudioPacketModule::StopPlayout()
{
    LDebug("Stop playout")
    bool start = false;
    {
        rtc::CritScope cs(&_crit);
        _playing = false;
        start = shouldStartProcessing();
    }
    updateProcessing(start);
    return 0;
}

bool AudioPacketModule::Playing() const
{
    rtc::CritScope cs(&_crit);
    return _playing;
}

int32_t AudioPacketModule::StartRecording()
{
    LDebug("Start recording")
    if (!_recIsInitialized) {
        return -1;
    }
    {
        rtc::CritScope cs(&_crit);
        _recording = true;
    }
    bool start = true;
    updateProcessing(start);
    return 0;
}

int32_t AudioPacketModule::StopRecording()
{
    LDebug("Stop recording")
    bool start = false;
    {
        rtc::CritScope cs(&_crit);
        _recording = false;
        start = shouldStartProcessing();
    }
    updateProcessing(start);
    return 0;
}

bool AudioPacketModule::Recording() const
{
    rtc::CritScope cs(&_crit);
    return _recording;
}

int32_t AudioPacketModule::SetAGC(bool /*enable*/)
{
    // No AGC but not needed since audio is pregenerated. Return success.
    return 0;
}

bool AudioPacketModule::AGC() const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetWaveOutVolume(uint16_t /*volume_left*/,
                                            uint16_t /*volume_right*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::WaveOutVolume(uint16_t* /*volume_left*/,
                                         uint16_t* /*volume_right*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::InitSpeaker()
{
    // No speaker, just playing from file. Return success.
    return 0;
}

bool AudioPacketModule::SpeakerIsInitialized() const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::InitMicrophone()
{
    // No microphone, just playing from file. Return success.
    return 0;
}

bool AudioPacketModule::MicrophoneIsInitialized() const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SpeakerVolumeIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetSpeakerVolume(uint32_t /*volume*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SpeakerVolume(uint32_t* /*volume*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MaxSpeakerVolume(uint32_t* /*max_volume*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MinSpeakerVolume(uint32_t* /*min_volume*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SpeakerVolumeStepSize(uint16_t* /*step_size*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MicrophoneVolumeIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetMicrophoneVolume(uint32_t volume)
{
    rtc::CritScope cs(&_crit);
    _currentMicLevel = volume;
    return 0;
}

int32_t AudioPacketModule::MicrophoneVolume(uint32_t* volume) const
{
    rtc::CritScope cs(&_crit);
    *volume = _currentMicLevel;
    return 0;
}

int32_t AudioPacketModule::MaxMicrophoneVolume(uint32_t* max_volume) const
{
    *max_volume = kMaxVolume;
    return 0;
}

int32_t AudioPacketModule::MinMicrophoneVolume(uint32_t* /*min_volume*/) const
{
    assert(false);
    return 0;
}

int32_t
AudioPacketModule::MicrophoneVolumeStepSize(uint16_t* /*step_size*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SpeakerMuteIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetSpeakerMute(bool /*enable*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SpeakerMute(bool* /*enabled*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MicrophoneMuteIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetMicrophoneMute(bool /*enable*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MicrophoneMute(bool* /*enabled*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MicrophoneBoostIsAvailable(bool* /*available*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetMicrophoneBoost(bool /*enable*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::MicrophoneBoost(bool* /*enabled*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::StereoPlayoutIsAvailable(bool* available) const
{
    // No recording device, just dropping audio. Stereo can be dropped just
    // as easily as mono.
    *available = true;
    return 0;
}

int32_t AudioPacketModule::SetStereoPlayout(bool /*enable*/)
{
    // No recording device, just dropping audio. Stereo can be dropped just
    // as easily as mono.
    return 0;
}

int32_t AudioPacketModule::StereoPlayout(bool* /*enabled*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::StereoRecordingIsAvailable(bool* available) const
{
    // Keep thing simple. No stereo recording.
    *available = false;
    return 0;
}

int32_t AudioPacketModule::SetStereoRecording(bool enable)
{
    if (!enable) {
        return 0;
    }
    return -1;
}

int32_t AudioPacketModule::StereoRecording(bool* /*enabled*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetRecordingChannel(const ChannelType channel)
{
    if (channel != AudioDeviceModule::kChannelBoth) {
        // There is no right or left in mono. I.e. kChannelBoth should be used
        // for
        // mono.
        assert(false);
        return -1;
    }
    return 0;
}

int32_t AudioPacketModule::RecordingChannel(ChannelType* channel) const
{
    // Stereo recording not supported. However, WebRTC ADM returns kChannelBoth
    // in that case. Do the same here.
    *channel = AudioDeviceModule::kChannelBoth;
    return 0;
}

int32_t AudioPacketModule::SetPlayoutBuffer(const BufferType /*type*/,
                                            uint16_t /*size_ms*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::PlayoutBuffer(BufferType* /*type*/,
                                         uint16_t* /*size_ms*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::PlayoutDelay(uint16_t* delay_ms) const
{
    // No delay since audio frames are dropped.
    *delay_ms = 0;
    return 0;
}

int32_t AudioPacketModule::RecordingDelay(uint16_t* /*delay_ms*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::CPULoad(uint16_t* /*load*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::StartRawOutputFileRecording(
    const char /*pcm_file_name_utf8*/[webrtc::kAdmMaxFileNameSize])
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::StopRawOutputFileRecording()
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::StartRawInputFileRecording(
    const char /*pcm_file_name_utf8*/[webrtc::kAdmMaxFileNameSize])
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::StopRawInputFileRecording()
{
    assert(false);
    return 0;
}

int32_t
AudioPacketModule::SetRecordingSampleRate(const uint32_t /*samples_per_sec*/)
{
    assert(false);
    return 0;
}

int32_t
AudioPacketModule::RecordingSampleRate(uint32_t* /*samples_per_sec*/) const
{
    assert(false);
    return 0;
}

int32_t
AudioPacketModule::SetPlayoutSampleRate(const uint32_t /*samples_per_sec*/)
{
    assert(false);
    return 0;
}

int32_t
AudioPacketModule::PlayoutSampleRate(uint32_t* /*samples_per_sec*/) const
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::ResetAudioDevice()
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::SetLoudspeakerStatus(bool /*enable*/)
{
    assert(false);
    return 0;
}

int32_t AudioPacketModule::GetLoudspeakerStatus(bool* /*enabled*/) const
{
    assert(false);
    return 0;
}


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG


/// @\}


/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
