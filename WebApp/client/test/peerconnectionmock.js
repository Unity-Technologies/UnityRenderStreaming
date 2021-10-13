import { sleep, getUniqueId } from './testutils';

// type RTCIceConnectionState = "checking" | "closed" | "completed" | "connected" | "disconnected" | "failed" | "new";
// type RTCRtpTransceiverDirection = "inactive" | "recvonly" | "sendonly" | "sendrecv" | "stopped";
// type RTCSdpType = "answer" | "offer" | "pranswer" | "rollback";
// type RTCSignalingState = "closed" | "have-local-offer" | "have-local-pranswer" | "have-remote-offer" | "have-remote-pranswer" | "stable";


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
    this.localDescription = { sdp: null, type: null };
    this.remoteDescription = { sdp: null, type: null };
    this.candidates = [];
    this.signalingState = "closed";
    this.iceConnectionState = "closed";
    this.audioTracks = new Map();
    this.videoTracks = new Map();
    this.channels = new Map();
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

  async setLocalDescription(description) {
    await this.delay();
    this.localDescription = description;
  }

  async setRemoteDescription(description) {
    await this.delay();
    this.remoteDescription = description;
  }

  async addIceCandidate(candidate) {
    await this.delay();
    this.candidates.push(candidate);
  }
}