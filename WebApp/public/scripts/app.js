import { VideoPlayer } from "./video-player.js";
import { registerKeyboardEvents, registerMouseEvents, sendClickEvent } from "./register-events.js";

let playButton;

showPlayButton();

function showPlayButton() {
  if (!document.getElementById('playButton')) {
    let elementPlayButton = document.createElement('img');
    elementPlayButton.id = 'playButton';
    elementPlayButton.src = 'images/Play.png';
    elementPlayButton.alt = 'Start Streaming';
    if (!playButton) {
      playButton = document.getElementById('player').appendChild(elementPlayButton);
    }
    playButton.addEventListener('click', function () {
      onClickPlayButton();
      playButton.style.display = 'none';
    });
  }
  else {
    playButton.style.display = 'block';
  }
}

function onClickPlayButton() {
  const playerDiv = document.getElementById('player');

  // add video player
  const elementVideo = document.createElement('video');
  elementVideo.id = 'Video';
  elementVideo.style.touchAction = 'none';
  playerDiv.appendChild(elementVideo);
  const videoPlayer = setupVideoPlayer(elementVideo);
  registerKeyboardEvents(videoPlayer);
  registerMouseEvents(videoPlayer, elementVideo);

  // add green button sample
  const elementBlueButton = document.createElement('button');
  elementBlueButton.id = "blueButton";
  elementBlueButton.innerHTML = "Light on";
  playerDiv.appendChild(elementBlueButton);
  elementBlueButton.addEventListener ("click", function() {
    sendClickEvent(videoPlayer, 1);
  });

  // add green button sample
  const elementGreenButton = document.createElement('button');
  elementGreenButton.id = "greenButton";
  elementGreenButton.innerHTML = "Light off";
  playerDiv.appendChild(elementGreenButton);
  elementGreenButton.addEventListener ("click", function() {
    sendClickEvent(videoPlayer, 2);
  });
}

function setupVideoPlayer(element, clientConfig) {
  let videoPlayer = new VideoPlayer(element, clientConfig);
  videoPlayer.setupConnection();
  return videoPlayer;
}
