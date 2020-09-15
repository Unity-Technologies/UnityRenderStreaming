import Signaling, { WebSocketSignaling } from "./signaling.js"

export class SendVideo {
    constructor() {
        const _this = this;
        this.config = SendVideo.getConfiguration();
        this.pc = null;
        this.localStream = null;
        this.negotiationneededCounter = 0;
        this.isOffer = false;
    }

    static getConfiguration() {
        let config = {};
        config.sdpSemantics = 'unified-plan';
        config.iceServers = [{ urls: ['stun:stun.l.google.com:19302'] }];
        return config;
    }

    async startVideo(localVideo) {
        try {
            this.localStream = await navigator.mediaDevices.getUserMedia({ video: true, audio: false });
            localVideo.srcObject = this.localStream;
            await localVideo.play();
        } catch (err) {
            console.error('mediaDevice.getUserMedia() error:', err);
        }
    }

    async setupConnection(remoteVideo) {
        const _this = this;
        // close current RTCPeerConnection
        if (this.pc) {
            console.log('Close current PeerConnection');
            this.pc.close();
            this.pc = null;
        }

        const protocolEndPoint = location.protocol + '//' + location.host + location.pathname + 'protocol';
        const createResponse = await fetch(protocolEndPoint);
        const res = await createResponse.json();

        if (res.useWebSocket) {
            this.signaling = new WebSocketSignaling();
        } else {
            this.signaling = new Signaling();
        }

        // Create peerConnection with proxy server and set up handlers
        this.pc = new RTCPeerConnection(this.config);

        this.pc.onsignalingstatechange = e => {
            console.log('signalingState changed:', e);
        };

        this.pc.oniceconnectionstatechange = e => {
            console.log('iceConnectionState changed:', e);
            console.log('pc.iceConnectionState:' + _this.pc.iceConnectionState);
            if (_this.pc.iceConnectionState === 'disconnected') {
                _this.hangUp();
            }
        };

        this.pc.onicegatheringstatechange = e => {
            console.log('iceGatheringState changed:', e);
        };

        this.pc.ontrack = async (e) => {
            if (e.track.kind == 'video') {
                remoteVideo.srcObject = e.streams[0];
                await remoteVideo.play();
            }
            if (e.track.kind == 'audio') {
                _this.localStream.addTrack(e.track);
            }
        };

        this.pc.onicecandidate = e => {
            if (e.candidate != null) {
                _this.signaling.sendCandidate(e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
            }
        };

        this.signaling.addEventListener('offer', async (e) => {
            const offer = e.detail;
            const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
            await _this.pc.setRemoteDescription(desc);
            let answer = await _this.pc.createAnswer();
            await _this.pc.setLocalDescription(answer);
            _this.signaling.sendAnswer(answer.sdp);    
        });

        this.signaling.addEventListener('answer', async (e) => {
            const answer = e.detail;
            const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
            await _this.pc.setRemoteDescription(desc);
        });

        this.signaling.addEventListener('candidate', async (e) => {
            const candidate = e.detail;
            const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
            _this.pc.addIceCandidate(iceCandidate);
        });

        // setup this.signaling
        await this.signaling.start();

        this.localStream.getTracks().forEach(track => _this.pc.addTrack(track, _this.localStream));

        const offer = await this.pc.createOffer();
        // set local sdp
        const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
        await this.pc.setLocalDescription(desc);
        await this.signaling.sendOffer(offer.sdp);
    }

    hangUp() {
        if (this.pc) {
            if (this.pc.iceConnectionState !== 'closed') {
                this.pc.close();
                this.pc = null;
                negotiationneededCounter = 0;
                console.log('sending close message');
                this.signaling.stop();
                return;
            }
        }
        console.log('peerConnection is closed.');
    }
}
