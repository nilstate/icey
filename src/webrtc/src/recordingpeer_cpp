///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "scy/webrtc/recordingpeer.h"

#ifdef HAVE_FFMPEG

#include "scy/logger.h"
#include "scy/webrtc/peermanager.h"
#include "scy/webrtc/videopacketsource.h"


using std::endl;


namespace scy {
namespace wrtc {


RecordingPeer::RecordingPeer(PeerManager* manager,
                             PeerFactoryContext* context,
                             const std::string& peerid,
                             const std::string& token,
                             const av::EncoderOptions& options)
    : Peer(manager, context, peerid, token, Peer::Offer)
    , _recorder(new StreamRecorder(options))
{
    // _constraints.SetMandatoryReceiveAudio(false);
    // _constraints.SetMandatoryReceiveVideo(false);
    // _constraints.SetAllowDtlsSctpDataChannels();
}


RecordingPeer::~RecordingPeer()
{
}


// rtc::scoped_refptr<webrtc::MediaStreamInterface> RecordingPeer::createMediaStream()
// {
//     assert(_mode == Offer);
//     assert(_factory);
//     assert(!_stream);
//     // assert(!_capture);
//     _stream = _factory->CreateLocalMediaStream(kStreamLabel);
//
//     _capturer.openFile(_file);
//     _capturer.addMediaTracks(_factory, _stream);
//
//     return _stream;
// }


void RecordingPeer::OnAddStream(webrtc::MediaStreamInterface* stream)
{
    assert(_mode == Answer);

    LDebug(_peerid, ": On add stream")
    _manager->onAddRemoteStream(this, stream);

    // Set tracks
    auto videoTracks = stream->GetVideoTracks();
    if (!videoTracks.empty())
        _recorder->setVideoTrack(videoTracks[0]);

    auto audioTracks = stream->GetAudioTracks();
    if (!audioTracks.empty())
        _recorder->setAudioTrack(audioTracks[0]);
}


void RecordingPeer::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
    assert(_mode == Answer);

    LDebug(_peerid, ": On remove stream")
    _manager->onRemoveRemoteStream(this, stream);

    assert(0 && "free streams");
}


// void RecordingPeer::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
// {
//     switch(new_state) {
//     case webrtc::PeerConnectionInterface::kStable:
//         break;
//     case webrtc::PeerConnectionInterface::kClosed:
//         _recorder.reset(); // shutdown the recorder
//         break;
//     case webrtc::PeerConnectionInterface::kHaveLocalOffer:
//     case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
//         break;
//     case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
//     case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
//         _capture->start();
//         break;
//     }
//
//     Peer::OnSignalingChange(new_state);
// }


void RecordingPeer::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    LDebug(_peerid, ": On ICE gathering change: ", new_state)

    switch (new_state) {
        case webrtc::PeerConnectionInterface::kIceConnectionNew:
        case webrtc::PeerConnectionInterface::kIceConnectionChecking:
        case webrtc::PeerConnectionInterface::kIceConnectionConnected:
            break;
        case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
            break;
        case webrtc::PeerConnectionInterface::kIceConnectionFailed:
        case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
        case webrtc::PeerConnectionInterface::kIceConnectionClosed:
        case webrtc::PeerConnectionInterface::kIceConnectionMax:
            break;
    }

    Peer::OnIceConnectionChange(new_state);
}


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG