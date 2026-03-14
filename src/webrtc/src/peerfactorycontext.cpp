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

#include "api/create_peerconnection_factory.h"
#include "api/peer_connection_factory_proxy.h"
#include "api/peer_connection_proxy.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "p2p/base/basic_packet_socket_factory.h"
#include "p2p/client/basic_port_allocator.h"
#include "pc/peer_connection.h"


using std::endl;


namespace scy {
namespace wrtc {


PeerFactoryContext::PeerFactoryContext(
    webrtc::AudioDeviceModule* default_adm,
    webrtc::VideoEncoderFactory* video_encoder_factory,
    webrtc::VideoDecoderFactory* video_decoder_factory,
    rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder_factory,
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder_factory,
    rtc::scoped_refptr<webrtc::AudioMixer> audio_mixer,
    rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing)
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
        std::unique_ptr<webrtc::VideoEncoderFactory>(video_encoder_factory), std::unique_ptr<webrtc::VideoDecoderFactory>(video_decoder_factory),
        audio_mixer, audio_processing);

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
