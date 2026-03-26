#pragma once


#include "config.h"
#include "icy/av/audiopacketencoder.h"
#include "icy/av/mediacapture.h"
#include "icy/av/multiplexpacketencoder.h"
#include "icy/av/videodecoder.h"
#include "icy/av/videopacketencoder.h"
#include "icy/packetstream.h"
#include "icy/symple/server.h"
#include "icy/webrtc/peersession.h"
#include "icy/webrtc/support/sympleserversignaller.h"

#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>


namespace icy {
namespace media_server {


class MediaSession;
class VideoRecorder;
struct VideoRecorderDeleter
{
    void operator()(VideoRecorder* recorder) const;
};


class RelayController
{
public:
    void registerSession(const std::shared_ptr<MediaSession>& session);
    void unregisterSession(const std::string& peerId);
    void onSessionActive(const std::string& peerId);
    void onSessionEnded(const std::string& peerId);
    void onViewerKeyframeRequested(const std::string& peerId);
    void onViewerBitrateEstimate(const std::string& peerId, unsigned int bps);
    void relayVideo(const std::string& peerId, av::VideoPacket& packet);
    void relayAudio(const std::string& peerId, av::AudioPacket& packet);
    void clear();

private:
    std::shared_ptr<MediaSession> currentSourceLocked();
    std::shared_ptr<MediaSession> electSourceLocked();
    std::vector<std::shared_ptr<MediaSession>> relayTargetsLocked(const std::string& sourcePeerId);
    unsigned int minViewerBitrateLocked() const;

    std::mutex _mutex;
    std::unordered_map<std::string, std::weak_ptr<MediaSession>> _sessions;
    std::unordered_map<std::string, unsigned int> _viewerBitrates;
    std::string _sourcePeerId;
};


class MediaSession : public std::enable_shared_from_this<MediaSession>
{
public:
    MediaSession(const std::string& peerId,
                 smpl::Server& server,
                 const std::string& serverPeerId,
                 const Config& config,
                 RelayController* relay);
    ~MediaSession();

    wrtc::PeerSession& session();
    const std::string& peerId() const;
    void onSignallingMessage(const json::Value& msg);
    bool active() const;
    void relayVideo(av::VideoPacket& packet);
    void relayAudio(av::AudioPacket& packet);
    void requestRelayKeyframe();
    void requestRelayBitrate(unsigned int bps);

private:
    static av::VideoCodec makeVideoCodec(const Config& config);
    static av::AudioCodec makeAudioCodec(const Config& config);

    void startStreaming();
    void stopStreaming();
    void startRecording();
    void stopRecording();
    void startRelay();
    void stopRelay();
    void onRelayedVideo(av::VideoPacket& packet);
    void onRelayedAudio(av::AudioPacket& packet);

    std::string _peerId;
    PacketStream _stream;
    const Config& _config;
    RelayController* _relay = nullptr;
    std::shared_ptr<av::MediaCapture> _capture;
    std::shared_ptr<av::VideoPacketEncoder> _videoEncoder;
    std::shared_ptr<av::AudioPacketEncoder> _audioEncoder;
    wrtc::WebRtcTrackSender* _videoSender = nullptr;
    wrtc::WebRtcTrackSender* _audioSender = nullptr;
    bool _streamReady = false;
    bool _relayAttached = false;
    std::unique_ptr<VideoRecorder, VideoRecorderDeleter> _recorder;
    std::unique_ptr<wrtc::SympleServerSignaller> _signaller;
    std::unique_ptr<wrtc::PeerSession> _session;
    mutable std::mutex _mutex;
};


} // namespace media_server
} // namespace icy
