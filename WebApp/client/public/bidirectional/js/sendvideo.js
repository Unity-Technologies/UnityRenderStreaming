import { Signaling, WebSocketSignaling } from "../../js/signaling.js";
import Peer from "../../js/peer.js";
import * as Logger from "../../js/logger.js";

export class SendVideo {
  constructor() {
    this.pc = null;
    this.localVideo = null;
    this.remoteVideo = null;
    this.preferedCodecs = null;
    this.ondisconnect = function (message) { Logger.log(`Disconnect peer. message:${message}`); };
  }

  async startVideo(localVideo, videoSource, audioSource, videoWidth, videoHeight) {
    try {
      this.localVideo = localVideo;
      const constraints = {
        video: { deviceId: videoSource ? { exact: videoSource } : undefined },
        audio: { deviceId: audioSource ? { exact: audioSource } : undefined }
      };

      if (videoWidth != null || videoWidth != 0) {
        constraints.video.width = videoWidth;
      }
      if (videoHeight != null || videoHeight != 0) {
        constraints.video.height = videoHeight;
      }

      this.localStream = await navigator.mediaDevices.getUserMedia(constraints);
      this.localVideo.srcObject = this.localStream;
      await localVideo.play();
    } catch (err) {
      Logger.error(`mediaDevice.getUserMedia() error:${err}`);
    }
  }

  async setupConnection(remoteVideo, connectionId, useWebSocket, codecs) {
    const _this = this;
    this.remoteVideo = remoteVideo;
    this.preferedCodecs = codecs;

    if (useWebSocket) {
      this.signaling = new WebSocketSignaling();
    } else {
      this.signaling = new Signaling();
    }

    this.signaling.addEventListener('connect', async (e) => {
      const data = e.detail;
      _this.prepareNewPeerConnection(data.connectionId, data.polite);
      _this.addTracks(data.connectionId);
    });

    this.signaling.addEventListener('disconnect', async (e) => {
      const data = e.detail;
      if (_this.pc != null && _this.pc.connectionId == data.connectionId) {
        _this.ondisconnect(`Receive disconnect message from server. connectionId:${data.connectionId}`);
      }
    });

    this.signaling.addEventListener('offer', async (e) => {
      const offer = e.detail;
      if (_this.pc == null) {
        _this.prepareNewPeerConnection(offer.connectionId, offer.polite);
        _this.addTracks(offer.connectionId);
      }
      const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
      try {
        await _this.pc.onGotDescription(offer.connectionId, desc);
      } catch (error) {
        _this.ondisconnect(`Error happen on GotDescription that description.\n Message: ${error}\n RTCSdpType:${desc.type}\n sdp:${desc.sdp}`);
      }
    });

    this.signaling.addEventListener('answer', async (e) => {
      const answer = e.detail;
      const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
      if (_this.pc != null) {
        try {
          await _this.pc.onGotDescription(answer.connectionId, desc);
        } catch (error) {
          _this.ondisconnect(`Error happen on GotDescription that description.\n Message: ${error}\n RTCSdpType:${desc.type}\n sdp:${desc.sdp}`);
        }
      }
    });

    this.signaling.addEventListener('candidate', async (e) => {
      const candidate = e.detail;
      const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
      if (_this.pc != null) {
        await _this.pc.onGotCandidate(candidate.connectionId, iceCandidate);
      }
    });

    await this.signaling.start();
    await this.signaling.createConnection(connectionId);
  }

  prepareNewPeerConnection(connectionId, polite) {
    const _this = this;
    // close current RTCPeerConnection
    if (this.pc) {
      Logger.log('Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }

    // Create peerConnection with proxy server and set up handlers
    this.pc = new Peer(connectionId, polite, this.preferedCodecs);
    this.pc.addEventListener('disconnect', () => {
      _this.ondisconnect(`Receive disconnect message from peer.`);
    });
    this.pc.addEventListener('trackevent', (e) => {
      const trackEvent = e.detail;
      const direction = trackEvent.transceiver.direction;
      if (direction == "sendrecv" || direction == "recvonly") {
        if (_this.remoteVideo.srcObject == null) {
          _this.remoteVideo.srcObject = new MediaStream();
        }
        _this.remoteVideo.srcObject.addTrack(trackEvent.track);
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
  }

  addTracks(connectionId) {
    const _this = this;
    const tracks = _this.localVideo.srcObject.getTracks();
    for (const track of tracks) {
      _this.pc.addTrack(connectionId, track);
    }
  }

  async getStats(connectionId) {
    return await this.pc.getStats(connectionId);
  }

  async hangUp(connectionId) {
    if (this.signaling == null) {
      return;
    }

    this.pc.close();
    this.pc = null;
    Logger.log(`delete connection ${connectionId}`);
    await this.signaling.deleteConnection(connectionId);

    this.remoteVideo.srcObject = null;
    await this.stop();
  }

  async stop() {
    if (this.signaling) {
      await this.signaling.stop();
      this.signaling = null;
    }
  }
}
