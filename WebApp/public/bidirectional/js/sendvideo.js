import Signaling, { WebSocketSignaling } from "../../js/signaling.js";
import * as Config from "../../js/config.js";

export class SendVideo {
  constructor() {
    const _this = this;
    this.config = Config.getRTCConfiguration();
    this.pc = null;
    this.localStream = null;
    this.remoteStram = new MediaStream();
    this.isOffer = false;
    this.connectionId = null;
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

  async setupConnection(remoteVideo, connectionId, useWebSocket) {
    const _this = this;
    this.connectionId = connectionId;
    this.remoteVideo = remoteVideo;
    this.remoteVideo.srcObject = this.remoteStram;

    this.remoteStram.onaddtrack = async (e) => await _this.remoteVideo.play();

    if (useWebSocket) {
      this.signaling = new WebSocketSignaling();
    } else {
      this.signaling = new Signaling();
    }

    this.signaling.addEventListener('connect', async (e) => {
      const data = e.detail;
      _this.prepareNewPeerConnection(data.peerExists, data.connectionId);
      _this.addTracks();
    });

    this.signaling.addEventListener('offer', async (e) => {
      const offer = e.detail;

      if (_this.connectionId != offer.connectionId) {
        return;
      }

      _this.prepareNewPeerConnection(false, offer.connectionId);
      _this.addTracks();

      const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
      await _this.pc.setRemoteDescription(desc);
      let answer = await _this.pc.createAnswer();
      await _this.pc.setLocalDescription(answer);
      _this.signaling.sendAnswer(offer.connectionId, answer.sdp);
    });

    this.signaling.addEventListener('answer', async (e) => {
      const answer = e.detail;

      if (_this.connectionId != answer.connectionId) {
        return;
      }

      const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
      await _this.pc.setRemoteDescription(desc);
    });

    this.signaling.addEventListener('candidate', async (e) => {
      const candidate = e.detail;

      if (_this.connectionId != candidate.connectionId) {
        return;
      }

      const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
      _this.pc.addIceCandidate(iceCandidate);
    });

    await this.signaling.start();
    await this.signaling.createConnection(connectionId);
  }

  prepareNewPeerConnection(isOffer, connectionId) {
    if (this.connectionId != connectionId) {
      return;
    }

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
        _this.signaling.sendCandidate(_this.connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
      }
    };

    this.pc.onnegotiationneeded = async () => {
      if (_this.isOffer) {
        await _this.sendOffer();
      }
    };
  }

  async sendOffer() {
    const _this = this;
    let offer = await _this.pc.createOffer();
    console.log('createOffer() succsess in promise');

    if (_this.pc.signalingState != 'stable') {
      console.error("peerConnection's signaling state is not stable. " + pc.SignalingState);
      return;
    }

    await _this.pc.setLocalDescription(offer);
    console.log('setLocalDescription() succsess in promise');
    _this.signaling.sendOffer(_this.connectionId, offer.sdp);
  }

  addTracks() {
    const _this = this;
    _this.localStream.getTracks().forEach(track => _this.pc.addTrack(track, _this.localStream));
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
