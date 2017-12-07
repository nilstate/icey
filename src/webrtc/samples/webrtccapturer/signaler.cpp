///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
///


#include "signaler.h"
#include "util.h"

#include "scy/av/codec.h"
#include "scy/av/format.h"
#include "scy/filesystem.h"
#include "scy/util.h"

#include "scy/webrtc/peerfactorycontext.h"
#include "scy/webrtc/util.h"

#include <iostream>
#include <string>


using std::endl;


namespace scy {


Signaler::Signaler(const smpl::Client::Options& options)
    : _client(options)
{
    _client.StateChange += slot(this, &Signaler::onClientStateChange);
    _client.roster().ItemAdded += slot(this, &Signaler::onPeerConnected);
    _client.roster().ItemRemoved += slot(this, &Signaler::onPeerDiconnected);
    _client += packetSlot(this, &Signaler::onPeerCommand);
    _client += packetSlot(this, &Signaler::onPeerEvent);
    _client += packetSlot(this, &Signaler::onPeerMessage);
    _client.connect();

    _context.initCustomNetworkManager();
}


Signaler::~Signaler()
{
}


void Signaler::sendSDP(wrtc::Peer* conn, const std::string& type,
                       const std::string& sdp)
{
    assert(type == "offer" || type == "answer");

    smpl::Event e({
      { "name", "ice:sdp" },
      { "sdp", {
        { wrtc::kSessionDescriptionTypeName, type },
        { wrtc::kSessionDescriptionSdpName, sdp }
      }}
    });

    postMessage(e);
}


void Signaler::sendCandidate(wrtc::Peer* conn, const std::string& mid,
                             int mlineindex, const std::string& sdp)
{
    smpl::Event e({
      { "name", "ice:candidate" },
      { "candidate", {
        { wrtc::kCandidateSdpMidName, mid },
        { wrtc::kCandidateSdpMlineIndexName, mlineindex},
        { wrtc::kCandidateSdpName, sdp}
      }}
    });

    postMessage(e);
}


void Signaler::onPeerConnected(smpl::Peer& peer)
{
    if (peer.id() == _client.ourID())
        return;
    LDebug("Peer connected: ", peer.id())

    if (wrtc::PeerManager::exists(peer.id())) {
        LDebug("Peer already has a session: ", peer.id())
    }
}


void Signaler::onPeerCommand(smpl::Command& c)
{
    LDebug("Peer command: ", c.from().toString())

    // List available devices for streaming
    if (c.node() == "streaming:devices") {
        json::value devices;
        auto devs = wrtc::getVideoCaptureDevices();
        for (auto dev : devs) {
            devices.push_back(dev);
        }

        c.setData("devices", devices);
        c.setStatus(200);
        _client.respond(c);
    }

    // List available files for streaming
    else if (c.node() == "streaming:files") {
        json::value files;
        StringVec nodes;
        fs::readdir(getDataDirectory(), nodes);
        for (auto node : nodes) {
            files.push_back(node);
        }

        c.setData("files", files);
        c.setStatus(200);
        _client.respond(c);
    }

    // Start a streaming session
    else if (c.node() == "streaming:start") {
        std::string device = c.data("device").get<std::string>();

        // Stream a file from the filesystem like so:
        // std::string file = c.data("file").get<std::string>();
        // std::string filePath(getDataDirectory());
        // fs::addnode(filePath, file);

        // Create the Peer Connection
        auto conn = new wrtc::Peer(this, &_context, c.from().id, c.id(), wrtc::Peer::Offer);
        conn->constraints().SetMandatoryReceiveAudio(false);
        conn->constraints().SetMandatoryReceiveVideo(false);
        conn->constraints().SetAllowDtlsSctpDataChannels();

        // conn->setPortRange(40000, 40001);

        // Create tracks and device captures
        rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
            _context.factory->CreateAudioTrack(wrtc::kAudioLabel,
                _context.factory->CreateAudioSource(nullptr)));
        rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
            _context.factory->CreateVideoTrack(wrtc::kVideoLabel,
                _context.factory->CreateVideoSource(
                    wrtc::openWebRtcVideoCaptureDevice(device), nullptr)));

