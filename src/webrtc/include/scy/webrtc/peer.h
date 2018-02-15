///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_Peer_H
#define SCY_WebRTC_Peer_H


#include "scy/webrtc/peerfactorycontext.h"

#include "webrtc/api/jsep.h"
#include "webrtc/api/peerconnectioninterface.h"
#include "webrtc/api/test/fakeconstraints.h"
#include "webrtc/p2p/client/basicportallocator.h"


namespace scy {
namespace wrtc {


class PeerManager;


class Peer : public webrtc::PeerConnectionObserver,
             public webrtc::CreateSessionDescriptionObserver
{
public:
    enum Mode
    {
        Offer, ///< Operating as offerer
        Answer ///< Operating as answerer
    };

    Peer(PeerManager* manager,
         PeerFactoryContext* context,
         const std::string& peerid,
         const std::string& token,
         Mode mode);
    virtual ~Peer();

    /// Create the local media stream.
    /// Only necessary when we are creating the offer.
    virtual rtc::scoped_refptr<webrtc::MediaStreamInterface> createMediaStream();

    /// Create the peer connection once configuration, constraints and
    /// streams have been created.
    virtual void createConnection();

    /// Close the peer connection.
    virtual void closeConnection();

    /// Create the offer SDP tos end to the peer.
    /// No offer should be received after creating the offer.
    /// A call to `recvSDP` with answer is expected in order to initiate the session.
    virtual void createOffer();

    /// Receive a remote offer or answer.
    virtual void recvSDP(const std::string& type, const std::string& sdp);

    /// Receive a remote candidate.
    virtual void recvCandidate(const std::string& mid, int mlineindex, const std::string& sdp);

    /// Set a custom PeerFactory object.
    /// Must be set before any streams are initiated.
    void setPeerFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory);

    /// Set the port range for WebRTC connections.
    /// Must be done before the connection is initiated.
    void setPortRange(int minPort, int maxPort);

    std::string peerid() const;
    std::string token() const;
    webrtc::FakeConstraints& constraints();
    webrtc::PeerConnectionFactoryInterface* factory() const;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection() const;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream() const;

protected:
    /// inherited from PeerConnectionObserver
    virtual void OnAddStream(webrtc::MediaStreamInterface* stream); ///< @deprecated
    virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream); ///< @deprecated
    virtual void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override; ///< since 7f0676
    virtual void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override; ///< since 7f0676
    virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> stream) override; ///< since 7f0676
    virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
    virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
    virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    virtual void OnRenegotiationNeeded();

    /// inherited from CreateSessionDescriptionObserver
    virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    virtual void OnFailure(const std::string& error) override;

    virtual int AddRef() const override { return 1; }
    virtual int Release() const override { return 0; }

protected:
    PeerManager* _manager;
    PeerFactoryContext* _context;
    std::string _peerid;
    std::string _token;
    Mode _mode;
    webrtc::PeerConnectionInterface::RTCConfiguration _config;
    webrtc::FakeConstraints _constraints;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> _stream;
    std::unique_ptr<cricket::BasicPortAllocator> _portAllocator;
};


class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver
{
public:
    static DummySetSessionDescriptionObserver* Create()
    {
        return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
    }

    virtual void OnSuccess() override;
    virtual void OnFailure(const std::string& error) override;

protected:
    DummySetSessionDescriptionObserver() = default;
    ~DummySetSessionDescriptionObserver() = default;
};


} } // namespace scy::wrtc


#endif


/// @\}
