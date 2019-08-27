import Signaling from "./signaling.js"

export class VideoPlayer {

  constructor(elements, config) {
    const _this = this;
    this.cfg = VideoPlayer.getConfiguration(config);
    this.pc = null;
    this.channel = null;
    this.UnityStreams = [];
    this.UnityStreamCount = 2;
    this.offerOptions = {
      offerToReceiveAudio: true,
      offerToReceiveVideo: true,
    };

    this.videos = elements;
    this.videos.forEach(v=>{
      v.playsInline = true;
      v.addEventListener('loadedmetadata', function () {
        v.play();
        _this.resizeVideo();
      }, true);
    })

// TODO:: Fix "resizeVideo()" function to be able to use multiple videos
//    this.video = element;
//    this.video.playsInline = true;
//    this.video.addEventListener('loadedmetadata', function () {
//      _this.video.play();
//      _this.resizeVideo();
//    }, true);

    this.interval = 3000;
    this.signaling = new Signaling();
    this.ondisconnect = function(){};
    this.onaddtrackfinish = function (mediaStreams) {};
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));
  }

  static getConfiguration(config) {
    if(config === undefined) {
      config = {};
    }
    config.sdpSemantics = 'unified-plan';
    config.iceServers = [{urls: ['stun:stun.l.google.com:19302']}];
    config.bundlePolicy = "max-bundle";
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

    this.pc.addTransceiver("video");
    this.pc.addTransceiver("audio");
    this.pc.addTransceiver("video");
    this.pc.addTransceiver("audio");


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
    let tempCount = 0;
    this.pc.ontrack = function (e) {

      console.log('New track added: ', e.streams);
      console.log(e.track);

      if (_this.UnityStreams.indexOf(e.streams[0])==-1)
      {
        _this.UnityStreams.push(e.streams[0]);
        if ( _this.UnityStreamCount==_this.UnityStreams.length )
        {
          _this.onaddtrackfinish(_this.UnityStreams);
        }
      }
    };

    _this.videos[0].onresize = function () {
      console.log("video 0 width:=" + _this.videos[0].videoWidth);
      console.log("video 0 height:=" + _this.videos[0].videoHeight);
    }

    _this.videos[1].onresize = function () {
      console.log("video 1 width:=" + _this.videos[1].videoWidth);
      console.log("video 1 height:=" + _this.videos[1].videoHeight);
    }

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
  };

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
  };

  selectMediaStream(streamId){

    for (let i=0; i<this.UnityStreams.length; ++i)
    {
      if (streamId==this.UnityStreams[i].id)
      {
        this.videos[i].srcObject = this.UnityStreams[i];
      }
    }

  };

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
  };
}
