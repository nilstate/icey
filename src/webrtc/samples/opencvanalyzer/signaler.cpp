///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
///


#include "signaler.h"
using std::endl;


namespace scy {
    namespace wrtc {

        Signaler::Signaler(
                const smpl::Client::Options &options,
                std::string *networkBinPath,
                std::string *networkConfigPath,
                float minConfidence,
                ThreadSafeQueue<nlohmann::json> *detectionQueue)
                : _client(options) {
            _client.StateChange += slot(this, &Signaler::onClientStateChange);
            _client.roster().ItemAdded += slot(this, &Signaler::onPeerConnected);
            _client.roster().ItemRemoved += slot(this, &Signaler::onPeerDiconnected);
            _client += packetSlot(this, &Signaler::onPeerMessage);
            _client.connect();

            _networkBinPath = networkBinPath;
            _networkConfigPath = networkConfigPath;
            _minConfidence = minConfidence;
            _detectionQueue = detectionQueue;
        }


        Signaler::~Signaler() {
        }

        ObjectDetector* Signaler::getDetectorPtr() {
            return _detector.get();
        }


        void Signaler::sendSDP(wrtc::Peer *conn, const std::string &type, const std::string &sdp) {
            assert(type == "offer" || type == "answer");
            smpl::Message m;
            json::value desc;
            desc[wrtc::kSessionDescriptionTypeName] = type;
            desc[wrtc::kSessionDescriptionSdpName] = sdp;
            m[type] = desc;

            postMessage(m);
        }
        void Signaler::sendCandidate(wrtc::Peer *conn, const std::string &mid, int mlineindex, const std::string &sdp) {
            smpl::Message m;
            json::value desc;
            desc[wrtc::kCandidateSdpMidName] = mid;
            desc[wrtc::kCandidateSdpMlineIndexName] = mlineindex;
            desc[wrtc::kCandidateSdpName] = sdp;
            m["candidate"] = desc;
            postMessage(m);
        }

        void Signaler::onPeerConnected(smpl::Peer &peer) {
            if (peer.id() == _client.ourID())
                return;
            LDebug("Peer connected: ", peer.id())
            if (wrtc::PeerManager::exists(peer.id())) {
                LDebug("Peer already has session: ", peer.id())
                return;
            }

            auto conn = new wrtc::Peer(this, &_context, peer.id(), "", wrtc::Peer::Answer);
            conn->constraints().SetMandatoryReceiveVideo(true);
            conn->constraints().SetMandatoryReceiveAudio(false);
            conn->createConnection();
            wrtc::PeerManager::add(peer.id(), conn);
        }
        void Signaler::onPeerMessage(smpl::Message &m) {
            LDebug("Peer message: ", m.from().toString())
            if (m.find("offer") != m.end()) {
                recvSDP(m.from().id, m["offer"]);
            } else if (m.find("answer") != m.end()) {
                assert(0 && "answer not supported");
            } else if (m.find("candidate") != m.end()) {
                recvCandidate(m.from().id, m["candidate"]);
            }
            // else assert(0 && "unknown event");
        }
        void Signaler::onPeerDiconnected(const smpl::Peer &peer) {
            LDebug("Peer disconnected")
            auto conn = get(peer.id());
            if (conn) {
                LDebug("Closing peer connection: ", peer.id())
                conn->closeConnection(); // will be deleted via callback
            }
        }

        void Signaler::onClientStateChange(void *sender, sockio::ClientState &state, const sockio::ClientState &oldState) {
            SDebug << "Client state changed from " << oldState << " to " << state
                   << endl;

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

        void Signaler::onAddRemoteStream(wrtc::Peer *conn, webrtc::MediaStreamInterface *stream) {
            // TODO: create multiple detectors in case of multiple streams
             _detector.reset(new ObjectDetector(_networkBinPath,
                                                _networkConfigPath,
                                                _minConfidence,
                                                _detectionQueue));
            auto videoTracks = stream->GetVideoTracks();
            if (!videoTracks.empty())
                _detector->setVideoTrack(videoTracks[0]);
        }

        void Signaler::onRemoveRemoteStream(wrtc::Peer *conn, webrtc::MediaStreamInterface *stream) {
            assert(0 && "free streams");
        }

        void Signaler::onStable(wrtc::Peer *conn) {
        }

        void Signaler::onClosed(wrtc::Peer *conn) {
            _detector.reset(); // shutdown the recorder
            wrtc::PeerManager::onClosed(conn);
        }

        void Signaler::onFailure(wrtc::Peer *conn, const std::string &error) {
            _detector.reset(); // shutdown the recorder
            wrtc::PeerManager::onFailure(conn, error);
        }

        void Signaler::postMessage(const smpl::Message &m) {
            _ipc.push(new ipc::Action(
                    std::bind(&Signaler::syncMessage, this, std::placeholders::_1),
                    m.clone()));
        }

        void Signaler::syncMessage(const ipc::Action &action) {
            auto m = reinterpret_cast<smpl::Message *>(action.arg);
            _client.send(*m);
            delete m;
        }
    }

} // namespace scy
