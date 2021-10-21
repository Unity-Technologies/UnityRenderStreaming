import { gamepadHandler } from "./gamepadEvents.js";
import * as Logger from "./logger.js";
import { Keymap } from "./keymap.js";

const InputEvent = {
  Keyboard: 0,
  Mouse: 1,
  MouseWheel: 2,
  Touch: 3,
  ButtonClick: 4,
  Gamepad: 5
};

const KeyboardEventType = {
  Up: 0,
  Down: 1
};

const GamepadEventType = {
  ButtonUp: 0,
  ButtonDown: 1,
  ButtonPressed: 2,
  Axis: 3
};

const PointerPhase = {
  None: 0,
  Began: 1,
  Moved: 2,
  Ended: 3,
  Canceled: 4,
  Stationary: 5
};

let sendGamepadButtonDown = undefined;
let sendGamepadButtonUp = undefined;
let sendGamepadButtonPressed;
let gamepadAxisChange = undefined;
let gamepadConnected = undefined;
let gamepadDisconnected = undefined;

export function registerGamepadEvents(videoPlayer) {

  const _videoPlayer = videoPlayer;

  sendGamepadButtonDown = (e) => {
    Logger.log("gamepad id: " + e.id + " button index: " + e.index + " value " + e.value + " down");
    let data = new DataView(new ArrayBuffer(19));
    data.setUint8(0, InputEvent.Gamepad);
    data.setUint8(1, GamepadEventType.ButtonDown);
    data.setUint8(2, e.index);
    data.setFloat64(3, e.value, true);

    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  };

  sendGamepadButtonUp = (e) => {
    Logger.log("gamepad id: " + e.id + " button index: " + e.index + " value " + e.value + " up");
    let data = new DataView(new ArrayBuffer(19));
    data.setUint8(0, InputEvent.Gamepad);
    data.setUint8(1, GamepadEventType.ButtonUp);
    data.setUint8(2, e.index);
    data.setFloat64(3, e.value, true);

    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  };

  sendGamepadButtonPressed = (e) => {
    Logger.log("gamepad id: " + e.id + " button index: " + e.index + " value " + e.value + " pressed");
    let data = new DataView(new ArrayBuffer(19));
    data.setUint8(0, InputEvent.Gamepad);
    data.setUint8(1, GamepadEventType.ButtonPressed);
    data.setUint8(2, e.index);
    data.setFloat64(3, e.value, true);

    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  };

  gamepadAxisChange = (e) => {
    Logger.log("gamepad id: " + e.id + " axis: " + e.index + " value " + e.value + " x:" + e.x + " y:" + e.y);
    let data = new DataView(new ArrayBuffer(27));
    data.setUint8(0, InputEvent.Gamepad);
    data.setUint8(1, GamepadEventType.Axis);
    data.setUint8(2, e.index);
    data.setFloat64(3, e.x, true);
    data.setFloat64(11, e.y, true);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  };

  gamepadConnected = (e) => { gamepadHandler(e, true); };
  gamepadDisconnected = (e) => { gamepadHandler(e, false); };


  document.addEventListener("gamepadButtonDown", sendGamepadButtonDown, false);
  document.addEventListener("gamepadButtonUp", sendGamepadButtonUp, false);
  document.addEventListener("gamepadButtonPressed", sendGamepadButtonPressed, false);
  document.addEventListener("gamepadAxis", gamepadAxisChange, false);

  window.addEventListener("gamepadconnected", gamepadConnected, false);
  window.addEventListener("gamepaddisconnected", gamepadDisconnected, false);
}

export function unregisterGamepadEvents() {

  document.removeEventListener("gamepadButtonDown", sendGamepadButtonDown, false);
  document.removeEventListener("gamepadButtonUp", sendGamepadButtonUp, false);
  document.removeEventListener("gamepadButtonPressed", sendGamepadButtonPressed, false);
  document.removeEventListener("gamepadAxis", gamepadAxisChange, false);

  window.removeEventListener("gamepadconnected", gamepadConnected, false);
  window.removeEventListener("gamepaddisconnected", gamepadDisconnected, false);

}


let sendKeyUp = undefined;
let sendKeyDown = undefined;


export function registerKeyboardEvents(videoPlayer) {

  const _videoPlayer = videoPlayer;

  function sendKey(e, type) {
    const key = Keymap[e.code];
    const character = e.key.length === 1 ? e.key.charCodeAt(0) : 0;
    Logger.log("key down " + key + ", repeat = " + e.repeat + ", character = " + character);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.Keyboard, type, e.repeat, key, character]).buffer);
  }


  sendKeyUp = (e) => {
    sendKey(e, KeyboardEventType.Up);
  };

  sendKeyDown = (e) => {
    sendKey(e, KeyboardEventType.Down);
  };

  document.addEventListener('keyup', sendKeyUp, false);
  document.addEventListener('keydown', sendKeyDown, false);
}


export function unregisterKeyboardEvents() {

  //Stop listening to keyboard events
  document.removeEventListener('keyup', sendKeyUp, false);
  document.removeEventListener('keydown', sendKeyDown, false);
}


let sendMouse = undefined;
let sendMouseWheel = undefined;
let sendTouchEnd = undefined;
let sendTouchStart = undefined;
let sendTouchCancel = undefined;
let sendTouchMove = undefined;


