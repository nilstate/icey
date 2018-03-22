///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_FakeAudioDeviceModule_H
#define SCY_WebRTC_FakeAudioDeviceModule_H


#include "scy/base.h"

#include "rtc_base/basictypes.h"
#include "rtc_base/criticalsection.h"
#include "rtc_base/messagehandler.h"
#include "rtc_base/scoped_ref_ptr.h"
#include "rtc_base/thread.h"
#include "common_types.h"
#include "modules/audio_device/include/audio_device.h"


namespace scy {
namespace wrtc {


/// This class implements a fake `AudioDeviceModule` that does absolutely nothing.
class FakeAudioDeviceModule : public webrtc::AudioDeviceModule,
                              public rtc::MessageHandler
{
public:
    /// Creates a `FakeAudioDeviceModule` or returns NULL on failure.
    static rtc::scoped_refptr<FakeAudioDeviceModule> Create();

    /// Following functions are inherited from `webrtc::AudioDeviceModule`.
    /// Only functions called by `Peer` are implemented, the rest do
    /// nothing and return success. If a function is not expected to be called
    /// by `Peer` an assertion is triggered if it is in fact called.
    // int64_t TimeUntilNextProcess() override;
    // void Process() override;

    int32_t ActiveAudioLayer(AudioLayer* audio_layer) const override;

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

    int32_t InitSpeaker() override;
    bool SpeakerIsInitialized() const override;
    int32_t InitMicrophone() override;
    bool MicrophoneIsInitialized() const override;

    int32_t SpeakerVolumeIsAvailable(bool* available) override;
    int32_t SetSpeakerVolume(uint32_t volume) override;
    int32_t SpeakerVolume(uint32_t* volume) const override;
    int32_t MaxSpeakerVolume(uint32_t* max_volume) const override;
    int32_t MinSpeakerVolume(uint32_t* min_volume) const override;

    int32_t MicrophoneVolumeIsAvailable(bool* available) override;
    int32_t SetMicrophoneVolume(uint32_t volume) override;
    int32_t MicrophoneVolume(uint32_t* volume) const override;
    int32_t MinMicrophoneVolume(uint32_t* min_volume) const override;
    int32_t MaxMicrophoneVolume(uint32_t* max_volume) const override;

    int32_t SpeakerMuteIsAvailable(bool* available) override;
    int32_t SetSpeakerMute(bool enable) override;
    int32_t SpeakerMute(bool* enabled) const override;

    int32_t MicrophoneMuteIsAvailable(bool* available) override;
    int32_t SetMicrophoneMute(bool enable) override;
    int32_t MicrophoneMute(bool* enabled) const override;

    int32_t StereoPlayoutIsAvailable(bool* available) const override;
    int32_t SetStereoPlayout(bool enable) override;
    int32_t StereoPlayout(bool* enabled) const override;
    int32_t StereoRecordingIsAvailable(bool* available) const override;
    int32_t SetStereoRecording(bool enable) override;
    int32_t StereoRecording(bool* enabled) const override;

    int32_t PlayoutDelay(uint16_t* delay_ms) const override;

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
    /// the creator of a FakeAudioDeviceModule instance. To create an instance of
    /// this class use the `Create()` API.
    explicit FakeAudioDeviceModule();

    /// The destructor is protected because it is reference counted and should
    /// not be deleted directly.
    virtual ~FakeAudioDeviceModule();

private:
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

    /// Protects variables for multithread access.
    rtc::CriticalSection _crit;
};


} } // namespace scy::wrtc


#endif // SCY_WebRTC_FakeAudioDeviceModule_H


/// @\}
