import { sleep, getUniqueId } from './testutils';

export default class PeerConnectionMock extends EventTarget {
  constructor(config) {
    super();
    this.delay = async () => await sleep(10);
    this.config = config;
    this.ontrack = undefined;
    this.onicecandidate = undefined;
    this.onnegotiationneeded = undefined;
    this.onsignalingstatechange = undefined;
    this.oniceconnectionstatechange = undefined;
    this.onicegatheringstatechange = undefined;
    this.pendingLocalDescription = null;
    this.currentLocalDescription = null;
    this.pendingRemoteDescription = null;
    this.currentRemoteDescription = null;
    this.candidates = [];
    this.signalingState = "stable";
    this.iceConnectionState = "new";
    this.iceGatheringState = "new";
    this.audioTracks = new Map();
    this.videoTracks = new Map();
    this.channels = new Map();
    this.transceiverCount = 0;
    this.transceivers = new Map();
  }

  get localDescription() {
    if (this.pendingLocalDescription) {
      return this.pendingLocalDescription;
    }

    return this.currentLocalDescription;
  }

  get remoteDescription() {
    if (this.pendingRemoteDescription) {
      return this.pendingRemoteDescription;
    }

    return this.currentRemoteDescription;
  }

  close() {
    this.ontrack = undefined;
    this.onicecandidate = undefined;
    this.onnegotiationneeded = undefined;
    this.onsignalingstatechange = undefined;
    this.oniceconnectionstatechange = undefined;
    this.onicegatheringstatechange = undefined;
    this.pendingLocalDescription = null;
    this.currentLocalDescription = null;
    this.pendingRemoteDescription = null;
    this.currentRemoteDescription = null;
    this.candidates = [];
    this.signalingState = "close";
    this.iceConnectionState = "closed";
    this.audioTracks.clear();
    this.videoTracks.clear();
    this.channels.clear();
    this.transceiverCount = 0;
    this.transceivers.clear();
  }

  fireOnNegotiationNeeded() {
    if (this.onnegotiationneeded) {
      this.onnegotiationneeded();
    }
  }

  getTransceivers() {
    return Array.from(this.transceivers.values());
  }

  addTrack(track) {
    if (track.kind == "audio") {
      this.audioTracks.set(track.id, track);
    } else {
      this.videoTracks.set(track.id, track);
    }
    const transceiver = { sender: { direction: "sendrecv", track: track }, receiver: null };
    this.transceivers.set(this.transceiverCount++, transceiver);
    this.fireOnNegotiationNeeded();
    return transceiver.sender;
  }

  addTransceiver(trackOrKind) {
    if (typeof trackOrKind == "string") {
      const track = { id: getUniqueId(), kind: trackOrKind };
      if (track.kind == "audio") {
        this.audioTracks.set(track.id, track);
      } else {
        this.videoTracks.set(track.id, track);
      }
      const transceiver = { sender: { direction: "sendrecv", track: track }, receiver: null };
      this.transceivers.set(this.transceiverCount++, transceiver);
      this.fireOnNegotiationNeeded();
      return transceiver;
    }

    if (trackOrKind.kind == "audio") {
      this.audioTracks.set(trackOrKind.id, trackOrKind);
    } else {
      this.videoTracks.set(trackOrKind.id, trackOrKind);
    }
    const transceiver = { sender: { direction: "sendrecv", track: trackOrKind }, receiver: null };
    this.transceivers.set(this.transceiverCount++, transceiver);
    this.fireOnNegotiationNeeded();
    return transceiver;
  }

  createDataChannel(label) {
    const channel = { id: getUniqueId(), label: label };
    this.channels.set(channel.id, channel);
    this.fireOnNegotiationNeeded();
    return channel;
  }

  async setLocalDescription(description = null) {
    if (description == null) {
      description = this.createSessionDescription();
    }
    await this.delay();
    this.setSessionDescription(description, false);
  }

  async setRemoteDescription(description) {
    await this.delay();
    if (description.type == "offer" && this.signalingState == "have-local-offer") {
      this.setSessionDescription({ type: "rollback", sdp: "" }, true);
    }
    this.setSessionDescription(description, true);
  }

