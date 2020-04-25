///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_RecordingPeer_H
#define SCY_WebRTC_RecordingPeer_H


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/av.h"
#include "scy/av/mediacapture.h"
#include "scy/webrtc/multiplexmediacapturer.h"
#include "scy/webrtc/peer.h"
#include "scy/webrtc/streamrecorder.h"


namespace scy {
namespace wrtc {


/// Peer connection class for recording the remote peer video.
class RecordingPeer : public Peer
{
public:
    /// Create the recording peer connection.
    RecordingPeer(PeerManager* manager,
                  PeerFactoryContext* context,
                  const std::string& peerid,
                  const std::string& token,
                  const av::EncoderOptions& options = av::EncoderOptions());
    virtual ~RecordingPeer();

    /// inherited from PeerObserver
    virtual void OnAddStream(webrtc::MediaStreamInterface* stream) override;
    virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream) override;
    virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;

protected:
    std::unique_ptr<StreamRecorder> _recorder;
};


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG
#endif
