export const LetterBoxType = {
  Vertical: 0,
  Horizontal: 1
};

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

    // (3) add offset of letterbox
    _position[0] -= this._offsetX;
    _position[1] -= this._offsetY;

    // (4) mapping element rectangle to video rectangle
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

  get letterBoxType() {
    const videoRatio = this._videoHeight / this._videoWidth;
    const rectRatio = this._rect.height / this._rect.width;
    return videoRatio > rectRatio ? LetterBoxType.Vertical : LetterBoxType.Horizontal;
  }

  get letterBoxSize() {
    switch(this.letterBoxType) {
      case LetterBoxType.Horizontal:
        const ratioWidth = this._rect.width / this._videoWidth;
        const height = this._videoHeight * ratioWidth;
        return (this._rect.height - height) * 0.5;
      case LetterBoxType.Vertical:
        const ratioHeight = this._rect.height / this._videoHeight;
        const width = this._videoWidth * ratioHeight;
        return (this._rect.width - width) * 0.5;
    }
  }

  _reset() {
    this._videoRatio = this._videoHeight / this.videoWidth;
    this._rectRatio = this._rect.height / this.width;
    this._letterBox = this._videoRatio > this._rectRatio ? LetterBoxType.Vertical : LetterBoxType.Horizontal;

    this._offsetX = this._letterBox == LetterBoxType.Vertical ? this.letterBoxSize : 0;
    this._offsetY = this._letterBox == LetterBoxType.Horizontal ? this.letterBoxSize : 0;
  }
}  