///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_PeerFactoryContext_H
#define SCY_WebRTC_PeerFactoryContext_H


#include "scy/webrtc/webrtc.h"

#include "webrtc/pc/peerconnectionfactory.h"


namespace scy {
namespace wrtc {


class PeerFactoryContext
{
public:
    PeerFactoryContext(
        webrtc::AudioDeviceModule* default_adm = nullptr,
        cricket::WebRtcVideoEncoderFactory* video_encoder_factory = nullptr,
        cricket::WebRtcVideoDecoderFactory* video_decoder_factory = nullptr,
        rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder_factory = nullptr,
        rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder_factory = nullptr);

    void initCustomNetworkManager();

    std::unique_ptr<rtc::Thread> networkThread;
    std::unique_ptr<rtc::Thread> workerThread;
    std::unique_ptr<rtc::NetworkManager> networkManager;
    std::unique_ptr<rtc::PacketSocketFactory> socketFactory;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;
    // rtc::scoped_refptr<webrtc::AudioDeviceModule> audioDeviceManager;
};


} } // namespace scy::wrtc


#endif


/// @\}