export function registerMouseEvents(videoPlayer, playerElement) {

  const _videoPlayer = videoPlayer;

  function sendTouch(e, phase) {
    const changedTouches = Array.from(e.changedTouches);
    const touches = Array.from(e.touches);
    const phrases = [];

    for (let i = 0; i < changedTouches.length; i++) {
      if (touches.find(function (t) {
        return t.identifier === changedTouches[i].identifier;
      }) === undefined) {
        touches.push(changedTouches[i]);
      }
    }

    for (let i = 0; i < touches.length; i++) {
      touches[i].identifier;
      phrases[i] = changedTouches.find(
        function (e) {
          return e.identifier === touches[i].identifier;
        }) === undefined ? PointerPhase.Stationary : phase;
    }

    Logger.log("touch phase:" + phase + " length:" + changedTouches.length + " pageX" + changedTouches[0].pageX + ", pageX: " + changedTouches[0].pageY + ", force:" + changedTouches[0].force);

    let data = new DataView(new ArrayBuffer(2 + 13 * touches.length));
    data.setUint8(0, InputEvent.Touch);
    data.setUint8(1, touches.length);
    let byteOffset = 2;
    for (let i = 0; i < touches.length; i++) {

      const scale = _videoPlayer.videoScale;
      const originX = _videoPlayer.videoOriginX;
      const originY = _videoPlayer.videoOriginY;

      const x = (touches[i].pageX - originX) / scale;
      // According to Unity Coordinate system
      // const y = (touches[i].pageX - originY) / scale;
      const y = _videoPlayer.videoHeight - (touches[i].pageY - originY) / scale;

      data.setInt32(byteOffset, touches[i].identifier, true);
      byteOffset += 4;
      data.setUint8(byteOffset, phrases[i]);
      byteOffset += 1;
      data.setInt16(byteOffset, x, true);
      byteOffset += 2;
      data.setInt16(byteOffset, y, true);
      byteOffset += 2;
      data.setFloat32(byteOffset, touches[i].force, true);
      byteOffset += 4;
    }
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  sendTouchMove = (e) => {
    sendTouch(e, PointerPhase.Moved);
    e.preventDefault();
  };

  sendTouchStart = (e) => {
    sendTouch(e, PointerPhase.Began);
    e.preventDefault();
  };

  sendTouchEnd = (e) => {
    sendTouch(e, PointerPhase.Ended);
    e.preventDefault();
  };

  sendTouchCancel = (e) => {
    sendTouch(e, PointerPhase.Canceled);
    e.preventDefault();
  };

  sendMouse = (e) => {
    const scale = _videoPlayer.videoScale;
    const originX = _videoPlayer.videoOriginX;
    const originY = _videoPlayer.videoOriginY;

    const x = (e.clientX - originX) / scale;
    // According to Unity Coordinate system
    // const y = (e.clientY - originY) / scale;
    const y = _videoPlayer.videoHeight - (e.clientY - originY) / scale;

    Logger.log("x: " + x + ", y: " + y + ", scale: " + scale + ", originX: " + originX + ", originY: " + originY + " mouse button:" + e.buttons);
    let data = new DataView(new ArrayBuffer(6));
    data.setUint8(0, InputEvent.Mouse);
    data.setInt16(1, x, true);
    data.setInt16(3, y, true);
    data.setUint8(5, e.buttons);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  };

  function sendMouseWheel(e) {
    Logger.log("mouse wheel with delta " + e.wheelDelta);
    let data = new DataView(new ArrayBuffer(9));
    data.setUint8(0, InputEvent.MouseWheel);
    data.setFloat32(1, e.deltaX, true);
    data.setFloat32(5, e.deltaY, true);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  // Listen to mouse events
  playerElement.addEventListener('click', sendMouse, false);
  playerElement.addEventListener('mousedown', sendMouse, false);
  playerElement.addEventListener('mouseup', sendMouse, false);
  playerElement.addEventListener('mousemove', sendMouse, false);
  playerElement.addEventListener('wheel', sendMouseWheel, false);

  // Listen to touch events based on "Touch Events Level1" TR.
  //
  // Touch event Level1 https://www.w3.org/TR/touch-events/
  // Touch event Level2 https://w3c.github.io/touch-events/
  //
  playerElement.addEventListener('touchend', sendTouchEnd, false);
  playerElement.addEventListener('touchstart', sendTouchStart, false);
  playerElement.addEventListener('touchcancel', sendTouchCancel, false);
  playerElement.addEventListener('touchmove', sendTouchMove, false);
}


export function unregisterMouseEvents(playerElement) {

  // Stop listening to mouse events
  playerElement.removeEventListener('click', sendMouse, false);
  playerElement.removeEventListener('mousedown', sendMouse, false);
  playerElement.removeEventListener('mouseup', sendMouse, false);
  playerElement.removeEventListener('mousemove', sendMouse, false);
  playerElement.removeEventListener('wheel', sendMouseWheel, false);

  // Stop listening to touch events based on "Touch Events Level1" TR.
  playerElement.removeEventListener('touchend', sendTouchEnd, false);
  playerElement.removeEventListener('touchstart', sendTouchStart, false);
  playerElement.removeEventListener('touchcancel', sendTouchCancel, false);
  playerElement.removeEventListener('touchmove', sendTouchMove, false);

}


export function sendClickEvent(videoPlayer, elementId) {
  let data = new DataView(new ArrayBuffer(3));
  data.setUint8(0, InputEvent.ButtonClick);
  data.setInt16(1, elementId, true);
  videoPlayer && videoPlayer.sendMsg(data.buffer);
}
