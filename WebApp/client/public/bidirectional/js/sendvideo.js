import * as Logger from "../../js/logger.js";

export class SendVideo {
  constructor(localVideoElement, remoteVideoElement) {
    this.localVideo = localVideoElement;
    this.remoteVideo = remoteVideoElement;
  }

  async startLocalVideo(videoSource, audioSource, videoWidth, videoHeight) {
    try {
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

  getLocalTracks() {
    return this.localVideo.srcObject.getTracks();
  }

  addRemoteTrack(track) {
    if (remoteVideo.srcObject == null) {
      remoteVideo.srcObject = new MediaStream();
    }
    remoteVideo.srcObject.addTrack(track);
  }
}
