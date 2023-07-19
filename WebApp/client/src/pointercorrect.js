export const LetterBoxType = {
  Vertical: 0,
  Horizontal: 1
};

export class PointerCorrector {
  /**
   * @param {Number} videoWidth
   * @param {Number} videoHeight
   * @param {HTMLVideoElement} videoElem
   */
  constructor(videoWidth, videoHeight, videoElem) {
    this.reset(videoWidth, videoHeight, videoElem);
  }

  /**
   * @param {Number[]} position MouseEvent.clientX, MouseEvent.clientY
   * @returns {Number[]}
   */
   map(position) {
    var rect = this._videoElem.getBoundingClientRect();
    const _position = new Array(2);

    // (1) set origin point to zero
    _position[0] = position[0] - rect.left;
    _position[1] = position[1] - rect.top;

    // (2) translate Unity coordinate system (reverse y-axis)
    _position[1] = rect.height - _position[1];

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
   * @param {HTMLVideoElement} videoElem
   */
  setRect(videoElem) {
    this._videoElem = videoElem;
    this._reset();
  }

  /**
   * @param {Number} videoWidth
   * @param {Number} videoHeight
   * @param {HTMLVideoElement} videoElem
   */
  reset(videoWidth, videoHeight, videoElem) {
    this._videoWidth = videoWidth;
    this._videoHeight = videoHeight;
    this._videoElem = videoElem;
    this._reset();
  }

  get letterBoxType() {
    const videoRatio = this._videoHeight / this._videoWidth;
    var rect = this._videoElem.getBoundingClientRect();
    const rectRatio = rect.height / rect.width;
    return videoRatio > rectRatio ? LetterBoxType.Vertical : LetterBoxType.Horizontal;
  }

  get letterBoxSize() {
    var rect = this._videoElem.getBoundingClientRect();
    switch(this.letterBoxType) {
      case LetterBoxType.Horizontal: {
        const ratioWidth = rect.width / this._videoWidth;
        const height = this._videoHeight * ratioWidth;
        return (rect.height - height) * 0.5;
      }
      case LetterBoxType.Vertical: {
        const ratioHeight = rect.height / this._videoHeight;
        const width = this._videoWidth * ratioHeight;
        return (rect.width - width) * 0.5;
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

    var rect = this._videoElem.getBoundingClientRect();

    const x = letterBoxType == LetterBoxType.Vertical ? letterBoxSize : 0;
    const y = letterBoxType == LetterBoxType.Horizontal ? letterBoxSize : 0;
    const width = letterBoxType == LetterBoxType.Vertical ? rect.width - letterBoxSize * 2 : rect.width;
    const height = letterBoxType == LetterBoxType.Horizontal ? rect.height - letterBoxSize * 2 : rect.height;

    return {x: x, y: y, width: width, height: height};
  }

  _reset() {
    this._contentRect = this.contentRect;
  }
}
