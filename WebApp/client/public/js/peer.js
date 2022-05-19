import * as Config from "./config.js";
import * as Logger from "./logger.js";

export default class Peer extends EventTarget {
  constructor(connectionId, polite, resendIntervalMsec = 5000) {
    super();
    const _this = this;
    this.connectionId = connectionId;
    this.polite = polite;
    this.config = Config.getRTCConfiguration();
    this.pc = new RTCPeerConnection(this.config);
    this.makingOffer = false;
    this.waitingAnswer = false;
    this.ignoreOffer = false;
    this.srdAnswerPending = false;
    this.log = str => void Logger.log(`[${_this.polite ? 'POLITE' : 'IMPOLITE'}] ${str}`);
    this.assert_equals = window.assert_equals ? window.assert_equals : (a, b, msg) => { if (a === b) { return; } throw new Error(`${msg} expected ${b} but got ${a}`); };
    this.interval = resendIntervalMsec;
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));

    this.pc.ontrack = e => {
      _this.log(`ontrack:${e}`);
      _this.dispatchEvent(new CustomEvent('trackevent', { detail: e }));
    };
    this.pc.onicecandidate = ({ candidate }) => {
      _this.log(`send candidate:${candidate}`);
      if (candidate == null) {
        return;
      }
      _this.dispatchEvent(new CustomEvent('sendcandidate', { detail: { connectionId: _this.connectionId, candidate: candidate.candidate, sdpMLineIndex: candidate.sdpMLineIndex, sdpMid: candidate.sdpMid } }));
    };

    this.pc.onnegotiationneeded = async () => {
      try {
        _this.log(`SLD due to negotiationneeded`);
        _this.assert_equals(_this.pc.signalingState, 'stable', 'negotiationneeded always fires in stable state');
        _this.assert_equals(_this.makingOffer, false, 'negotiationneeded not already in progress');
        _this.makingOffer = true;
        await _this.pc.setLocalDescription();
        _this.assert_equals(_this.pc.signalingState, 'have-local-offer', 'negotiationneeded not racing with onmessage');
        _this.assert_equals(_this.pc.localDescription.type, 'offer', 'negotiationneeded SLD worked');
        _this.waitingAnswer = true;
        _this.dispatchEvent(new CustomEvent('sendoffer', { detail: { connectionId: _this.connectionId, sdp: _this.pc.localDescription.sdp } }));
      } catch (e) {
        _this.log(e);
      } finally {
        _this.makingOffer = false;
      }
    };

    this.pc.onsignalingstatechange = e => {
      _this.log(`signalingState changed:${e}`);
    };

    this.pc.oniceconnectionstatechange = e => {
      _this.log(`iceConnectionState changed:${e}`);
      if (_this.pc.iceConnectionState === 'disconnected') {
        this.dispatchEvent(new Event('disconnect'));
      }
    };

    this.pc.onicegatheringstatechange = e => {
      _this.log(`iceGatheringState changed:${e}'`);
    };

    this.loopResendOffer();
  }

  async loopResendOffer() {
    while (this.connectionId) {
      if (this.pc != null && this.waitingAnswer) {
        this.dispatchEvent(new CustomEvent('sendoffer', { detail: { connectionId: this.connectionId, sdp: this.pc.localDescription.sdp } }));
      }
      await this.sleep(this.interval);
    }
  }

  close() {
    this.connectionId = null;
    if (this.pc != null) {
      this.pc.close();
      this.pc = null;
    }
  }

  getTransceivers(connectionId) {
    if (this.connectionId != connectionId) {
      return null;
    }

    return this.pc.getTransceivers();
  }

  addTrack(connectionId, track) {
    if (this.connectionId != connectionId) {
      return null;
    }

    const sender = this.pc.addTrack(track);
    const transceiver = this.pc.getTransceivers().find((t) => t.sender == sender);
    transceiver.direction = "sendonly";
    return sender;
  }

  addTransceiver(connectionId, trackOrKind, init) {
    if (this.connectionId != connectionId) {
      return null;
    }

    return this.pc.addTransceiver(trackOrKind, init);
  }

  createDataChannel(connectionId, label) {
    if (this.connectionId != connectionId) {
      return null;
    }

    return this.pc.createDataChannel(label);
  }

  async onGotDescription(connectionId, description) {
    if (this.connectionId != connectionId) {
      return;
    }

    const _this = this;
    const isStable =
      this.pc.signalingState == 'stable' ||
      (this.pc.signalingState == 'have-local-offer' && this.srdAnswerPending);
    this.ignoreOffer =
      description.type == 'offer' && !this.polite && (this.makingOffer || !isStable);

    if (this.ignoreOffer) {
      _this.log(`glare - ignoring offer`);
      return;
    }

    this.waitingAnswer = false;
    this.srdAnswerPending = description.type == 'answer';
    _this.log(`SRD(${description.type})`);
    await this.pc.setRemoteDescription(description);
    this.srdAnswerPending = false;

    if (description.type == 'offer') {
      _this.assert_equals(this.pc.signalingState, 'have-remote-offer', 'Remote offer');
      _this.assert_equals(this.pc.remoteDescription.type, 'offer', 'SRD worked');
      _this.log('SLD to get back to stable');
      await this.pc.setLocalDescription();
      _this.assert_equals(this.pc.signalingState, 'stable', 'onmessage not racing with negotiationneeded');
      _this.assert_equals(this.pc.localDescription.type, 'answer', 'onmessage SLD worked');
      _this.dispatchEvent(new CustomEvent('sendanswer', { detail: { connectionId: _this.connectionId, sdp: _this.pc.localDescription.sdp } }));

    } else {
      _this.assert_equals(this.pc.remoteDescription.type, 'answer', 'Answer was set');
      _this.assert_equals(this.pc.signalingState, 'stable', 'answered');
      this.pc.dispatchEvent(new Event('negotiated'));
    }
  }

  async onGotCandidate(connectionId, candidate) {
    if (this.connectionId != connectionId) {
      return;
    }

    try {
      await this.pc.addIceCandidate(candidate);
    } catch (e) {
      if (!this.ignoreOffer) this.log(e);
    }
  }
}
