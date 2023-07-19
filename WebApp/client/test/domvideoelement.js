// mock class

export class DOMHTMLVideoElement {
  constructor(rect) {
    this.rect = rect;
  }

  getBoundingClientRect() {
    return this.rect;
  }
}
