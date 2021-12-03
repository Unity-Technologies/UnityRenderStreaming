import {
  Signaling,
  WebSocketSignaling 
} from "../../js/signaling.js";

import {
  Observer,
  Sender
} from "../../js/sender.js";

import { 
  InputRemoting 
} from "../../js/inputremoting.js";


import * as Config from "../../js/config.js";
import * as Logger from "../../js/logger.js";

/** @enum {number} */
const ActionType = {
  ChangeLabel: 0
};

function uuid4() {
  var temp_url = URL.createObjectURL(new Blob());
  var uuid = temp_url.toString();
  URL.revokeObjectURL(temp_url);
  return uuid.split(/[:/]/g).pop().toLowerCase(); // remove prefixes
}

function isTouchDevice() {
  return (('ontouchstart' in window) ||
     (navigator.maxTouchPoints > 0) ||
     (navigator.msMaxTouchPoints > 0));
}

export class VideoPlayer {
  constructor(elements) {
    const _this = this;
    this.cfg = Config.getRTCConfiguration();
    this.pc = null;
    this.multiplayChannel = null;
    this.connectionId = null;
    this.sender = new Sender(elements[0]);
    this.sender.addMouse();
    this.sender.addKeyboard();
    if(isTouchDevice()) {
      this.sender.addTouchscreen();
    }
    this.sender.addGamepad();
    this.inputRemoting = new InputRemoting(this.sender);

    // main video
    this.localStream = new MediaStream();
    this.video = elements[0];
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', function () {
      _this.video.play();
      _this.resizeVideo();
    }, true);

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
        this.localStream.addTrack(e.track);
        this.video.srcObject = this.localStream;
    }.bind(this);
    this.pc.onicecandidate = function (e) {
      if (e.candidate != null) {
        this.signaling.sendCandidate(_this.connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
      }
    }.bind(this);

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

    // Create data channel with proxy server and set up handlers
    this.inputSenderChannel = this.pc.createDataChannel("input");
    this.inputSenderChannel.onopen = this._onOpenInputSenderChannel.bind(this);
    this.multiplayChannel = this.pc.createDataChannel("multiplay");
    this.multiplayChannel.onopen = this._onOpenMultiplayChannel.bind(this);

    this.inputRemoting.subscribe(new Observer(this.inputSenderChannel));

    // Add transceivers to receive multi stream.
    // It can receive two video tracks and one audio track from Unity app.
    // This operation is required to generate offer SDP correctly.
    this.pc.addTransceiver('video', { direction: 'recvonly' });

    // create offer
    const offer = await this.pc.createOffer();

    // set local sdp
    const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
    await this.pc.setLocalDescription(desc);
    await this.signaling.sendOffer(this.connectionId, offer.sdp);
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
  }

  async _onOpenMultiplayChannel() {
    await new Promise(resolve => setTimeout(resolve, 100));
    const num = Math.floor(Math.random() * 100000);
    this._changeLabel(String(num));
  }

  async _onOpenInputSenderChannel() {
    await new Promise(resolve => setTimeout(resolve, 100));
    this.inputRemoting.startSending();
  }

  _changeLabel(label) {
    const json = JSON.stringify({type: ActionType.ChangeLabel, argument: label});
    this.multiplayChannel.send(json);
  }

  async stop() {
    await this.signaling.stop();
  }
}
