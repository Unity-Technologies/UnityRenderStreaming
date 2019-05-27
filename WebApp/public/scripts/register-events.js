let InputEvent = {
  KeyDown : 0,
  KeyUp : 1,
  MouseDown : 2,
  MouseUp : 3,
  MouseMove : 4,
  MouseWheel : 5
};

let isPlayMode = false;

export function registerKeyboardEvents(videoPlayer) {
  const _videoPlayer = videoPlayer;
  document.addEventListener('keydown', function (e) {
    console.log("key down " + e.key + ", repeat = " + e.repeat);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.KeyDown, e.key]).buffer);
  }, false);
  document.addEventListener('keyup', function (e) {
    console.log("key up " + e.key);
    _videoPlayer && _videoPlayer.sendMsg(new Uint8Array([InputEvent.KeyUp, e.key]).buffer);
  }, false);
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
  playerElement.addEventListener('touchend', playVideoWithTouch , false);


  function pointerLockChange() {
    if (_document.pointerLockElement === playerElement ||
      _document.mozPointerLockElement === playerElement ||
      _document.webkitPointerLockElement === playerElement) {
      isPlayMode = false;
      console.log('Pointer locked');
      document.addEventListener('mousemove', sendMousePosition, false);
    }
    else {
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
  }
  function sendMousePosition(e) {
    console.log("deltaX: " + e.movementX + ", deltaY: " + e.movementY);
    let data = new DataView(new ArrayBuffer(5));
    data.setUint8(0, InputEvent.MouseMove);
    data.setInt16(1, e.movementX, true);
    data.setInt16(3, e.movementY, true);
    _videoPlayer.sendMsg(data.buffer);
  }
  function sendMouseDown(e) {
    console.log("mouse button " + e.button + " down");
    let data = new DataView(new ArrayBuffer(2));
    data.setUint8(0, InputEvent.MouseDown);
    data.setUint8(1, e.button);
    _videoPlayer && _videoPlayer.sendMsg(data.buffer);
  }
  function sendMouseUp(e) {
    console.log("mouse button " + e.button + " up");
    let data = new DataView(new ArrayBuffer(2));
    data.setUint8(0, InputEvent.MouseUp);
    data.setUint8(1, e.button);
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
