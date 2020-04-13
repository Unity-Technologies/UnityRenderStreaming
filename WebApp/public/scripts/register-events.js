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
}

const GamepadEventType = {
  ButtonUp: 0,
  ButtonDown: 1,
  ButtonPressed: 2,
  Axis: 3
}

const PointerPhase = {
  None: 0,
  Began: 1,
  Moved: 2,
  Ended: 3,
  Canceled: 4,
  Stationary: 5
}

const Keymap = {
  "Space": 1,
  "Enter": 2,
  "Tab": 3,
  "Backquote": 4,
  "Quote": 5,
  "Semicolon": 6,
  "Comma": 7,
  "Period": 8,
  "Slash": 9,
  "Backslash": 10,
  "LeftBracket": 11,
  "RightBracket": 12,
  "Minus": 13,
  "Equals": 14,
  "KeyA": 15,
  "KeyB": 16,
  "KeyC": 17,
  "KeyD": 18,
  "KeyE": 19,
  "KeyF": 20,
  "KeyG": 21,
  "KeyH": 22,
  "KeyI": 23,
  "KeyJ": 24,
  "KeyK": 25,
  "KeyL": 26,
  "KeyM": 27,
  "KeyN": 28,
  "KeyO": 29,
  "KeyP": 30,
  "KeyQ": 31,
  "KeyR": 32,
  "KeyS": 33,
  "KeyT": 34,
  "KeyU": 35,
  "KeyV": 36,
  "KeyW": 37,
  "KeyX": 38,
  "KeyY": 39,
  "KeyZ": 40,
  "Digit1": 41,
  "Digit2": 42,
  "Digit3": 43,
  "Digit4": 44,
  "Digit5": 45,
  "Digit6": 46,
  "Digit7": 47,
  "Digit8": 48,
  "Digit9": 49,
  "Digit0": 50,
  "ShiftLeft": 51,
  "ShiftRight": 52,
  "AltLeft": 53,
  "AltRight": 54,
  // "AltGr": 54,
  "ControlLeft": 55,
  "ControlRight": 56,
  "MetaLeft": 57,
  "MetaRight": 58,
  // "LeftWindows": 57,
  // "RightWindows": 58,
  // "LeftApple": 57,
  // "RightApple": 58,
  // "LeftCommand": 57,
  // "RightCommand": 58,
  "ContextMenu": 59,
  "Escape": 60,
  "ArrowLeft": 61,
  "ArrowRight": 62,
  "ArrowUp": 63,
  "ArrowDown": 64,
  "Backspace": 65,
  "PageDown": 66,
  "PageUp": 67,
  "Home": 68,
  "End": 69,
  "Insert": 70,
  "Delete": 71,
  "CapsLock": 72,
  "NumLock": 73,
  "PrintScreen": 74,
  "ScrollLock": 75,
  "Pause": 76,
  "NumpadEnter": 77,
  "NumpadDivide": 78,
  "NumpadMultiply": 79,
  "NumpadPlus": 80,
  "NumpadMinus": 81,
  "NumpadPeriod": 82,
  "NumpadEquals": 83,
  "Numpad0": 84,
  "Numpad1": 85,
  "Numpad2": 86,
  "Numpad3": 87,
  "Numpad4": 88,
  "Numpad5": 89,
  "Numpad6": 90,
  "Numpad7": 91,
  "Numpad8": 92,
  "Numpad9": 93,
  "F1": 94,
  "F2": 95,
  "F3": 96,
  "F4": 97,
  "F5": 98,
  "F6": 99,
  "F7": 100,
  "F8": 101,
  "F9": 102,
  "F10": 103,
  "F11": 104,
  "F12": 105,
  // "OEM1": 106,
  // "OEM2": 107,
  // "OEM3": 108,
  // "OEM4": 109,
  // "OEM5": 110,
  // "IMESelected": 111,
};


