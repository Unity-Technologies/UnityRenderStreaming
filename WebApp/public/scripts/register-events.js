

export function registerKeyboardEvents(videoPlayer) {
    
    //Listen to keyboard events
    document.addEventListener('keyup', videoPlayer.sendKeyUp, false);
    document.addEventListener('keydown', videoPlayer.sendKeyDown, false);
}

export function unregisterKeyboardEvents(videoPlayer) {

    //Stop listening to keyboard events
    document.removeEventListener('keyup', videoPlayer.sendKeyUp, false);
    document.removeEventListener('keydown', videoPlayer.sendKeyDown, false);
}

export function registerMouseEvents(videoPlayer, playerElement) {

    // Listen to mouse events
    playerElement.addEventListener('click', videoPlayer.sendMouse, false);
    playerElement.addEventListener('mousedown', videoPlayer.sendMouse, false);
    playerElement.addEventListener('mouseup', videoPlayer.sendMouse, false);
    playerElement.addEventListener('mousemove', videoPlayer.sendMouse, false);
    playerElement.addEventListener('wheel', videoPlayer.sendMouseWheel, false);

    // Listen to touch events based on "Touch Events Level1" TR.
    //
    // Touch event Level1 https://www.w3.org/TR/touch-events/
    // Touch event Level2 https://w3c.github.io/touch-events/
    //
    playerElement.addEventListener('touchend', videoPlayer.sendTouchEnd, false);
    playerElement.addEventListener('touchstart', videoPlayer.sendTouchStart, false);
    playerElement.addEventListener('touchcancel', videoPlayer.sendTouchCancel, false);
    playerElement.addEventListener('touchmove', videoPlayer.sendTouchMove, false);

}

export function unregisterMouseEvents(videoPlayer, playerElement) {

      // Stop listening to mouse events
      playerElement.removeEventListener('click', videoPlayer.sendMouse, false);
      playerElement.removeEventListener('mousedown', videoPlayer.sendMouse, false);
      playerElement.removeEventListener('mouseup', videoPlayer.sendMouse, false);
      playerElement.removeEventListener('mousemove', videoPlayer.sendMouse, false);
      playerElement.removeEventListener('wheel', videoPlayer.sendMouseWheel, false);

      // Stop listening to touch events based on "Touch Events Level1" TR.
      playerElement.removeEventListener('touchend', videoPlayer.sendTouchEnd, false);
      playerElement.removeEventListener('touchstart', videoPlayer.sendTouchStart, false);
      playerElement.removeEventListener('touchcancel', videoPlayer.sendTouchCancel, false);
      playerElement.removeEventListener('touchmove', videoPlayer.sendTouchMove, false);

}

