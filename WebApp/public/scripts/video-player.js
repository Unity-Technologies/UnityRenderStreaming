import Signaling from "./signaling.js"

const InputEvent = {
  Keyboard: 0,
  Mouse: 1,
  MouseWheel: 2,
  Touch: 3,
  ButtonClick: 4
};

const KeyboardEventType = {
  Up: 0,
  Down: 1
}

const PointerPhase = {
  None: 0,
  Began: 1,
  Moved: 2,
  Ended: 3,
  Canceled: 4,
  Stationary: 5
}

const Keymap = {
  "Space": 1,
  "Enter": 2,
  "Tab": 3,
  "Backquote": 4,
  "Quote": 5,
  "Semicolon": 6,
  "Comma": 7,
  "Period": 8,
  "Slash": 9,
  "Backslash": 10,
  "LeftBracket": 11,
  "RightBracket": 12,
  "Minus": 13,
  "Equals": 14,
  "KeyA": 15,
  "KeyB": 16,
  "KeyC": 17,
  "KeyD": 18,
  "KeyE": 19,
  "KeyF": 20,
  "KeyG": 21,
  "KeyH": 22,
  "KeyI": 23,
  "KeyJ": 24,
  "KeyK": 25,
  "KeyL": 26,
  "KeyM": 27,
  "KeyN": 28,
  "KeyO": 29,
  "KeyP": 30,
  "KeyQ": 31,
  "KeyR": 32,
  "KeyS": 33,
  "KeyT": 34,
  "KeyU": 35,
  "KeyV": 36,
  "KeyW": 37,
  "KeyX": 38,
  "KeyY": 39,
  "KeyZ": 40,
  "Digit1": 41,
  "Digit2": 42,
  "Digit3": 43,
  "Digit4": 44,
  "Digit5": 45,
  "Digit6": 46,
  "Digit7": 47,
  "Digit8": 48,
  "Digit9": 49,
  "Digit0": 50,
  "ShiftLeft": 51,
  "ShiftRight": 52,
  "AltLeft": 53,
  "AltRight": 54,
  // "AltGr": 54,
  "ControlLeft": 55,
  "ControlRight": 56,
  "MetaLeft": 57,
  "MetaRight": 58,
  // "LeftWindows": 57,
  // "RightWindows": 58,
  // "LeftApple": 57,
  // "RightApple": 58,
  // "LeftCommand": 57,
  // "RightCommand": 58,
  "ContextMenu": 59,
  "Escape": 60,
  "ArrowLeft": 61,
  "ArrowRight": 62,
  "ArrowUp": 63,
  "ArrowDown": 64,
  "Backspace": 65,
  "PageDown": 66,
  "PageUp": 67,
  "Home": 68,
  "End": 69,
  "Insert": 70,
  "Delete": 71,
  "CapsLock": 72,
  "NumLock": 73,
  "PrintScreen": 74,
  "ScrollLock": 75,
  "Pause": 76,
  "NumpadEnter": 77,
  "NumpadDivide": 78,
  "NumpadMultiply": 79,
  "NumpadPlus": 80,
  "NumpadMinus": 81,
  "NumpadPeriod": 82,
  "NumpadEquals": 83,
  "Numpad0": 84,
  "Numpad1": 85,
  "Numpad2": 86,
  "Numpad3": 87,
  "Numpad4": 88,
  "Numpad5": 89,
  "Numpad6": 90,
  "Numpad7": 91,
  "Numpad8": 92,
  "Numpad9": 93,
  "F1": 94,
  "F2": 95,
  "F3": 96,
  "F4": 97,
  "F5": 98,
  "F6": 99,
  "F7": 100,
  "F8": 101,
  "F9": 102,
  "F10": 103,
  "F11": 104,
  "F12": 105,
  // "OEM1": 106,
  // "OEM2": 107,
  // "OEM3": 108,
  // "OEM4": 109,
  // "OEM5": 110,
  // "IMESelected": 111,
};


export class VideoPlayer {