  createSessionDescription() {
    if (this.signalingState == "stable" || this.signalingState == "have-local-offer" || this.signalingState == "have-remote-pranswer") {
      return { type: "offer", sdp: "lastcreatedoffer" };
    }
    return { type: "answer", sdp: "lastcreatedanswer" };
  }

  setSessionDescription(description, remote) {
    if (description.type == "rollback"
      && (this.signalingState == "stable" || this.signalingState == "have-local-pranswer" || this.signalingState == "have-remote-pranswer")) {
      throw "InvalidStateError";
    }

    if (description.type != "rollback") {
      if (remote) {
        if (description.type == "offer") {
          this.pendingRemoteDescription = description;
          this.signalingState = "have-remote-offer";
          this.onsignalingstatechange(this.signalingState);
          // if sdp contains track string, create dummy track
          if (description.sdp.includes("track")) {
            const track = { id: getUniqueId(), kind: "video" };
            this.videoTracks.set(track.id, track);
          }
        }
        if (description.type == "answer") {
          this.currentRemoteDescription = description;
          this.currentLocalDescription = this.pendingLocalDescription;
          this.pendingLocalDescription = null;
          this.pendingRemoteDescription = null;
          this.signalingState = "stable";
          this.onsignalingstatechange(this.signalingState);
        }
        if (description.type == "pranswer") {
          this.pendingRemoteDescription = description;
          this.signalingState = "have-remote-pranswer";
          this.onsignalingstatechange(this.signalingState);
        }
      } else {
        if (description.type == "offer") {
          this.pendingLocalDescription = description;
          this.signalingState = "have-local-offer";
          this.onsignalingstatechange(this.signalingState);
        }
        if (description.type == "answer") {
          this.currentLocalDescription = description;
          this.currentRemoteDescription = this.pendingRemoteDescription;
          this.pendingLocalDescription = null;
          this.pendingRemoteDescription = null;
          this.signalingState = "stable";
          this.onsignalingstatechange(this.signalingState);
          // if sdp contains track string, create dummy track
          if (description.sdp.includes("track")) {
            const track = { id: getUniqueId(), kind: "audio" };
            this.audioTracks.set(track.id, track);
          }
        }
        if (description.type == "pranswer") {
          this.pendingLocalDescription = description;
          this.signalingState = "have-local-pranswer";
          this.onsignalingstatechange(this.signalingState);
        }
      }
    } else {
      this.pendingLocalDescription = null;
      this.pendingRemoteDescription = null;
      this.signalingState = "stable";
      this.onsignalingstatechange(this.signalingState);
    }

    if (this.videoTracks.size != 0 || this.audioTracks.size != 0) {
      this.mockGatheringIceCandidate(this.videoTracks.size + this.audioTracks.size);
    }

    //fire ontrack with new tracks, after using tracks clear.
    if (this.ontrack) {
      for (const track of this.videoTracks.values()) {
        this.ontrack({ track: track });
      }
      this.videoTracks.clear();

      for (const track of this.audioTracks.values()) {
        this.ontrack({ track: track });
      }
      this.audioTracks.clear();
    }
  }

  async mockGatheringIceCandidate(count) {
    this.iceGatheringState = "gathering";
    this.onicegatheringstatechange(this.iceGatheringState);
    if (this.onicecandidate) {
      for (let index = 0; index < count; index++) {
        await this.delay();
        const newCandidate = { candidate: getUniqueId(), sdpMLineIndex: index, sdpMid: index };
        this.onicecandidate(newCandidate);
      }
    }
    this.iceGatheringState = "complete";
    this.onicegatheringstatechange(this.iceGatheringState);
    this.onicecandidate({ candidate: null, sdpMLineIndex: null, sdpMid: null });
  }

  async addIceCandidate(candidate) {
    await this.delay();
    if (this.remoteDescription == null) {
      throw "InvalidStateError";
    }
    this.candidates.push(candidate);
  }
}
