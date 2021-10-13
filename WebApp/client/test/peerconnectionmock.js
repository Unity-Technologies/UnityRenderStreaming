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
    this.audioTracks = new Map();
    this.videoTracks = new Map();
    this.channels = new Map();
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
    this.signalingState = "close";
    this.iceConnectionState = "closed";
    this.audioTracks.clear();
    this.videoTracks.clear();
    this.channels.clear();
  }

  fireOnNegotiationNeeded() {
    if (this.onnegotiationneeded) {
      this.onnegotiationneeded();
    }
  }

  addTrack(track) {
    if (track.kind == "audio") {
      this.audioTracks.set(track.id, track);
    } else {
      this.videoTracks.set(track.id, track);
    }
    this.fireOnNegotiationNeeded();
  }

  addTransceiver(trackOrKind) {
    if (typeof trackOrKind == "string") {
      const track = { id: getUniqueId(), kind: trackOrKind };
      if (track.kind == "audio") {
        this.audioTracks.set(track.id, track);
      } else {
        this.videoTracks.set(track.id, track);
      }
      this.fireOnNegotiationNeeded();
      return;
    }

    if (trackOrKind.kind == "audio") {
      this.audioTracks.set(trackOrKind.id, trackOrKind);
    } else {
      this.videoTracks.set(trackOrKind.id, trackOrKind);
    }
    this.fireOnNegotiationNeeded();
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
  }

  async addIceCandidate(candidate) {
    await this.delay();
    this.candidates.push(candidate);
  }
}