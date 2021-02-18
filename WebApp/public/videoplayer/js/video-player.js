import Signaling, { WebSocketSignaling } from "../../js/signaling.js";
import * as Config from "../../js/config.js";
import uuid4 from 'https://cdn.jsdelivr.net/gh/tracker1/node-uuid4/browser.mjs';

// enum type of event sending from Unity
var UnityEventType = {
  SWITCH_VIDEO: 0
};

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
    this.video = elements[0];
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', function () {
      _this.video.play();
      _this.resizeVideo();
    }, true);

    // secondly video
    this.localStream2 = new MediaStream();
    this.videoThumb = elements[1];
    this.videoThumb.playsInline = true;
    this.videoThumb.addEventListener('loadedmetadata', function () {
      _this.videoThumb.play();
    }, true);

    this.videoTrackList = [];
    this.videoTrackIndex = 0;
    this.maxVideoTrackLength = 2;

    this.ondisconnect = function () { };
  }

  async setupConnection(useWebSocket) {
    const _this = this;
    // close current RTCPeerConnection
    if (this.pc) {
      console.log('Close current PeerConnection');
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
      console.log('signalingState changed:', e);
    };
    this.pc.oniceconnectionstatechange = function (e) {
      console.log('iceConnectionState changed:', e);
      console.log('pc.iceConnectionState:' + _this.pc.iceConnectionState);
      if (_this.pc.iceConnectionState === 'disconnected') {
        _this.ondisconnect();
      }
    };
    this.pc.onicegatheringstatechange = function (e) {
      console.log('iceGatheringState changed:', e);
    };
    this.pc.ontrack = function (e) {
      if (e.track.kind == 'video') {
        _this.videoTrackList.push(e.track);
      }
      if (e.track.kind == 'audio') {
        _this.localStream.addTrack(e.track);
      }
      if (_this.videoTrackList.length == _this.maxVideoTrackLength) {
        _this.switchVideo(_this.videoTrackIndex);
      }
    };
    this.pc.onicecandidate = function (e) {
      if (e.candidate != null) {
        _this.signaling.sendCandidate(_this.connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
      }
    };
    // Create data channel with proxy server and set up handlers
    this.channel = this.pc.createDataChannel('data');
    this.channel.onopen = function () {
      console.log('Datachannel connected.');
    };
    this.channel.onerror = function (e) {
      console.log("The error " + e.error.message + " occurred\n while handling data with proxy server.");
    };
    this.channel.onclose = function () {
      console.log('Datachannel disconnected.');
    };
    this.channel.onmessage = async (msg) => {
      // receive message from unity and operate message
      let data;
      // receive message data type is blob only on Firefox
      if (navigator.userAgent.indexOf('Firefox') != -1) {
        data = await msg.data.arrayBuffer();
      } else {
        data = msg.data;
      }
      const bytes = new Uint8Array(data);
      _this.videoTrackIndex = bytes[1];
      switch (bytes[0]) {
        case UnityEventType.SWITCH_VIDEO:
          _this.switchVideo(_this.videoTrackIndex);
          break;
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
    // It can receive two video tracks and one audio track from Unity app.
    // This operation is required to generate offer SDP correctly.
    this.pc.addTransceiver('video', { direction: 'recvonly' });
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

  // switch streaming destination main video and secondly video
  switchVideo(indexVideoTrack) {
    this.video.srcObject = this.localStream;
    this.videoThumb.srcObject = this.localStream2;

    if (indexVideoTrack == 0) {
      this.replaceTrack(this.localStream, this.videoTrackList[0]);
      this.replaceTrack(this.localStream2, this.videoTrackList[1]);
    }
    else {
      this.replaceTrack(this.localStream, this.videoTrackList[1]);
      this.replaceTrack(this.localStream2, this.videoTrackList[0]);
    }
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
      console.log('Close current PeerConnection');
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
        console.log('Connection not ready');
        break;
      case 'open':
        this.channel.send(msg);
        break;
      case 'closing':
        console.log('Attempt to sendMsg message while closing');
        break;
      case 'closed':
        console.log('Attempt to sendMsg message while connection closed.');
        break;
    }
  };
}
