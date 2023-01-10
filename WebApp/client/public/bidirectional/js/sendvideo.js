import * as Logger from "../../module/logger.js";

export class SendVideo {
  constructor(localVideoElement, remoteVideoElement) {
    this.localVideo = localVideoElement;
    this.remoteVideo = remoteVideoElement;
  }

  /**
   * @param {MediaTrackConstraints} videoSource
   * @param {MediaTrackConstraints} audioSource
   * @param {number} videoWidth
   * @param {number} videoHeight
   */
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

      const localStream = await navigator.mediaDevices.getUserMedia(constraints);
      this.localVideo.srcObject = localStream;
      await this.localVideo.play();
    } catch (err) {
      Logger.error(`mediaDevice.getUserMedia() error:${err}`);
    }
  }

  /**
   * @returns {MediaStreamTrack[]}
   */
  getLocalTracks() {
    return this.localVideo.srcObject.getTracks();
  }

  /**
   * @param {MediaStreamTrack} track
   */
  addRemoteTrack(track) {
    if (this.remoteVideo.srcObject == null) {
      this.remoteVideo.srcObject = new MediaStream();
    }
    this.remoteVideo.srcObject.addTrack(track);
  }
}
