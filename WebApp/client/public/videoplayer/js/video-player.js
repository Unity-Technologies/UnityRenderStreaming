import { Signaling, WebSocketSignaling } from "../../module/signaling.js";
import Peer from "../../module/peer.js";
import * as Logger from "../../module/logger.js";


// enum type of event sending from Unity
var UnityEventType = {
  SWITCH_VIDEO: 0
};

function uuid4() {
  var temp_url = URL.createObjectURL(new Blob());
  var uuid = temp_url.toString();
  URL.revokeObjectURL(temp_url);
  return uuid.split(/[:/]/g).pop().toLowerCase(); // remove prefixes
}

export class VideoPlayer {
  constructor(elements) {
    const _this = this;
    this.pc = null;
    this.channel = null;
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
      Logger.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }

    if (useWebSocket) {
      this.signaling = new WebSocketSignaling();
    } else {
      this.signaling = new Signaling();
    }

    this.connectionId = uuid4();

    // Create peerConnection with proxy server and set up handlers
    this.pc = new Peer(this.connectionId, true);
    this.pc.addEventListener('disconnect', () => {
      _this.ondisconnect();
    });
    this.pc.addEventListener('trackevent', (e) => {
      const data = e.detail;
      if (data.track.kind == 'video') {
        _this.videoTrackList.push(data.track);
      }
      if (data.track.kind == 'audio') {
        _this.localStream.addTrack(data.track);
      }
      if (_this.videoTrackList.length == _this.maxVideoTrackLength) {
        _this.switchVideo(_this.videoTrackIndex);
      }
    });
    this.pc.addEventListener('sendoffer', (e) => {
      const offer = e.detail;
      _this.signaling.sendOffer(offer.connectionId, offer.sdp);
    });
    this.pc.addEventListener('sendanswer', (e) => {
      const answer = e.detail;
      _this.signaling.sendAnswer(answer.connectionId, answer.sdp);
    });
    this.pc.addEventListener('sendcandidate', (e) => {
      const candidate = e.detail;
      _this.signaling.sendCandidate(candidate.connectionId, candidate.candidate, candidate.sdpMid, candidate.sdpMLineIndex);
    });

    this.signaling.addEventListener('disconnect', async (e) => {
      const data = e.detail;
      if (_this.pc != null && _this.pc.connectionId == data.connectionId) {
        _this.ondisconnect();
      }
    });
    this.signaling.addEventListener('offer', async (e) => {
      const offer = e.detail;
      const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
      if (_this.pc != null) {
        await _this.pc.onGotDescription(offer.connectionId, desc);
      }
    });
    this.signaling.addEventListener('answer', async (e) => {
      const answer = e.detail;
      const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
      if (_this.pc != null) {
        await _this.pc.onGotDescription(answer.connectionId, desc);
      }
    });
    this.signaling.addEventListener('candidate', async (e) => {
      const candidate = e.detail;
      const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
      if (_this.pc != null) {
        await _this.pc.onGotCandidate(candidate.connectionId, iceCandidate);
      }
    });

    // setup signaling
    await this.signaling.start();

    // Create data channel with proxy server and set up handlers
    this.channel = this.pc.createDataChannel(this.connectionId, 'data');
    this.channel.onopen = function () {
      Logger.log('Datachannel connected.');
    };
    this.channel.onerror = function (e) {
      Logger.log("The error " + e.error.message + " occurred\n while handling data with proxy server.");
    };
    this.channel.onclose = function () {
      Logger.log('Datachannel disconnected.');
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
  }

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
  }

  async stop() {
    if (this.signaling) {
      await this.signaling.stop();
      this.signaling = null;
    }

    if (this.pc) {
      this.pc.close();
      this.pc = null;
    }
  }
}
