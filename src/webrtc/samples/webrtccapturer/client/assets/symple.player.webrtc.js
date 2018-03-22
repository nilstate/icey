// -----------------------------------------------------------------------------
// WebRTC Engine
//

window.RTCPeerConnection = window.mozRTCPeerConnection || window.webkitRTCPeerConnection;
window.RTCSessionDescription = window.mozRTCSessionDescription || window.RTCSessionDescription;
window.RTCIceCandidate = window.mozRTCIceCandidate || window.RTCIceCandidate;
window.URL = window.webkitURL || window.URL;
navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia;


Symple.Media.registerEngine({
    id: 'WebRTC',
    name: 'WebRTC Player',
    formats: 'VP9, VP4, H.264, Opus',
    preference: 100,
    support: (function() {
        return typeof RTCPeerConnection != 'undefined';
    })()
});


Symple.Player.Engine.WebRTC = Symple.Player.Engine.extend({
    init: function(player) {
        Symple.log('symple:webrtc: init');
        this._super(player);

        this.rtcConfig = player.options.rtcConfig || {
          iceServers: [
            { url: 'stun:stun.l.google.com:19302' }
          ]
        }

        this.rtcOptions = player.options.rtcOptions || {
            optional: [
                { DtlsSrtpKeyAgreement: true } // required for FF <=> Chrome interop
            ]
        }

        // Specifies that this client will be the ICE initiator,
        // and will be sending the initial SDP Offer.
        this.initiator = player.options.initiator;

        // The `MediaStreamConstraints` object to pass to `getUserMedia`
        this.userMediaConstraints = player.options.userMediaConstraints || { 
            audio: true, 
            video: true
        }

        // Reference to the active local or remote media stream
        this.activeStream = null;
    },

    setup: function() {
        Symple.log('symple:webrtc: setup');

        this._createPeerConnection();

        if (typeof(this.video) == 'undefined') {
            this.video = document.createElement('video');
            this.video.autoplay = true;
            this.player.screen.prepend(this.video);
        }
    },

    destroy: function() {
        Symple.log('symple:webrtc: destroy');

        this.sendLocalSDP = null;
        this.sendLocalCandidate = null;
        this.activeStream = null; // TODO: Needs explicit close?

        if (this.video) {
            this.video.src = '';
            this.video = null;
            // Anything else required for video cleanup?
        }

        if (this.pc) {
            this.pc.close();
            this.pc = null;
            // Anything else required for peer connection cleanup?
        }
    },

    play: function(params) {
        Symple.log('symple:webrtc: play', params);

        // If there is an active stream then play it now.
        if (this.activeStream) {
            this.video.src = URL.createObjectURL(this.activeStream);
            this.video.play();
            this.setState('playing');
        }

        // Otherwise wait until ICE to complete before setting the 'playing' state.
        else {

            // If we are the ICE `initiator` then attempt to open the local video 
            // device and send the SDP Offer to the peer.
            if (this.initiator) {
                Symple.log('symple:webrtc: initiating', this.userMediaConstraints);
                var self = this;

                // TODO: Support device enumeration.
                navigator.getUserMedia(this.userMediaConstraints,
                    function (localStream) { // success

                        // Play the local video stream and create the SDP offer.
                        self.video.src = URL.createObjectURL(localStream);
                        self.pc.addStream(localStream);
                        self.pc.createOffer(
                            function(desc) { // success
                                Symple.log('symple:webrtc: offer', desc);
                                self._onLocalSDP(desc); 
                            },
                            function(err) { // error
                                Symple.log('symple:webrtc: offer failed', err);
                            });

                        // Store the active local stream
                        self.activeStream = localStream;
                    },
                    function(err) { // error
                        self.setError('getUserMedia() failed: ' + err);
                    });
            }
        }
    },

    stop: function() {
        // NOTE: Stopping the player does not close the peer connection,
        // only `destroy` does that. This enables us to resume playback 
        // quickly and with minimal delay.


        if (this.video) {
            this.video.src = '';
            // Do not nullify
        }

        // Close peer connection
        // if (this.pc) {
        //     this.pc.close();
        //     this.pc = null;
        // }

        this.setState('stopped');
    },

    mute: function(flag) {
        // Mute unless explicit false given
        flag = flag === false ? false : true;

        Symple.log('symple:webrtc: mute', flag);

        if (this.video)
            this.video.prop('muted', flag);
    },

    // Called when local SDP is ready to be sent to the peer.
    sendLocalSDP: null, // new Function,

    // Called when a local candidate is ready to be sent to the peer.
    sendLocalCandidate: null, // new Function,

    // Called when remote SDP is received from the peer.
    recvRemoteSDP: function(desc) {
        Symple.log('symple:webrtc: recv remote sdp', desc)
        if (!desc || !desc.type || !desc.sdp)
            throw 'Invalid remote SDP';

        var self = this;
        this.pc.setRemoteDescription(new RTCSessionDescription(desc),
            function() {
                Symple.log('symple:webrtc: sdp success');
            },
            function(message) {
                console.error('symple:webrtc: sdp error', message);
                self.setError('Cannot parse remote SDP offer');
            }
        );

        if (desc.type == 'offer') {
            self.pc.createAnswer(
                function(answer) { // success
                    self._onLocalSDP(answer);
                },
                function() { // error
                    self.setError('Cannot create local SDP answer');
                },
                null // this.mediaConstraints
            );
        }
    },

    // Called when remote candidate is received from the peer.
    recvRemoteCandidate: function(candidate) {
        Symple.log('symple:webrtc: recv remote candiate', candidate);
        if (!this.pc)
            throw 'The peer connection is not initialized'; // call recvRemoteSDP first

        this.pc.addIceCandidate(new RTCIceCandidate(candidate));
    },

    //
    // Private methods
    //

    // Called when local SDP is ready to be sent to the peer.
    _onLocalSDP: function(desc) {
        try {
            this.pc.setLocalDescription(desc);
            this.sendLocalSDP(desc);
        }
        catch (e) {
            Symple.log('symple:webrtc: failed to send local SDP', e);
        }
    },

    // Create the RTCPeerConnection object.
    _createPeerConnection: function() {
        if (this.pc)
            throw 'The peer connection is already initialized';

        Symple.log('symple:webrtc: create peer connnection', this.rtcConfig, this.rtcOptions);

        var self = this;
        this.pc = new RTCPeerConnection(this.rtcConfig, this.rtcOptions);
        this.pc.onicecandidate = function(event) {
            if (event.candidate) {
                Symple.log('symple:webrtc: candidate gathered', event.candidate);
                self.sendLocalCandidate(event.candidate);
            }
            else {
                Symple.log('symple:webrtc: candidate gathering complete');
            }
        };
        this.pc.onaddstream = function(event) {
            Symple.log('symple:webrtc: remote stream added', URL.createObjectURL(event.stream));

            // Set the state to playing once candidates have completed gathering.
            // This is the best we can do until ICE onstatechange is implemented.
            self.setState('playing');

            self.video.src = URL.createObjectURL(event.stream);
            self.video.play();

            // Store the active stream
            self.activeStream = event.stream;
        };
        this.pc.onremovestream = function(event) {
            Symple.log('symple:webrtc: remote stream removed', event);
            self.video.stop();
            self.video.src = '';
        };

        // NOTE: The following state events are still very unreliable.
        // Hopefully when the spec is complete this will change, but until then
        // we need to 'guess' the state.
        // this.pc.onconnecting = function(event) { Symple.log('symple:webrtc: onconnecting:', event); };
        // this.pc.onopen = function(event) { Symple.log('symple:webrtc: onopen:', event); };
        // this.pc.onicechange = function(event) { Symple.log('symple:webrtc: onicechange :', event); };
        // this.pc.onstatechange = function(event) { Symple.log('symple:webrtc: onstatechange :', event); };
    }
});


//
// Helpers

Symple.Media.iceCandidateType = function(candidateSDP) {
    if (candidateSDP.indexOf('typ relay') != -1)
        return 'turn';
    if (candidateSDP.indexOf('typ srflx') != -1)
        return 'stun';
    if (candidateSDP.indexOf('typ host') != -1)
        return 'host';
    return 'unknown';
}
