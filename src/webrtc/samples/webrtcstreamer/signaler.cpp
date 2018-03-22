///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
///


#include <iostream>
#include <string>

#include "scy/webrtc/audiopacketmodule.h"
#include "scy/webrtc/multiplexmediacapturer.h"
#include "scy/webrtc/videopacketsource.h"
#include "signaler.h"

#include "api/mediastreamtrackproxy.h"


using std::endl;


namespace scy {


Signaler::Signaler(const smpl::Client::Options& options)
    : _client(options)
    , _capturer()
    , _context(_capturer.getAudioModule())
{
    // Setup the signalling client
    _client.StateChange += slot(this, &Signaler::onClientStateChange);
    _client.roster().ItemAdded += slot(this, &Signaler::onPeerConnected);
    _client.roster().ItemRemoved += slot(this, &Signaler::onPeerDiconnected);
    _client += packetSlot(this, &Signaler::onPeerMessage);
    _client.connect();
}


Signaler::~Signaler()
{
}


void Signaler::startStreaming(const std::string& file, bool looping)
{
    // Open the video file
    _capturer.openFile(file, looping);
    // _capturer.start();
}


void Signaler::sendSDP(wrtc::Peer* conn, const std::string& type,
                       const std::string& sdp)
{
    assert(type == "offer" || type == "answer");
    smpl::Message m;
    json::value desc;
    desc[wrtc::kSessionDescriptionTypeName] = type;
    desc[wrtc::kSessionDescriptionSdpName] = sdp;
    m[type] = desc;

    // smpl::Message m({ type, {
    //     { wrtc::kSessionDescriptionTypeName, type },
    //     { wrtc::kSessionDescriptionSdpName, sdp} }
    // });

    postMessage(m);
}


void Signaler::sendCandidate(wrtc::Peer* conn, const std::string& mid,
                             int mlineindex, const std::string& sdp)
{
    smpl::Message m;
    json::value desc;
    desc[wrtc::kCandidateSdpMidName] = mid;
    desc[wrtc::kCandidateSdpMlineIndexName] = mlineindex;
    desc[wrtc::kCandidateSdpName] = sdp;
    m["candidate"] = desc;

    // smpl::Message m({ "candidate", {
    //     { wrtc::kCandidateSdpMidName, mid },
    //     { wrtc::kCandidateSdpMlineIndexName, mlineindex},
    //     { wrtc::kCandidateSdpName, sdp} }
    // });

    postMessage(m);
}


void Signaler::onPeerConnected(smpl::Peer& peer)
{
    if (peer.id() == _client.ourID())
        return;
    LDebug("Peer connected: ", peer.id())

    if (wrtc::PeerManager::exists(peer.id())) {
        LDebug("Peer already has session: ", peer.id())
        return;
    }

    // Create the Peer Peer
    auto conn = new wrtc::Peer(this, &_context, peer.id(), "", wrtc::Peer::Offer);
    conn->constraints().SetMandatoryReceiveAudio(false);
    conn->constraints().SetMandatoryReceiveVideo(false);
    conn->constraints().SetAllowDtlsSctpDataChannels();

    // Create the media stream and attach decoder
    // output to the peer connection
    _capturer.addMediaTracks(_context.factory, conn->createMediaStream());

    // Send the Offer SDP
    conn->createConnection();
    conn->createOffer();

    wrtc::PeerManager::add(peer.id(), conn);
}


void Signaler::onPeerMessage(smpl::Message& m)
{
    LDebug("Peer message: ", m.from().toString())

    if (m.find("offer") != m.end()) {
        assert(0 && "offer not supported");
    } else if (m.find("answer") != m.end()) {
        recvSDP(m.from().id, m["answer"]);
    } else if (m.find("candidate") != m.end()) {
        recvCandidate(m.from().id, m["candidate"]);
    }
    // else assert(0 && "unknown event");
}


void Signaler::onPeerDiconnected(const smpl::Peer& peer)
{
    LDebug("Peer disconnected")

    auto conn = wrtc::PeerManager::remove(peer.id());
    if (conn) {
        LDebug("Deleting peer connection: ", peer.id())
        // async delete not essential, but to be safe
        // delete conn;
        deleteLater<wrtc::Peer>(conn);
    }
}


void Signaler::onClientStateChange(void*, sockio::ClientState& state, const sockio::ClientState& oldState)
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
    assert(0 && "not required");
}


void Signaler::onRemoveRemoteStream(wrtc::Peer* conn, webrtc::MediaStreamInterface* stream)
{
    assert(0 && "not required");
}


void Signaler::onStable(wrtc::Peer* conn)
{
    _capturer.start();
}


void Signaler::onClosed(wrtc::Peer* conn)
{
    _capturer.stop();
    wrtc::PeerManager::onClosed(conn);
}


void Signaler::onFailure(wrtc::Peer* conn, const std::string& error)
{
    _capturer.stop();
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
