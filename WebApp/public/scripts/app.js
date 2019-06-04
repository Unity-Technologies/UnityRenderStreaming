import { VideoPlayer } from "./video-player.js";
import { registerKeyboardEvents, registerMouseEvents } from "./register-events.js";

let playButton;

showPlayButton();

function showPlayButton() {
  if (!document.getElementById('playButton')) {
    let playButtonImg = document.createElement('img');
    playButtonImg.id = 'playButton';
    playButtonImg.src = 'images/Play.png';
    playButtonImg.alt = 'Start Streaming';
    if (!playButton) {
      playButton = document.getElementById('player').appendChild(playButtonImg);
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
  const element = document.createElement('video');
  element.style.touchAction = 'none';
  playerDiv.appendChild(element);
  const videoPlayer = setupVideoPlayer(element);
  registerKeyboardEvents(videoPlayer);
  registerMouseEvents(videoPlayer, element);
}

function setupVideoPlayer(element, clientConfig) {
  let videoPlayer = new VideoPlayer(element, clientConfig);
  videoPlayer.setupConnection();
  return videoPlayer;
}
