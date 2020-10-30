import Signaling, { WebSocketSignaling } from "./signaling.js"

export class SendVideo {
    constructor() {
        const _this = this;
        this.config = SendVideo.getConfiguration();
        this.localStream = null;
        this.remoteStream = null;
        this.isOffer = false;
        this.connectionId = null;
        this.connectionIdAndPeer = new Map();
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
        this.remoteVideo = remoteVideo;
        this.remoteStream = new MediaStream();
        this.remoteVideo.srcObject = this.remoteStream;

        this.remoteStream.onaddtrack = async (e) => await _this.remoteVideo.play();

        const protocolEndPoint = location.protocol + '//' + location.host + location.pathname + 'protocol';
        const createResponse = await fetch(protocolEndPoint);
        const res = await createResponse.json();

        if (res.useWebSocket) {
            this.signaling = new WebSocketSignaling();
        } else {
            this.signaling = new Signaling();
        }

        this.signaling.addEventListener('connect', async(e) => {
            _this.connectionId = e.detail;
            _this.createPeerConnection(_this.connectionId, true);
        });

        this.signaling.addEventListener('offer', async (e) => {
            const offer = e.detail;

            if (_this.connectionIdAndPeer.has(offer.connectionId)) {
                console.error('peerConnection aleady exsist:', offer.connectionId);
                return;
            }
            const pc = _this.createPeerConnection(offer.connectionId, false);
            const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
            await pc.setRemoteDescription(desc);

            this.localStream.getTracks().forEach(track => pc.addTrack(track, _this.localStream));

            let answer = await pc.createAnswer();
            await pc.setLocalDescription(answer);
            _this.signaling.sendAnswer(offer.connectionId, answer.sdp);
        });

        this.signaling.addEventListener('answer', async (e) => {
            const answer = e.detail;

            if (!_this.connectionIdAndPeer.has(answer.connectionId)) {
                console.error('peerConnection not exsist', answer.connectionId);
                return;
            }

            const pc = _this.connectionIdAndPeer.get(answer.connectionId);
            const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
            await pc.setRemoteDescription(desc);
        });

        this.signaling.addEventListener('candidate', async (e) => {
            const candidate = e.detail;

            if (!_this.connectionIdAndPeer.has(candidate.connectionId)) {
                console.error('peerConnection not exsist');
                return;
            }

            const pc = _this.connectionIdAndPeer.get(candidate.connectionId);
            const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
            pc.addIceCandidate(iceCandidate);
        });

        await this.signaling.start();
    }

    async call() {
        const _this = this;

        if (!this.connectionIdAndPeer.has(this.connectionId)) {
            console.error("peer connection not prepared")
            return;
        }

        const pc = this.connectionIdAndPeer.get(this.connectionId);
        this.localStream.getTracks().forEach(track => pc.addTrack(track, _this.localStream));
    }

    createPeerConnection(connectionId, isOffer) {
        const _this = this;
        this.isOffer = isOffer;

        if(this.connectionIdAndPeer.has(connectionId)){
            const oldPc = this.connectionIdAndPeer.get(connectionId);
            oldPc.close();
            this.connectionIdAndPeer.delete(connectionId);
        }

        // Create peerConnection with proxy server and set up handlers
        const pc = new RTCPeerConnection(this.config);
        this.connectionIdAndPeer.set(connectionId, pc);

        pc.onsignalingstatechange = e => {
            console.log('signalingState changed:', e);
        };

        pc.oniceconnectionstatechange = e => {
            console.log('iceConnectionState changed:', e);
            console.log('pc.iceConnectionState:' + pc.iceConnectionState);
            if (pc.iceConnectionState === 'disconnected') {
                pc.close();
                _this.connectionIdAndPeer.delete(connectionId);
            }
        };

        pc.onicegatheringstatechange = e => {
            console.log('iceGatheringState changed:', e);
        };

        pc.ontrack = async (e) => {
            _this.remoteStream.addTrack(e.track);
        };

        pc.onicecandidate = e => {
            if (e.candidate != null) {
                _this.signaling.sendCandidate(connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
            }
        };

        pc.onnegotiationneeded = async () => {

            if(!_this.isOffer)
            {
                return;
            }

            let offer = await pc.createOffer();
            console.log('createOffer() succsess in promise');

            if(pc.signalingState != "stable")
            {
                console.error("peerconnection's signaling state is not stable.")
                return;
            }

            await pc.setLocalDescription(offer);
            console.log('setLocalDescription() succsess in promise');
            _this.signaling.sendOffer(connectionId, offer.sdp);
        };

        return pc;
    }

    hangUp() {

        this.localStream.getTracks().forEach(track => track.stop());
        this.remoteStream.getTracks().forEach(track => track.stop());

        for(let pc in this.connectionIdAndPeer.values()) {
            if (pc.iceConnectionState !== 'closed') {
                pc.close();
            }
        }
        this.signaling.stop();
        this.connectionIdAndPeer.clear();
        console.log('peerConnection is closed.');
    }
}