  constructor(element, config) {

    const _this = this;
    this.cfg = VideoPlayer.getConfiguration(config);
    this.pc = null;
    this.channel = null;
    this.offerOptions = {
      offerToReceiveAudio: true,
      offerToReceiveVideo: true,
    };
    this.video = element;
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', function () {
      _this.video.play();
      _this.resizeVideo();
    }, true);
    this.interval = 3000;
    this.signaling = new Signaling();
    this.ondisconnect = function(){};
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));


      this.sendTouchMove = (e) => {
        this.sendTouch(e, PointerPhase.Moved);
        e.preventDefault();
      }

      this.sendTouchStart = (e) => {
        this.sendTouch(e, PointerPhase.Began);
        e.preventDefault();
      }

      this.sendTouchEnd = (e) => {
        this.sendTouch(e, PointerPhase.Ended);
        e.preventDefault();
      }

      this.sendTouchCancel = (e) => {
        this.sendTouch(e, PointerPhase.Canceled);
        e.preventDefault();
      }

      this.sendMouse = (e) => {
        const scale = this.videoScale;
        const originX = this.videoOriginX;
        const originY = this.videoOriginY;

        const x = (e.clientX - originX) / scale;
        // According to Unity Coordinate system
        // const y = (e.clientY - originY) / scale;
        const y = this.videoHeight - (e.clientY - originY) / scale;

        console.log("x: " + x + ", y: " + y + ", scale: " + scale + ", originX: " + originX + ", originY: " + originY + " mouse button:" + e.buttons);
        let data = new DataView(new ArrayBuffer(6));
        data.setUint8(0, InputEvent.Mouse);
        data.setInt16(1, x, true);
        data.setInt16(3, y, true);
        data.setUint8(5, e.buttons);
        this.sendMsg(data.buffer);
      }

      this.sendMouseWheel = (e) => {
        console.log("mouse wheel with delta " + e.wheelDelta);
        let data = new DataView(new ArrayBuffer(9));
        data.setUint8(0, InputEvent.MouseWheel);
        data.setFloat32(1, e.deltaX, true);
        data.setFloat32(5, e.deltaY, true);
        this.sendMsg(data.buffer);
      }

      this.sendKeyUp = (e) => {
        this.sendKey(e, KeyboardEventType.Up);
      }

      this.sendKeyDown = (e) => {
        this.sendKey(e, KeyboardEventType.Down);
      }


  }

  static getConfiguration(config) {
    if(config === undefined) {
      config = {};
    }
    config.sdpSemantics = 'unified-plan';
    config.iceServers = [{urls: ['stun:stun.l.google.com:19302']}];
    return config;
  }

  async setupConnection() {
    const _this = this;
    // close current RTCPeerConnection
    if (this.pc) {
      console.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }

    // RTCDataChannel don't work on iOS safari
    // https://github.com/webrtc/samples/issues/1123
    if (
      navigator.userAgent.match(/iPad/i) ||
      navigator.userAgent.match(/iPhone/i) ||
      navigator.userAgent.match(/Safari/i) && !navigator.userAgent.match(/Chrome/i)
    ) {
      let stream = await navigator.mediaDevices.getUserMedia({audio: true});
      stream.getTracks().forEach(t => t.stop());
    }

    // Create peerConnection with proxy server and set up handlers
    this.pc = new RTCPeerConnection(this.cfg);
    this.pc.onsignalingstatechange = function (e) {
      console.log('signalingState changed:', e);
    };

    this.pc.oniceconnectionstatechange = function (e) {
      console.log('iceConnectionState changed:', e);
      console.log('pc.iceConnectionState:' + _this.pc.iceConnectionState);
      if(_this.pc.iceConnectionState === 'disconnected') {
        _this.ondisconnect();
      }
    };

    this.pc.onicegatheringstatechange = function (e) {
      console.log('iceGatheringState changed:', e);
    };

    this.pc.ontrack = function (e) {
      console.log('New track added: ', e.streams);
      _this.video.srcObject = e.streams[0];
    };

    this.pc.onicecandidate = function (e) {
      if(e.candidate != null) {
        _this.signaling.sendCandidate(_this.sessionId, _this.connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
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

    const createResponse = await this.signaling.create();
    const data = await createResponse.json();
    this.sessionId = data.sessionId;

    // create offer
    const offer = await this.pc.createOffer(this.offerOptions);

    await this.createConnection();
    // set local sdp
    offer.sdp = offer.sdp.replace(/useinbandfec=1/, 'useinbandfec=1;stereo=1;maxaveragebitrate=1048576');
    const desc = new RTCSessionDescription({sdp:offer.sdp, type:"offer"});
    await this.pc.setLocalDescription(desc);
    await this.sendOffer(offer);
  }

  async createConnection() {
    // signaling
    const res = await this.signaling.createConnection(this.sessionId);
    const data = await res.json();
    this.connectionId = data.connectionId;
  }

  async sendOffer(offer) {
    // signaling
    await this.signaling.sendOffer(this.sessionId, this.connectionId, offer.sdp);
    this.loopGetAnswer(this.sessionId, this.interval);
    this.loopGetCandidate(this.sessionId, this.interval);
  }

  async loopGetAnswer(sessionId, interval) {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 30000;

    while(true) {
      const res = await this.signaling.getAnswer(sessionId, lastTimeRequest);
      const data = await res.json();
      const answers = data.answers;
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      if(answers.length > 0) {
        const answer = answers[0];
        await this.setAnswer(sessionId, answer.sdp);
      }
      await this.sleep(interval);
    }
  }

  async loopGetCandidate(sessionId, interval) {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 30000;

    while(true) {
      const res = await this.signaling.getCandidate(sessionId, lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const candidates = data.candidates.filter(v => v.connectionId = this.connectionId);
      if(candidates.length > 0) {
        for(let candidate of candidates[0].candidates) {
          const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex});
          await this.pc.addIceCandidate(iceCandidate);
        }
      }
      await this.sleep(interval);
    }
  }

  async setAnswer(sessionId, sdp) {
    const desc = new RTCSessionDescription({sdp:sdp, type:"answer"});
    await this.pc.setRemoteDescription(desc);
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
      console.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }
  }

  sendMsg(msg) {
    if(this.channel == null) {
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
        console.log( 'Attempt to sendMsg message while connection closed.');
        break;
    }
  }



  sendKey(e, type) {
    const key = Keymap[e.code];
    const character = e.key.length === 1 ? e.key.charCodeAt(0) : 0;
    console.log("key down " + key + ", repeat = " + e.repeat + ", character = " + character);
    this.sendMsg(new Uint8Array([InputEvent.Keyboard, type, e.repeat, key, character]).buffer);
  }

  sendTouch(e, phase) {
    const changedTouches = Array.from(e.changedTouches);
    const touches = Array.from(e.touches);
    const phrases = [];

    for (let i = 0; i < changedTouches.length; i++) {
      if (touches.find(function (t) {
        return t.identifier === changedTouches[i].identifier
      }) === undefined) {
        touches.push(changedTouches[i]);
      }
    }

    for (let i = 0; i < touches.length; i++) {
      touches[i].identifier;
      phrases[i] = changedTouches.find(
        function (e) {
          return e.identifier === touches[i].identifier
        }) === undefined ? PointerPhase.Stationary : phase;
    }

    console.log("touch phase:" + phase + " length:" + changedTouches.length + " pageX" + changedTouches[0].pageX + ", pageX: " + changedTouches[0].pageY + ", force:" + changedTouches[0].force);

    let data = new DataView(new ArrayBuffer(2 + 13 * touches.length));
    data.setUint8(0, InputEvent.Touch);
    data.setUint8(1, touches.length);
    let byteOffset = 2;
    for (let i = 0; i < touches.length; i++) {

      const scale = this.videoScale;
      const originX = this.videoOriginX;
      const originY = this.videoOriginY;

      const x = (touches[i].pageX - originX) / scale;
      // According to Unity Coordinate system
      // const y = (touches[i].pageX - originY) / scale;
      const y = this.videoHeight - (touches[i].pageY - originY) / scale;

      data.setInt32(byteOffset, touches[i].identifier, true);
      byteOffset += 4;
      data.setUint8(byteOffset, phrases[i]);
      byteOffset += 1;
      data.setInt16(byteOffset, x, true);
      byteOffset += 2;
      data.setInt16(byteOffset, y, true);
      byteOffset += 2;
      data.setFloat32(byteOffset, touches[i].force, true);
      byteOffset += 4;
    }
    this.sendMsg(data.buffer);
  }



  sendClickEvent(videoPlayer, elementId) {
      let data = new DataView(new ArrayBuffer(3));
      data.setUint8(0, InputEvent.ButtonClick);
      data.setInt16(1, elementId, true);
      this.sendMsg(data.buffer);
  }

}
