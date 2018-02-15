///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_MultiplexMediaCapturer_H
#define SCY_WebRTC_MultiplexMediaCapturer_H


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/av.h"
#include "scy/av/mediacapture.h"
#include "scy/webrtc/audiopacketmodule.h"
#include "scy/webrtc/videopacketsource.h"

#include "webrtc/api/peerconnectioninterface.h"


namespace scy {
namespace wrtc {


class MultiplexMediaCapturer
{
public:
    MultiplexMediaCapturer();
    ~MultiplexMediaCapturer();

    void openFile(const std::string& file, bool loop = true);

    void addMediaTracks(webrtc::PeerConnectionFactoryInterface* factory,
                        webrtc::MediaStreamInterface* stream);

    void start();
    void stop();

    rtc::scoped_refptr<AudioPacketModule> getAudioModule();
    VideoPacketSource* createVideoSource();

protected:
    PacketStream _stream;
    av::MediaCapture::Ptr _videoCapture;
    rtc::scoped_refptr<AudioPacketModule> _audioModule;
};


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG
#endif


/// @\}
