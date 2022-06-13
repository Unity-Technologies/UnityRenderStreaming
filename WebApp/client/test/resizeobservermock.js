// mock class

let instanceResize = null;
let callbackResize = null;

export default class ResizeObserverMock {
  constructor(callback) {
    instanceResize = this;
    callbackResize = callback;
  }
  disconnect() { }
  observe(target, options) { }
  unobserve(target) { }
}