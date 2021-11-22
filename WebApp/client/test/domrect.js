// mock class

export class DOMRect {
  constructor(x, y, width, height) {
    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;
  }
  get left() {
    return this.x;
  }
  get top() {
    return this.y;
  }
}