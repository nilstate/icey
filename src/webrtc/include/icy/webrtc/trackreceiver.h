///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "icy/webrtc/webrtc.h"

#include "icy/av/packet.h"
#include "icy/packetsignal.h"
#include "icy/packetstream.h"
#include "icy/synchronizer.h"
#include "icy/timer.h"
#include "icy/webrtc/jitterbuffer.h"

#include <rtc/rtc.hpp>

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>


namespace icy {
namespace wrtc {

namespace detail {
class ReceiverJitterBuffer;
}


/// PacketStreamAdapter that receives depacketized frames from a single
/// remote libdatachannel Track and emits them as VideoPacket or
/// AudioPacket into a PacketStream.
///
/// Call setupReceiveTrack() on the track first to install the correct
/// depacketizer, then bind this receiver to it.
///
/// Usage:
///   pc->onTrack([&](shared_ptr<rtc::Track> track) {
///       if (setupReceiveTrack(track))
///           videoReceiver.bind(track);
///   });
///
///   videoReceiver.emitter += packetSlot(&recorder, &Recorder::onEncodedVideo);
///
/// Emits owning packets - the frame data is copied, so downstream
/// processors can safely queue packets asynchronously.
///
/// Emits VideoPacket for video tracks, AudioPacket for audio tracks.
/// Use those packets to drive a decoder, recorder, or custom pipeline.
/// See `samples/media-recorder` for a complete receive -> decode -> file example.
class WEBRTC_API WebRtcTrackReceiver : public PacketStreamAdapter
{
public:
    /// Construct an unbound receiver. Call bind() to attach a remote track.
    WebRtcTrackReceiver();
    ~WebRtcTrackReceiver() override;

    /// Bind to a remote track. Must be called after setupReceiveTrack()
    /// returned true.
    /// Installs an onFrame callback that converts each depacketized frame to
    /// a VideoPacket or AudioPacket and emits it on the PacketStream.
    /// The track type (video/audio) is detected from the SDP description.
    /// @param track  Remote track from the PeerConnection::onTrack callback.
    void bind(std::shared_ptr<rtc::Track> track);

    /// Replace the receive-side jitter-buffer settings.
    ///
    /// Reconfiguring resets any buffered media still waiting for release.
    void configureJitterBuffer(const JitterBufferConfig& config);

    /// Current jitter-buffer settings for this receiver.
    [[nodiscard]] JitterBufferConfig jitterBufferConfig() const;

    /// True when depacketized receive frames are buffered before emission.
    [[nodiscard]] bool jitterBufferEnabled() const;

    PacketSignal emitter;

private:
    struct DispatchState
    {
        std::atomic<bool> alive{true};
        std::atomic<uint64_t> generation{0};
    };

    void enqueue(std::unique_ptr<IPacket> packet);
    void flushPending();

    Synchronizer _dispatch;
    Timer _timer;
    mutable std::mutex _mutex;
    std::deque<std::unique_ptr<IPacket>> _pending;
    std::unique_ptr<detail::ReceiverJitterBuffer> _jitterBuffer;
    JitterBufferConfig _jitterConfig;
    std::int64_t _timerTickMs = 5;
    bool _timerNeedsUpdate = false;
    std::shared_ptr<DispatchState> _state = std::make_shared<DispatchState>();
    uint64_t _generation = 0;
};


} // namespace wrtc
} // namespace icy


/// @}
