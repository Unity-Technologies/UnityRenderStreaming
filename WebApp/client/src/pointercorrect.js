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
    _position[0] -= this._contentRect.x;
    _position[1] -= this._contentRect.y;

    // (4) mapping element rectangle to video rectangle
    _position[0] = _position[0] / this._contentRect.width * this._videoWidth;
    _position[1] = _position[1] / this._contentRect.height * this._videoHeight;

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
      case LetterBoxType.Horizontal: {
        const ratioWidth = this._rect.width / this._videoWidth;
        const height = this._videoHeight * ratioWidth;
        return (this._rect.height - height) * 0.5;
      }
      case LetterBoxType.Vertical: {
        const ratioHeight = this._rect.height / this._videoHeight;
        const width = this._videoWidth * ratioHeight;
        return (this._rect.width - width) * 0.5;
      }
    }
    throw 'invalid status';
  }

  /**
   * Returns rectangle for displaying video with the origin at the left-top of the element.
   * Not considered applying CSS like `object-fit`.
   * @returns {Object}
   */
  get contentRect() {
    const letterBoxType = this.letterBoxType;
    const letterBoxSize = this.letterBoxSize;

    const x = letterBoxType == LetterBoxType.Vertical ? letterBoxSize : 0;
    const y = letterBoxType == LetterBoxType.Horizontal ? letterBoxSize : 0;
    const width = letterBoxType == LetterBoxType.Vertical ? this._rect.width - letterBoxSize * 2 : this._rect.width;
    const height = letterBoxType == LetterBoxType.Horizontal ? this._rect.height - letterBoxSize * 2 : this._rect.height;

    return {x: x, y: y, width: width, height: height};
  }

  _reset() {
    this._contentRect = this.contentRect;
  }
}
