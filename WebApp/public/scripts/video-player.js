import SignalingChannel from "./signaling-channel.js"

export class VideoPlayer {
  constructor(element, options) {
    const _this = this;
    if(options === undefined) {
      options = {};
    }
    this.cfg = options;
    this.cfg.sdpSemantics = 'unified-plan';
    this.cfg.iceServers = [{urls: ['stun:stun.l.google.com:19302']}];
    this.pc = null;
    this.channel = null;
    this.offerOptions = {
      offerToReceiveAudio: true,
      offerToReceiveVideo: true,
    };
    this.video = element;
    this.video.id = 'Video';
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', function () {
      _this.video.play();
    }, true);
    this.interval = 3000;
    this.signalingChannel = new SignalingChannel();
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));
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
    if (navigator.userAgent.match(/iPad/i) || navigator.userAgent.match(/iPhone/i)) {
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
        _this.signalingChannel.sendCandidate(_this.sessionId, _this.connectionId, e.candidate.candidate, e.candidate.sdpMid, e.candidate.sdpMLineIndex);
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

    const createResponse = await this.signalingChannel.create();
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
    const res = await this.signalingChannel.createConnection(this.sessionId);
    const data = await res.json();
    this.connectionId = data.connectionId;
  }

  async sendOffer(offer) {
    // signaling
    await this.signalingChannel.sendOffer(this.sessionId, this.connectionId, offer.sdp);
    this.loopGetAnswer(this.sessionId, this.interval);
    this.loopGetCandidate(this.sessionId, this.interval);
  }

  async loopGetAnswer(sessionId, interval) {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 30000;

    while(true) {
      const res = await this.signalingChannel.getAnswer(sessionId, lastTimeRequest);
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
      const res = await this.signalingChannel.getCandidate(sessionId, lastTimeRequest);
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

  close() {
    if (this.pc) {
      console.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }
  };

  sendMsg(msg) {
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
