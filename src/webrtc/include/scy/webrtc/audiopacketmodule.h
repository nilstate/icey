///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_AudioPacketModule_H
#define SCY_WebRTC_AudioPacketModule_H


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/av.h"
#include "scy/av/audiobuffer.h"
#include "scy/av/packet.h"
#include "scy/packetsignal.h"

#include "webrtc/base/basictypes.h"
#include "webrtc/base/criticalsection.h"
#include "webrtc/base/messagehandler.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/base/thread.h"
#include "webrtc/common_types.h"
#include "webrtc/modules/audio_device/include/audio_device.h"


namespace scy {
namespace wrtc {


/// This class implements an `AudioDeviceModule` that can be used to process
/// arbitrary audio packets. This class is send only, recording is not implemented
///
/// Note P postfix of a function indicates that it should only be called by the
/// processing thread.
class AudioPacketModule : public webrtc::AudioDeviceModule,
                          public rtc::MessageHandler
{
public:
    typedef uint16_t Sample;

    /// Creates a `AudioPacketModule` or returns NULL on failure.
    static rtc::scoped_refptr<AudioPacketModule> Create();

    /// Handles input packets from the capture for sending.
    void onAudioCaptured(av::AudioPacket& packet);

    /// Following functions are inherited from `webrtc::AudioDeviceModule`.
    /// Only functions called by `Peer` are implemented, the rest do
    /// nothing and return success. If a function is not expected to be called
    /// by `Peer` an assertion is triggered if it is in fact called.
    int64_t TimeUntilNextProcess() override;
    void Process() override;

    int32_t ActiveAudioLayer(AudioLayer* audio_layer) const override;

    ErrorCode LastError() const override;
    int32_t RegisterEventObserver(webrtc::AudioDeviceObserver* event_callback) override;

    /// Note: Calling this method from a callback may result in deadlock.
    int32_t RegisterAudioCallback(webrtc::AudioTransport* audio_callback) override;

    int32_t Init() override;
    int32_t Terminate() override;
    bool Initialized() const override;

    int16_t PlayoutDevices() override;
    int16_t RecordingDevices() override;
    int32_t PlayoutDeviceName(uint16_t index,
                              char name[webrtc::kAdmMaxDeviceNameSize],
                              char guid[webrtc::kAdmMaxGuidSize]) override;
    int32_t RecordingDeviceName(uint16_t index,
                                char name[webrtc::kAdmMaxDeviceNameSize],
                                char guid[webrtc::kAdmMaxGuidSize]) override;

    int32_t SetPlayoutDevice(uint16_t index) override;
    int32_t SetPlayoutDevice(WindowsDeviceType device) override;
    int32_t SetRecordingDevice(uint16_t index) override;
    int32_t SetRecordingDevice(WindowsDeviceType device) override;

    int32_t PlayoutIsAvailable(bool* available) override;
    int32_t InitPlayout() override;
    bool PlayoutIsInitialized() const override;
    int32_t RecordingIsAvailable(bool* available) override;
    int32_t InitRecording() override;
    bool RecordingIsInitialized() const override;

    int32_t StartPlayout() override;
    int32_t StopPlayout() override;
    bool Playing() const override;
    int32_t StartRecording() override;
    int32_t StopRecording() override;
    bool Recording() const override;

    int32_t SetAGC(bool enable) override;
    bool AGC() const override;

    int32_t SetWaveOutVolume(uint16_t volume_left, uint16_t volume_right) override;
    int32_t WaveOutVolume(uint16_t* volume_left, uint16_t* volume_right) const override;

    int32_t InitSpeaker() override;
    bool SpeakerIsInitialized() const override;
    int32_t InitMicrophone() override;
    bool MicrophoneIsInitialized() const override;

    int32_t SpeakerVolumeIsAvailable(bool* available) override;
    int32_t SetSpeakerVolume(uint32_t volume) override;
    int32_t SpeakerVolume(uint32_t* volume) const override;
    int32_t MaxSpeakerVolume(uint32_t* max_volume) const override;
    int32_t MinSpeakerVolume(uint32_t* min_volume) const override;
    int32_t SpeakerVolumeStepSize(uint16_t* step_size) const override;

    int32_t MicrophoneVolumeIsAvailable(bool* available) override;
    int32_t SetMicrophoneVolume(uint32_t volume) override;
    int32_t MicrophoneVolume(uint32_t* volume) const override;
    int32_t MaxMicrophoneVolume(uint32_t* max_volume) const override;

    int32_t MinMicrophoneVolume(uint32_t* min_volume) const override;
    int32_t MicrophoneVolumeStepSize(uint16_t* step_size) const override;

    int32_t SpeakerMuteIsAvailable(bool* available) override;
    int32_t SetSpeakerMute(bool enable) override;
    int32_t SpeakerMute(bool* enabled) const override;

    int32_t MicrophoneMuteIsAvailable(bool* available) override;
    int32_t SetMicrophoneMute(bool enable) override;
    int32_t MicrophoneMute(bool* enabled) const override;

    int32_t MicrophoneBoostIsAvailable(bool* available) override;
    int32_t SetMicrophoneBoost(bool enable) override;
    int32_t MicrophoneBoost(bool* enabled) const override;

    int32_t StereoPlayoutIsAvailable(bool* available) const override;
    int32_t SetStereoPlayout(bool enable) override;
    int32_t StereoPlayout(bool* enabled) const override;
    int32_t StereoRecordingIsAvailable(bool* available) const override;
    int32_t SetStereoRecording(bool enable) override;
    int32_t StereoRecording(bool* enabled) const override;
    int32_t SetRecordingChannel(const ChannelType channel) override;
    int32_t RecordingChannel(ChannelType* channel) const override;

    int32_t SetPlayoutBuffer(const BufferType type, uint16_t size_ms = 0) override;
    int32_t PlayoutBuffer(BufferType* type, uint16_t* size_ms) const override;
    int32_t PlayoutDelay(uint16_t* delay_ms) const override;
    int32_t RecordingDelay(uint16_t* delay_ms) const override;

    int32_t CPULoad(uint16_t* load) const override;

    int32_t StartRawOutputFileRecording(
        const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) override;
    int32_t StopRawOutputFileRecording() override;
    int32_t StartRawInputFileRecording(
        const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) override;
    int32_t StopRawInputFileRecording() override;

    int32_t SetRecordingSampleRate(const uint32_t samples_per_sec) override;
    int32_t RecordingSampleRate(uint32_t* samples_per_sec) const override;
    int32_t SetPlayoutSampleRate(const uint32_t samples_per_sec) override;
    int32_t PlayoutSampleRate(uint32_t* samples_per_sec) const override;

    int32_t ResetAudioDevice() override;
    int32_t SetLoudspeakerStatus(bool enable) override;
    int32_t GetLoudspeakerStatus(bool* enabled) const override;
    bool BuiltInAECIsAvailable() const override { return false; }
    int32_t EnableBuiltInAEC(bool enable) override { return -1; }
    bool BuiltInAGCIsAvailable() const override { return false; }
    int32_t EnableBuiltInAGC(bool enable) override { return -1; }
    bool BuiltInNSIsAvailable() const override { return false; }
    int32_t EnableBuiltInNS(bool enable) override { return -1; }
#if defined(WEBRTC_IOS)
    int GetPlayoutAudioParameters(webrtc::AudioParameters* params) const override
    {
        return -1;
    }
    int GetRecordAudioParameters(webrtc::AudioParameters* params) const override
    {
        return -1;
    }
#endif // WEBRTC_IOS

    /// End of functions inherited from `webrtc::AudioDeviceModule`.

    /// The following function is inherited from `rtc::MessageHandler`.
    void OnMessage(rtc::Message* msg) override;

protected:
    /// The constructor is protected because the class needs to be created as a
    /// reference counted object (for memory managment reasons). It could be
    /// exposed in which case the burden of proper instantiation would be put on
    /// the creator of a AudioPacketModule instance. To create an instance of
    /// this class use the `Create()` API.
    explicit AudioPacketModule();

    /// The destructor is protected because it is reference counted and should
    /// not be deleted directly.
    virtual ~AudioPacketModule();

private:
    /// Initializes the state of the `AudioPacketModule`. This API is called on
    /// creation by the `Create()` API.
    bool Initialize();

    /// Returns true/false depending on if recording or playback has been
    /// enabled/started.
    bool shouldStartProcessing();

    /// Starts or stops the pushing and pulling of audio frames.
    void updateProcessing(bool start);

    /// Starts the periodic calling of `ProcessFrame()` in a thread safe way.
    void startProcessP();

    /// Periodcally called function that ensures that frames are pulled and
    /// pushed
    /// periodically if enabled/started.
    void processFrameP();

    /// Pulls frames from the registered webrtc::AudioTransport.
    void receiveFrameP();

    /// Pushes frames to the registered webrtc::AudioTransport.
    void sendFrameP();

    /// The time in milliseconds when Process() was last called or 0 if no call
    /// has been made.
    int64_t _lastProcessTimeMS;

    /// Callback for playout and recording.
    webrtc::AudioTransport* _audioCallback;

    bool _recording; ///< True when audio is being pushed from the instance.
    bool _playing;   ///< True when audio is being pulled by the instance.

    bool _playIsInitialized; ///< True when the instance is ready to pull audio.
    bool _recIsInitialized;  ///< True when the instance is ready to push audio.

    /// Input to and output from RecordedDataIsAvailable(..) makes it possible
    /// to modify the current mic level. The implementation does not care about
    /// the mic level so it just feeds back what it receives.
    uint32_t _currentMicLevel;

    /// `_nextFrameTime` is updated in a non-drifting manner to indicate the
    /// next wall clock time the next frame should be generated and received.
    /// `_started` ensures that _nextFrameTime can be initialized properly on first call.
    bool _started;
    int64_t _nextFrameTime;

    std::unique_ptr<rtc::Thread> _processThread;

    /// A FIFO buffer that stores samples from the audio source to be sent.
    av::AudioBuffer _sendFifo;

    /// A buffer with enough storage for a 10ms of samples to send.
    std::vector<Sample> _sendSamples;

    /// Protects variables that are accessed from `_processThread` and
    /// the main thread.
    rtc::CriticalSection _crit;

    /// Protects |_audioCallback| that is accessed from `_processThread` and
    /// the main thread.
    rtc::CriticalSection _critCallback;
};


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG
#endif // SCY_WebRTC_AudioPacketModule_H


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
