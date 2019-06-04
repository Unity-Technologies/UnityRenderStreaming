let InputEvent = {
  KeyDown: 0,
  MouseButton: 2,
  MouseMove: 4,
  MouseWheel: 5,
  Touch: 6
};

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
  document.addEventListener('keydown', function (e) {
    const key = e.key.charCodeAt(0);
    console.log("key down " + key + ", repeat = " + e.repeat);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.KeyDown, key]).buffer);
  }, false);

  /*
  document.addEventListener('keyup', function (e) {
    console.log("key up " + e.key);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.KeyUp, e.key]).buffer);
  }, false);
  */
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
  playerElement.addEventListener('mousedown', sendMouseDown, false);
  playerElement.addEventListener('mouseup', sendMouseUp, false);
  playerElement.addEventListener('mousewheel', sendMouseWheel, false);
  // ios workaround for not allowing auto-play
  playerElement.addEventListener('touchend', playVideoWithTouch, false);
  playerElement.addEventListener('touchstart', sendTouchStart, false);
  playerElement.addEventListener('touchcancel', sendTouchCancel, false);
  playerElement.addEventListener('touchend', sendTouchEnd, false);
  playerElement.addEventListener('touchmove', sendTouchMove, false);

  function pointerLockChange() {
    if (_document.pointerLockElement === playerElement ||
      _document.mozPointerLockElement === playerElement ||
      _document.webkitPointerLockElement === playerElement) {
      isPlayMode = false;
      console.log('Pointer locked');
      document.addEventListener('mousemove', sendMousePosition, false);
    } else {
      console.log('The pointer lock status is now unlocked');
      document.removeEventListener('mousemove', sendMousePosition, false);
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
    _videoPlayer.sendMsg(data.buffer);
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

  function sendMousePosition(e) {
    console.log("deltaX: " + e.movementX + ", deltaY: " + e.movementY + " mouse button:" + e.buttons);
    let data = new DataView(new ArrayBuffer(6));
    data.setUint8(0, InputEvent.MouseMove);
    data.setInt16(1, e.movementX, true);
    data.setInt16(3, e.movementY, true);
    data.setUint8(5, e.buttons);
    _videoPlayer.sendMsg(data.buffer);
  }

  function sendMouseDown(e) {
    console.log("mouse button " + e.buttons + " down");
    let data = new DataView(new ArrayBuffer(2));

    data.setUint8(0, InputEvent.MouseButton);
    data.setUint8(1, e.buttons);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function sendMouseUp(e) {
    console.log("mouse button " + e.buttons + " up");
    let data = new DataView(new ArrayBuffer(2));
    data.setUint8(0, InputEvent.MouseButton);
    data.setUint8(1, e.buttons);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }

  function sendMouseWheel(e) {
    console.log("mouse wheel with delta " + e.wheelDelta);
    let data = new DataView(new ArrayBuffer(3));
    data.setUint8(0, InputEvent.MouseWheel);
    data.setInt16(1, e.wheelDelta, true);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }
}
