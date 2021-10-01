let isDebug = false;

export function enable() {
  isDebug = true;
}

export function disable() {
  isDebug = false;
}

export function debug(msg) {
  isDebug && console.debug(msg);
}

export function info(msg) {
  isDebug && console.info(msg);
}

export function log(msg) {
  isDebug && console.log(msg);
}

export function warn(msg) {
  isDebug && console.warn(msg);
}

export function error(msg) {
  isDebug && console.error(msg);
}