let isPlayMode = false;
export function registerGamepadEvents(videoPlayer) {
  const _videoPlayer = videoPlayer;
  document.addEventListener("gamepadButtonDown", sendGamepadButtonDown, false);
  document.addEventListener("gamepadButtonUp", sendGamepadButtonUp, false);
  document.addEventListener("gamepadButtonPressed", sendGamepadButtonPressed, false);
  document.addEventListener("gamepadAxis", gamepadAxisChange, false);

  function sendGamepadButtonDown(e)
  {
    console.log("gamepad id: " + e.id + " button index: " + e.index + " value " + e.value + " down" );
    let data = new DataView(new ArrayBuffer(19));
    data.setUint8(0, InputEvent.Gamepad);
    data.setFloat64(1, e.id, true);
    data.setUint8(9, GamepadEventType.ButtonDown);
    data.setUint8(10, e.index);
    data.setFloat64(11, e.value, true);

    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function sendGamepadButtonUp(e)
  {
    console.log("gamepad id: " + e.id + " button index: " + e.index + " value " + e.value + " up" );
    let data = new DataView(new ArrayBuffer(19));
    data.setUint8(0, InputEvent.Gamepad);
    data.setFloat64(1, e.id, true);
    data.setUint8(9, GamepadEventType.ButtonUp);
    data.setUint8(10, e.index);
    data.setFloat64(11, e.value, true);

    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function sendGamepadButtonPressed(e)
  {
    console.log("gamepad id: " + e.id + " button index: " + e.index + " value " + e.value + " pressed" );
    let data = new DataView(new ArrayBuffer(19));
    data.setUint8(0, InputEvent.Gamepad);
    data.setFloat64(1, e.id, true);
    data.setUint8(9, GamepadEventType.ButtonPressed);
    data.setUint8(10, e.index);
    data.setFloat64(11, e.value, true);
    
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function gamepadAxisChange(e)
  {
    console.log("gamepad id: " + e.id + " axis: " + e.index + " value " + e.value + " x:" + e.x + " y:" + e.y );
    let data = new DataView(new ArrayBuffer(27));
    data.setUint8(0, InputEvent.Gamepad);  
    data.setFloat64(1, e.id, true);
    data.setUint8(9, GamepadEventType.Axis);  
    data.setUint8(10, e.index);
    data.setFloat64(11, e.x, true);
    data.setFloat64(19, e.y, true);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }
}


export function registerKeyboardEvents(videoPlayer) {
  const _videoPlayer = videoPlayer;
  document.addEventListener('keyup', sendKeyUp, false);
  document.addEventListener('keydown', sendKeyDown, false);

  function sendKeyUp(e) {
    sendKey(e, KeyboardEventType.Up);
  }

  function sendKeyDown(e) {
    sendKey(e, KeyboardEventType.Down);
  }

  function sendKey(e, type) {
    const key = Keymap[e.code];
    const character = e.key.length === 1 ? e.key.charCodeAt(0) : 0;
    console.log("key down " + key + ", repeat = " + e.repeat + ", character = " + character);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.Keyboard, type, e.repeat, key, character]).buffer);
  }
}

export function registerMouseEvents(videoPlayer, playerElement) {
  const _videoPlayer = videoPlayer;

  // Listen to mouse events
  playerElement.addEventListener('click', sendMouse, false);
  playerElement.addEventListener('mousedown', sendMouse, false);
  playerElement.addEventListener('mouseup', sendMouse, false);
  playerElement.addEventListener('mousemove', sendMouse, false);
  playerElement.addEventListener('wheel', sendMouseWheel, false);

  // ios workaround for not allowing auto-play

  // Listen to touch events based on "Touch Events Level1" TR.
  //
  // Touch event Level1 https://www.w3.org/TR/touch-events/
  // Touch event Level2 https://w3c.github.io/touch-events/
  //
  playerElement.addEventListener('touchend', sendTouchEnd, false);
  playerElement.addEventListener('touchstart', sendTouchStart, false);
  playerElement.addEventListener('touchcancel', sendTouchCancel, false);
  playerElement.addEventListener('touchmove', sendTouchMove, false);

  function sendTouch(e, phase) {
    const changedTouches = Array.from(e.changedTouches);
    const touches = Array.from(e.touches);
    const phrases = [];

    for (let i = 0; i < changedTouches.length; i++) {
      if (touches.find(function (t) {
        return t.identifier === changedTouches[i].identifier
      }) === undefined) {
        touches.push(changedTouches[i]);
      }
    }

    for (let i = 0; i < touches.length; i++) {
      touches[i].identifier;
      phrases[i] = changedTouches.find(
        function (e) {
          return e.identifier === touches[i].identifier
        }) === undefined ? PointerPhase.Stationary : phase;
    }

    console.log("touch phase:" + phase + " length:" + changedTouches.length + " pageX" + changedTouches[0].pageX + ", pageX: " + changedTouches[0].pageY + ", force:" + changedTouches[0].force);

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

  function sendTouchMove(e) {
    sendTouch(e, PointerPhase.Moved);
    e.preventDefault();
  }

  function sendTouchStart(e) {
    sendTouch(e, PointerPhase.Began);
    e.preventDefault();
  }

  function sendTouchEnd(e) {
    sendTouch(e, PointerPhase.Ended);
    e.preventDefault();
  }

  function sendTouchCancel(e) {
    sendTouch(e, PointerPhase.Canceled);
    e.preventDefault();
  }

  function sendMouse(e) {
    const scale = _videoPlayer.videoScale;
    const originX = _videoPlayer.videoOriginX;
    const originY = _videoPlayer.videoOriginY;

    const x = (e.clientX - originX) / scale;
    // According to Unity Coordinate system
    // const y = (e.clientY - originY) / scale;
    const y = _videoPlayer.videoHeight - (e.clientY - originY) / scale;

    console.log("x: " + x + ", y: " + y + ", scale: " + scale + ", originX: " + originX + ", originY: " + originY + " mouse button:" + e.buttons);
    let data = new DataView(new ArrayBuffer(6));
    data.setUint8(0, InputEvent.Mouse);
    data.setInt16(1, x, true);
    data.setInt16(3, y, true);
    data.setUint8(5, e.buttons);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function sendMouseWheel(e) {
    console.log("mouse wheel with delta " + e.wheelDelta);
    let data = new DataView(new ArrayBuffer(9));
    data.setUint8(0, InputEvent.MouseWheel);
    data.setFloat32(1, e.deltaX, true);
    data.setFloat32(5, e.deltaY, true);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }
}

export function sendClickEvent(videoPlayer, elementId) {
  let data = new DataView(new ArrayBuffer(3));
  data.setUint8(0, InputEvent.ButtonClick);
  data.setInt16(1, elementId, true);
  videoPlayer && videoPlayer.sendMsg(data.buffer);
}