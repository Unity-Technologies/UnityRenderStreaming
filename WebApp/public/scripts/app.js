import { VideoPlayer } from "./video-player.js";
import { registerKeyboardEvents, registerMouseEvents, sendClickEvent } from "./register-events.js";

let playButton;
let videoPlayer;

showPlayButton();

function showPlayButton() {
  if (!document.getElementById('playButton')) {
    let elementPlayButton = document.createElement('img');
    elementPlayButton.id = 'playButton';
    elementPlayButton.src = 'images/Play.png';
    elementPlayButton.alt = 'Start Streaming';
    playButton = document.getElementById('player').appendChild(elementPlayButton);
    playButton.addEventListener('click', function () {
      onClickPlayButton();
      playButton.style.display = 'none';
    });
  }
}

function onClickPlayButton() {
  const playerDiv = document.getElementById('player');

  // add video player
  const elementVideo = document.createElement('video');
  elementVideo.id = 'Video';
  elementVideo.style.touchAction = 'none';
  playerDiv.appendChild(elementVideo);
  videoPlayer = setupVideoPlayer(elementVideo);
  registerKeyboardEvents(videoPlayer);
  registerMouseEvents(videoPlayer, elementVideo);

  // add green button
  const elementBlueButton = document.createElement('button');
  elementBlueButton.id = "blueButton";
  elementBlueButton.innerHTML = "Light on";
  playerDiv.appendChild(elementBlueButton);
  elementBlueButton.addEventListener ("click", function() {
    sendClickEvent(videoPlayer, 1);
  });

  // add green button
  const elementGreenButton = document.createElement('button');
  elementGreenButton.id = "greenButton";
  elementGreenButton.innerHTML = "Light off";
  playerDiv.appendChild(elementGreenButton);
  elementGreenButton.addEventListener ("click", function() {
    sendClickEvent(videoPlayer, 2);
  });

  // add fullscreen button
  const elementFullscreenButton = document.createElement('img');
  elementFullscreenButton.id = 'fullscreenButton';
  elementFullscreenButton.src = 'images/FullScreen.png';
  playerDiv.appendChild(elementFullscreenButton);
  elementFullscreenButton.addEventListener ("click", function() {
    if (!document.fullscreenElement) {
      if(document.documentElement.requestFullscreen) {
        document.documentElement.requestFullscreen();
      }
      else if(document.documentElement.webkitRequestFullscreen){
        document.documentElement.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT);
      }
    }
  });
  document.addEventListener('webkitfullscreenchange', onFullscreenChange);
  document.addEventListener('fullscreenchange', onFullscreenChange);

  function onFullscreenChange(e) {
    if(document.webkitFullscreenElement || document.fullscreenElement) {
      elementFullscreenButton.style.display = 'none';
    }
    else {
      elementFullscreenButton.style.display = 'block';
    }
  }
}

function setupVideoPlayer(element, config) {
  const videoPlayer = new VideoPlayer(element, config);
  videoPlayer.setupConnection();
  videoPlayer.ondisconnect = onDisconnect;
  return videoPlayer;
}

function onDisconnect() {
  const playerDiv = document.getElementById('player')
  clearChildren(playerDiv);
  videoPlayer = null;
  showPlayButton();
}

function clearChildren(element) {
  while (element.firstChild) {
    element.removeChild(element.firstChild);
  }
}
