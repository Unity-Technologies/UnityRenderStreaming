export class PointerCorrector {

  /**
   * @param {Number} videoWidth 
   * @param {Number} videoHeight
   * @param {DOMRect} rect
   */
  constructor(videoWidth, videoHeight, rect) {
    this.reset(videoWidth, videoHeight, rect);
  }

  /**
   * @param {Number[]} position MouseEvent.clientX, MouseEvent.clientY
   * @returns {Number[]}
   */
   map(position) {
    const _position = new Array(2);

    // (1) set origin point to zero
    _position[0] = position[0] - this._rect.left;
    _position[1] = position[1] - this._rect.top;

    // (2) translate Unity coordinate system (reverse y-axis)
    _position[1] = this._rect.height - _position[1];

    // (3) mapping element rectangle to video rectangle
    _position[0] = _position[0] / this._rect.width * this._videoWidth;
    _position[1] = _position[1] / this._rect.height * this._videoHeight;

    return _position;
  }

  /**
   * @param {Number} videoWidth
   */
   setVideoWidth(videoWidth) {
    this._videoWidth = videoWidth;
    this._reset();
  }

  /**
   * @param {Number} videoHeight
   */
  setVideoHeight(videoHeight) {
    this._videoHeight = videoHeight;
    this._reset();
  }

  /**
   * @param {DOMRect} rect
   */
  setRect(rect) {
    this._rect = rect;
    this._reset();
  }

  /**
   * @param {Number} videoWidth 
   * @param {Number} videoHeight
   * @param {DOMRect} rect
   */
  reset(videoWidth, videoHeight, rect) {
    this._videoWidth = videoWidth;
    this._videoHeight = videoHeight;
    this._rect = rect;
    this._reset();
  }

  _reset() {
    this._videoRatio = this._videoHeight / this.videoWidth;
  }
}  