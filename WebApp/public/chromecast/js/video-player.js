import Signaling, { WebSocketSignaling } from "../../js/signaling.js";
import * as Config from "../../js/config.js";
import * as Logger from "./logger.js";
import uuid4 from 'https://cdn.jsdelivr.net/gh/tracker1/node-uuid4/browser.mjs';


export class VideoPlayer {
  constructor(elements) {
    const _this = this;
    this.cfg = Config.getRTCConfiguration();
    this.pc = null;
    this.channel = null;
    this.offerOptions = {
      offerToReceiveAudio: true,
      offerToReceiveVideo: true,
    };
    this.connectionId = null;

    // main video
    this.localStream = new MediaStream();
    Logger.log("elements[0] " + elements[0]);
    this.video = elements[0];
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', function () {
      _this.video.play();
      _this.resizeVideo();
    }, true);


    this.videoTrackList = [];
    this.videoTrackIndex = 0;
    this.maxVideoTrackLength = 1;

    this.ondisconnect = function () { };
  }

  async setupConnection(useWebSocket) {
    const _this = this;
    // close current RTCPeerConnection
    if (this.pc) {
      Logger.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }

    if (useWebSocket) {
      this.signaling = new WebSocketSignaling();
    } else {
      this.signaling = new Signaling();
    }

    // Create peerConnection with proxy server and set up handlers
    this.pc = new RTCPeerConnection(this.cfg);
    this.pc.onsignalingstatechange = function (e) {
      Logger.log('signalingState changed:', e);
    };
    this.pc.oniceconnectionstatechange = function (e) {
      Logger.log('iceConnectionState changed:', e);
      Logger.log('pc.iceConnectionState:' + _this.pc.iceConnectionState);
      if (_this.pc.iceConnectionState === 'disconnected') {
        _this.ondisconnect();
      }
    };
    this.pc.onicegatheringstatechange = function (e) {
      Logger.log('iceGatheringState changed:', e);
    };
    this.pc.ontrack = function (e) {
      if (e.track.kind == 'video') {
        Logger.log('video ontrack: ' + e.track);
        _this.videoTrackList.push(e.track);
      }
      if (e.track.kind == 'audio') {
        Logger.log('audio ontrack: ' + e.track);
        _this.localStream.addTrack(e.track);
      }
      if (_this.videoTrackList.length == _this.maxVideoTrackLength) {
        Logger.log('video: ' + _this.video);
        _this.video.srcObject = _this.localStream;
        _this.replaceTrack(_this.localStream, _this.videoTrackList[0]);
      }
    };
    this.pc.onicecandidate = function (e) {
      if (e.candidate != null) {
        _this.signaling.sendCandidate(_this.connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
      }
    };

    this.signaling.addEventListener('answer', async (e) => {
      const answer = e.detail;
      const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
      await _this.pc.setRemoteDescription(desc);
    });

    this.signaling.addEventListener('candidate', async (e) => {
      const candidate = e.detail;
      const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
      await _this.pc.addIceCandidate(iceCandidate);
    });

    // setup signaling
    await this.signaling.start();
    this.connectionId = uuid4();

    // Add transceivers to receive multi stream.
    // This operation is required to generate offer SDP correctly.
    this.pc.addTransceiver('video', { direction: 'recvonly' });
    this.pc.addTransceiver('audio', { direction: 'recvonly' });

    // create offer
    const offer = await this.pc.createOffer(this.offerOptions);

    // set local sdp
    const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
    await this.pc.setLocalDescription(desc);
    await this.signaling.sendOffer(this.connectionId, offer.sdp);
  };

  resizeVideo() {
    const clientRect = this.video.getBoundingClientRect();
    const videoRatio = this.videoWidth / this.videoHeight;
    const clientRatio = clientRect.width / clientRect.height;

    this._videoScale = videoRatio > clientRatio ? clientRect.width / this.videoWidth : clientRect.height / this.videoHeight;
    const videoOffsetX = videoRatio > clientRatio ? 0 : (clientRect.width - this.videoWidth * this._videoScale) * 0.5;
    const videoOffsetY = videoRatio > clientRatio ? (clientRect.height - this.videoHeight * this._videoScale) * 0.5 : 0;
    this._videoOriginX = clientRect.left + videoOffsetX;
    this._videoOriginY = clientRect.top + videoOffsetY;
  }


  // replace video track related the MediaStream
  replaceTrack(stream, newTrack) {
    const tracks = stream.getVideoTracks();
    for (const track of tracks) {
      if (track.kind == 'video') {
        stream.removeTrack(track);
      }
    }
    stream.addTrack(newTrack);
  }

  get videoWidth() {
    return this.video.videoWidth;
  }

  get videoHeight() {
    return this.video.videoHeight;
  }

  get videoOriginX() {
    return this._videoOriginX;
  }

  get videoOriginY() {
    return this._videoOriginY;
  }

  get videoScale() {
    return this._videoScale;
  }

  close() {
    if (this.pc) {
      Logger.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }
  };

  sendMsg(msg) {
    if (this.channel == null) {
      return;
    }
    switch (this.channel.readyState) {
      case 'connecting':
        Logger.log('Connection not ready');
        break;
      case 'open':
        this.channel.send(msg);
        break;
      case 'closing':
        Logger.log('Attempt to sendMsg message while closing');
        break;
      case 'closed':
        Logger.log('Attempt to sendMsg message while connection closed.');
        break;
    }
  };
}
