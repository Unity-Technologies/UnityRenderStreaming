export class Receiver {
  constructor(videoElement) {
    const _this = this;
    this.localStream = new MediaStream();
    this.video = videoElement;
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', function () {
      _this.video.play();
      _this.resizeVideo();
    }, true);
    this.video.srcObject = this.localStream;
  }

  addTrack(track) {
    this.video.srcObject.addTrack(track);
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
}