        // Create the media stream and attach device captures
        auto stream = conn->createMediaStream();
        stream->AddTrack(audio_track);
        stream->AddTrack(video_track);

        // Send the Offer SDP
        conn->createConnection();
        conn->createOffer();

        wrtc::PeerManager::add(c.from().id, conn);

        c.setStatus(200);
        _client.respond(c);
    }

    // Stop streaming
    else if (c.node() == "streaming:stop") {
        auto conn = wrtc::PeerManager::get(c.from().id, false);
        if (conn) {
            LDebug("Closing peer connection: ", c.from().id)
            conn->closeConnection(); // will be deleted via callback
        }
    }

    // Start a recording session
    // else if (c.node() == "recording:start") {
    //
    //    av::EncoderOptions options;
    //    options.ofile = OUTPUT_FILENAME;
    //    options.oformat = OUTPUT_FORMAT;
    //
    //    auto conn = new RecordingPeerConnection(this, c.from().id, c.id(), options);
    //    conn->constraints().SetMandatoryReceiveVideo(true);
    //    conn->constraints().SetMandatoryReceiveAudio(true);
    //    conn->createConnection();
    //    wrtc::PeerManager::add(c.id(), conn);
    //
    //    c.setStatus(200);
    //    _client.respond(c);
    //    // _client.persistence().add(c.id(), reinterpret_cast<smpl::Message *>(c.clone()), 0);
    // }
}


void Signaler::onPeerEvent(smpl::Event& e)
{
    LDebug("Peer event: ", e.from().toString())

    if (e.name() == "ice:sdp") {
        recvSDP(e.from().id, e["sdp"]);
    }
    else if (e.name() == "ice:candidate") {
        recvCandidate(e.from().id, e["candidate"]);
    }
}


void Signaler::onPeerMessage(smpl::Message& m)
{
    LDebug("Peer message: ", m.from().toString())
}


void Signaler::onPeerDiconnected(const smpl::Peer& peer)
{
    LDebug("Peer disconnected")

    // TODO: Loop all and close for peer

    auto conn = get(peer.id(), false);
    if (conn) {
        LDebug("Closing peer connection: ", peer.id())
        conn->closeConnection(); // will be deleted via callback
    }
}


void Signaler::onClientStateChange(void* sender, sockio::ClientState& state,
                                   const sockio::ClientState& oldState)
{
    LDebug("Client state changed from ", oldState, " to ", state)

    switch (state.id()) {
        case sockio::ClientState::Connecting:
            break;
        case sockio::ClientState::Connected:
            break;
        case sockio::ClientState::Online:
            break;
        case sockio::ClientState::Error:
            throw std::runtime_error("Cannot connect to Symple server. "
                                     "Did you start the demo app and the "
                                     "Symple server is running on port 4500?");
    }
}


void Signaler::onAddRemoteStream(wrtc::Peer* conn, webrtc::MediaStreamInterface* stream)
{
}


void Signaler::onRemoveRemoteStream(wrtc::Peer* conn, webrtc::MediaStreamInterface* stream)
{
    assert(0 && "free streams");
}


void Signaler::onStable(wrtc::Peer* conn)
{
}


void Signaler::onClosed(wrtc::Peer* conn)
{
    wrtc::PeerManager::onClosed(conn);
}


void Signaler::onFailure(wrtc::Peer* conn, const std::string& error)
{
    wrtc::PeerManager::onFailure(conn, error);
}


void Signaler::postMessage(const smpl::Message& m)
{
    _ipc.push(new ipc::Action(
        std::bind(&Signaler::syncMessage, this, std::placeholders::_1),
        m.clone()));
}


void Signaler::syncMessage(const ipc::Action& action)
{
    auto m = reinterpret_cast<smpl::Message*>(action.arg);
    _client.send(*m);
    delete m;
}


} // namespace scy
