///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "scy/webrtc/peerfactorycontext.h"
#include "scy/webrtc/fakeaudiodevicemodule.h"
//#include "pc/test/fakeaudiocapturemodule.h"
#include "scy/logger.h"

#include "api/peerconnectionfactoryproxy.h"
#include "api/peerconnectionproxy.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "p2p/base/basicpacketsocketfactory.h"
#include "p2p/client/basicportallocator.h"
#include "pc/peerconnection.h"


using std::endl;


namespace scy {
namespace wrtc {


PeerFactoryContext::PeerFactoryContext(
    webrtc::AudioDeviceModule* default_adm,
    cricket::WebRtcVideoEncoderFactory* video_encoder_factory,
    cricket::WebRtcVideoDecoderFactory* video_decoder_factory,
    rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder_factory,
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder_factory)
{
    // Setup threads
    networkThread = rtc::Thread::CreateWithSocketServer();
    workerThread = rtc::Thread::Create();
    if (!networkThread->Start() || !workerThread->Start())
        throw std::runtime_error("Failed to start WebRTC threads");

    // Init required builtin factories if not provided
    if (!audio_encoder_factory)
        audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
    if (!audio_decoder_factory)
        audio_decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();

    // Create the factory
    factory = webrtc::CreatePeerConnectionFactory(
        networkThread.get(), workerThread.get(), rtc::Thread::Current(),
        default_adm, audio_encoder_factory, audio_decoder_factory,
        video_encoder_factory, video_decoder_factory);

    // if (audio_encoder_factory || audio_decoder_factory) {
    //     factory = webrtc::CreatePeerConnectionFactory(
    //         networkThread.get(), workerThread.get(), rtc::Thread::Current(),
    //         default_adm, audio_encoder_factory, audio_decoder_factory,
    //         video_encoder_factory, video_decoder_factory);
    // }
    // else {
    //     factory = webrtc::CreatePeerConnectionFactory(
    //         networkThread.get(), workerThread.get(), rtc::Thread::Current(),
    //         default_adm, video_encoder_factory, video_decoder_factory);
    // }

    if (!factory)
        throw std::runtime_error("Failed to create WebRTC factory");
}


void PeerFactoryContext::initCustomNetworkManager()
{
    networkManager.reset(new rtc::BasicNetworkManager());
    socketFactory.reset(new rtc::BasicPacketSocketFactory(networkThread.get()));
}


} } // namespace scy::wrtc


/// @\}
