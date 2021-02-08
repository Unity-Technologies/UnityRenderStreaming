import Signaling, { WebSocketSignaling } from "../../scripts/signaling.js"

export class SendVideo {
  constructor() {
    const _this = this;
    this.config = SendVideo.getConfiguration();
    this.pc = null;
    this.localStream = null;
    this.remoteStram = new MediaStream();
    this.isOffer = false;
    this.connectionId = null;
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

  async setupConnection(remoteVideo, connectionId) {
    const _this = this;
    this.connectionId = connectionId;
    this.remoteVideo = remoteVideo;
    this.remoteVideo.srcObject = this.remoteStram;

    this.remoteStram.onaddtrack = async (e) => await _this.remoteVideo.play();

    const protocolEndPoint = location.protocol + '//' + location.host + location.pathname + 'protocol';
    const createResponse = await fetch(protocolEndPoint);
    const res = await createResponse.json();

    if (res.useWebSocket) {
      this.signaling = new WebSocketSignaling();
    } else {
      this.signaling = new Signaling();
    }

    this.signaling.addEventListener('connect', async (e) => {
      const data = e.detail;
      _this.prepareNewPeerConnection(data.peerExists, data.connectionId);

      if (data.peerExists) {
        _this.localStream.getTracks().forEach(track => _this.pc.addTrack(track, _this.localStream));
      }
    });

    this.signaling.addEventListener('offer', async (e) => {
      if (_this.pc) {
        console.error('peerConnection alreay exist');
      }
      const offer = e.detail;
      _this.prepareNewPeerConnection(false, offer.connectionId);
      const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
      await _this.pc.setRemoteDescription(desc);
      let answer = await _this.pc.createAnswer();
      await _this.pc.setLocalDescription(answer);
      _this.signaling.sendAnswer(_this.connectionId, answer.sdp);
    });

    this.signaling.addEventListener('answer', async (e) => {
      if (!_this.pc) {
        console.error('peerConnection NOT exist!');
        return;
      }
      const answer = e.detail;
      const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
      await _this.pc.setRemoteDescription(desc);
    });

    this.signaling.addEventListener('candidate', async (e) => {
      const candidate = e.detail;
      const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
      _this.pc.addIceCandidate(iceCandidate);
    });

    await this.signaling.start();
    await this.signaling.createConnection(this.connectionId);
  }

  prepareNewPeerConnection(isOffer, connectionId) {
    const _this = this;
    this.isOffer = isOffer;
    // close current RTCPeerConnection
    if (this.pc) {
      console.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
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
      _this.remoteStram.addTrack(e.track);
    };

    this.pc.onicecandidate = e => {
      if (e.candidate != null) {
        _this.signaling.sendCandidate(connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
      }
    };

    this.pc.onnegotiationneeded = async () => {
      if (_this.isOffer) {
        let offer = await _this.pc.createOffer();
        console.log('createOffer() succsess in promise');
        await _this.pc.setLocalDescription(offer);
        console.log('setLocalDescription() succsess in promise');
        _this.signaling.sendOffer(_this.connectionId, offer.sdp);
      }
    };
  }

  async hangUp() {
    if (this.pc) {
      if (this.pc.iceConnectionState !== 'closed') {
        this.pc.close();
        this.pc = null;
        console.log('sending close message');
        this.signaling.stop();
        return;
      }
    }
    console.log('peerConnection is closed.');
    await this.signaling.deleteConnection(this.connectionId);
    this.connectionId = null;
  }
}
