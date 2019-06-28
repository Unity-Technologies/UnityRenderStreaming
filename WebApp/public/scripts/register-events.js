let InputEvent = {
  Keyboard: 0,
  Mouse: 1,
  MouseWheel: 2,
  Touch: 3,
  ButtonClick: 4
};

let KeyboardEventType = {
  Up: 0,
  Down: 1
}

let PointerPhase = {
  None: 0,
  Began: 1,
  Moved: 2,
  Ended: 3,
  Canceled: 4,
  Stationary: 5
}

let isPlayMode = false;

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
    console.log("key down " + e.key + ", repeat = " + e.repeat);
    const key = e.key.charCodeAt(0);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.Keyboard, type, e.repeat, key]).buffer);
  }
}

export function registerMouseEvents(videoPlayer, playerElement) {
  const _videoPlayer = videoPlayer;
  const _playerElement = playerElement;
  const _document = document;
  playerElement.requestPointerLock = playerElement.requestPointerLock ||
    playerElement.mozRequestPointerLock || playerElement.webkitRequestPointerLock;

  // Listen to lock state change events
  document.addEventListener('pointerlockchange', pointerLockChange, false);
  document.addEventListener('mozpointerlockchange', pointerLockChange, false);
  document.addEventListener('webkitpointerlockchange', pointerLockChange, false);

  // Listen to mouse events
  playerElement.addEventListener('click', playVideo, false);
  playerElement.addEventListener('mousedown', sendMouse, false);
  playerElement.addEventListener('mouseup', sendMouse, false);
  playerElement.addEventListener('wheel', sendMouseWheel, false);

  // ios workaround for not allowing auto-play

  // Listen to touch events based on "Touch Events Level1" TR.
  //
  // Touch event Level1 https://www.w3.org/TR/touch-events/
  // Touch event Level2 https://w3c.github.io/touch-events/
  //
  playerElement.addEventListener('touchend', playVideoWithTouch, false);
  playerElement.addEventListener('touchstart', sendTouchStart, false);
  playerElement.addEventListener('touchcancel', sendTouchCancel, false);
  playerElement.addEventListener('touchmove', sendTouchMove, false);

  function pointerLockChange() {
    if (_document.pointerLockElement === playerElement ||
      _document.mozPointerLockElement === playerElement ||
      _document.webkitPointerLockElement === playerElement) {
      isPlayMode = false;
      console.log('Pointer locked');

      document.addEventListener('mousemove', sendMouse, false);
    } else {
      console.log('The pointer lock status is now unlocked');
      document.removeEventListener('mousemove', sendMouse, false);
    }
  }

  function playVideo() {
    if (_playerElement.paused) {
      _playerElement.play();
    }
    if (!isPlayMode) {
      _playerElement.requestPointerLock();
      isPlayMode = true;
    }
  }

  function playVideoWithTouch() {
    if (_playerElement.paused) {
      _playerElement.play();
    }
    isPlayMode = true;
    playerElement.removeEventListener('touchend', playVideoWithTouch);
    playerElement.addEventListener('touchend', sendTouchEnd, false);
  }

  function sendTouch(e, phase) {
    const changes = e.changedTouches;
    console.log("touchMove: length:" + changes.length + " pageX" + changes[0].pageX + ", pageX: " + changes[0].pageY + ", force:" + changes[0].force);

    let data = new DataView(new ArrayBuffer(3 + 8 * changes.length));
    data.setUint8(0, InputEvent.Touch);
    data.setUint8(1, phase);
    data.setUint8(2, changes.length);
    let byteOffset = 3;
    for (let i = 0; i < changes.length; i++) {
      data.setInt16(byteOffset, changes[i].pageX, true);
      byteOffset += 2;
      data.setInt16(byteOffset, changes[i].pageY, true);
      byteOffset += 2;
      data.setFloat32(byteOffset, changes[i].force, true);
      byteOffset += 4;
    }
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function sendTouchMove(e) {
    sendTouch(e, PointerPhase.Moved);
  }

  function sendTouchStart(e) {
    sendTouch(e, PointerPhase.Began);
  }

  function sendTouchEnd(e) {
    sendTouch(e, PointerPhase.Ended);
  }

  function sendTouchCancel(e) {
    sendTouch(e, PointerPhase.Canceled);
  }

  function sendMouse(e) {
    console.log("deltaX: " + e.movementX + ", deltaY: " + e.movementY + " mouse button:" + e.buttons);
    let data = new DataView(new ArrayBuffer(6));
    data.setUint8(0, InputEvent.Mouse);
    data.setInt16(1, e.movementX, true);
    data.setInt16(3, e.movementY, true);
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
