///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "scy/webrtc/peer.h"
#include "scy/webrtc/peermanager.h"
#include "scy/webrtc/peerfactorycontext.h"
#include "scy/logger.h"


using std::endl;


namespace scy {
namespace wrtc {


Peer::Peer(PeerManager* manager,
           PeerFactoryContext* context,
           const std::string& peerid,
           const std::string& token,
           Mode mode)
    : _manager(manager)
    , _context(context)
    , _peerid(peerid)
    , _token(token)
    , _mode(mode)
    //, _context->factory(manager->factory())
    , _peerConnection(nullptr)
    , _stream(nullptr)
{
    // webrtc::PeerConnectionInterface::IceServer stun;
    // stun.uri = kGoogleStunServerUri;
    // _config.servers.push_back(stun);

    // _constraints.SetMandatoryReceiveAudio(true);
    // _constraints.SetMandatoryReceiveVideo(true);
    // _constraints.SetAllowDtlsSctpDataChannels();
}


Peer::~Peer()
{
    LDebug(_peerid, ": Destroying")
    // closeConnection();

    if (_peerConnection) {
        _peerConnection->Close();
    }
}


rtc::scoped_refptr<webrtc::MediaStreamInterface> Peer::createMediaStream()
{
    assert(_mode == Offer);
    //assert(_context->factory);
    assert(!_stream);
    _stream = _context->factory->CreateLocalMediaStream(kStreamLabel);
    return _stream;
}


void Peer::setPortRange(int minPort, int maxPort)
{
    assert(!_peerConnection);

    if (!_context->networkManager) {
        throw std::runtime_error("Must initialize custom network manager to set port range");
    }

    if (!_context->socketFactory) {
        throw std::runtime_error("Must initialize custom socket factory to set port range");
    }

    if (!_portAllocator)
        _portAllocator.reset(new cricket::BasicPortAllocator(
            _context->networkManager.get(),
            _context->socketFactory.get()));
    _portAllocator->SetPortRange(minPort, maxPort);
}


void Peer::createConnection()
{
    assert(_context->factory);
    _peerConnection = _context->factory->CreatePeerConnection(_config, &_constraints,
                                                              std::move(_portAllocator), nullptr, this);

    if (_stream) {
        if (!_peerConnection->AddStream(_stream)) {
            throw std::runtime_error("Adding stream to Peer failed");
        }
    }
}


void Peer::closeConnection()
{
    LDebug(_peerid, ": Closing")

    if (_peerConnection) {
        _peerConnection->Close();
    }
    else {
        // Call onClosed if no connection has been
        // made so callbacks are always run.
        _manager->onClosed(this);
    }
}


void Peer::createOffer()
{
    assert(_mode == Offer);
    assert(_peerConnection);

    _peerConnection->CreateOffer(this, &_constraints);
}


void Peer::recvSDP(const std::string& type, const std::string& sdp)
{
    LDebug(_peerid, ": Receive ", type, ": ", sdp)

    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* desc(
        webrtc::CreateSessionDescription(type, sdp, &error));
    if (!desc) {
        throw std::runtime_error("Can't parse remote SDP: " + error.description);
    }
    _peerConnection->SetRemoteDescription(
        DummySetSessionDescriptionObserver::Create(), desc);

    if (type == "offer") {
        assert(_mode == Answer);
        _peerConnection->CreateAnswer(this, &_constraints);
    } else {
        assert(_mode == Offer);
    }
}


void Peer::recvCandidate(const std::string& mid, int mlineindex,
                                   const std::string& sdp)
{
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> candidate(
        webrtc::CreateIceCandidate(mid, mlineindex, sdp, &error));
    if (!candidate) {
        throw std::runtime_error("Can't parse remote candidate: " + error.description);
    }
    _peerConnection->AddIceCandidate(candidate.get());
}


void Peer::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    LDebug(_peerid, ": On signaling state change: ", new_state)

    switch (new_state) {
        case webrtc::PeerConnectionInterface::kStable:
            _manager->onStable(this);
            break;
        case webrtc::PeerConnectionInterface::kClosed:
            _manager->onClosed(this);
            break;
        case webrtc::PeerConnectionInterface::kHaveLocalOffer:
        case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
        case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
        case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
            break;
    }
}


void Peer::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    LDebug(_peerid, ": On ICE connection change: ", new_state)
}


void Peer::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    LDebug(_peerid, ": On ICE gathering change: ", new_state)
}


void Peer::OnRenegotiationNeeded()
{
    LDebug(_peerid, ": On renegotiation needed")
}


void Peer::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    // proxy to deprecated OnAddStream method
    OnAddStream(stream.get());
}


void Peer::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    // proxy to deprecated OnRemoveStream method
    OnRemoveStream(stream.get());
}


void Peer::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> stream)
{
    assert(0 && "virtual");
}


void Peer::OnAddStream(webrtc::MediaStreamInterface* stream)
{
    assert(_mode == Answer);

    LDebug(_peerid, ": On add stream")
    _manager->onAddRemoteStream(this, stream);
}


void Peer::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
    assert(_mode == Answer);

    LDebug(_peerid, ": On remove stream")
    _manager->onRemoveRemoteStream(this, stream);
}


void Peer::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    std::string sdp;
    if (!candidate->ToString(&sdp)) {
        LError(_peerid, ": Failed to serialize candidate")
        assert(0);
        return;
    }

    _manager->sendCandidate(this, candidate->sdp_mid(),
                            candidate->sdp_mline_index(), sdp);
}


void Peer::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    LDebug(_peerid, ": Set local description")
    _peerConnection->SetLocalDescription(
        DummySetSessionDescriptionObserver::Create(), desc);

    // Send an SDP offer to the peer
    std::string sdp;
    if (!desc->ToString(&sdp)) {
        LError(_peerid, ": Failed to serialize local sdp")
        assert(0);
        return;
    }

    _manager->sendSDP(this, desc->type(), sdp);
}


void Peer::OnFailure(const std::string& error)
{
    LError(_peerid, ": On failure: ", error)

    _manager->onFailure(this, error);
}


void Peer::setPeerFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory)
{
    assert(!_context->factory); // should not be already set via PeerManager
    _context->factory = factory;
}


std::string Peer::peerid() const
{
    return _peerid;
}


std::string Peer::token() const
{
    return _token;
}


webrtc::FakeConstraints& Peer::constraints()
{
    return _constraints;
}


webrtc::PeerConnectionFactoryInterface* Peer::factory() const
{
    return _context->factory.get();
}


rtc::scoped_refptr<webrtc::PeerConnectionInterface> Peer::peerConnection() const
{
    return _peerConnection;
}


rtc::scoped_refptr<webrtc::MediaStreamInterface> Peer::stream() const
{
    return _stream;
}


//
// Dummy Set Peer Description Observer
//


void DummySetSessionDescriptionObserver::OnSuccess()
{
    LDebug("On SDP parse success")
}


void DummySetSessionDescriptionObserver::OnFailure(const std::string& error)
{
    LError("On SDP parse error: ", error)
    assert(0);
}


} } // namespace scy::wrtc


/// @\}
