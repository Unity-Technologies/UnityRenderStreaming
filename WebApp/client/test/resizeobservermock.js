// mock class

/* eslint-disable no-unused-vars */
let instanceResize = null;
/* eslint-disable no-unused-vars */
let callbackResize = null;

export default class ResizeObserverMock {
  constructor(callback) {
    instanceResize = this;
    callbackResize = callback;
  }
  disconnect() { }
  /* eslint-disable no-unused-vars */
  observe(target, options) { }
  /* eslint-disable no-unused-vars */
  unobserve(target) { }
}