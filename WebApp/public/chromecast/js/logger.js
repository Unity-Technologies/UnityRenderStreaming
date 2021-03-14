import {getCastDebugLogger} from "./receiver.js";

const LOG_TAG1 = 'Logger';

export function debug(msg) {
  getCastDebugLogger().debug(LOG_TAG1, msg);
}

export function info(msg) {
  getCastDebugLogger().info(LOG_TAG1, msg);
}

export function log(msg) {
  getCastDebugLogger().info(LOG_TAG1, msg);
}

export function warn(msg) {
  getCastDebugLogger().warn(LOG_TAG1, msg);
}

export function error(msg) {
  getCastDebugLogger().error(LOG_TAG1, msg);
}
