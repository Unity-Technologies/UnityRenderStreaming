import {
  VideoPlayer
} from "./video-player.js";

import {
  getServerConfig
} from "../../js/config.js";

import {
  createDisplayStringArray
} from "../../js/stats.js"

setup();

let playButton;
let videoPlayer;
let useWebSocket;

const playerDiv = document.getElementById('player');
const codecPreferences = document.getElementById('codecPreferences');
const supportsSetCodecPreferences = window.RTCRtpTransceiver &&
  'setCodecPreferences' in window.RTCRtpTransceiver.prototype;
const messageDiv = document.getElementById('message');
messageDiv.style.display = 'none';

window.document.oncontextmenu = function () {
  return false;     // cancel default menu
};

window.addEventListener('resize', function () {
  videoPlayer.resizeVideo();
}, true);

window.addEventListener('beforeunload', async () => {
  await videoPlayer.stop();
}, true);

async function setup() {
  const res = await getServerConfig();
  useWebSocket = res.useWebSocket;
  showWarningIfNeeded(res.startupMode);
  showCodecSelect();
  showPlayButton();
}

function showWarningIfNeeded(startupMode) {
  const warningDiv = document.getElementById("warning");
  if (startupMode == "private") {
    warningDiv.innerHTML = "<h4>Warning</h4> This sample is not working on Private Mode.";
    warningDiv.hidden = false;
  }
}

function showPlayButton() {
  if (!document.getElementById('playButton')) {
    let elementPlayButton = document.createElement('img');
    elementPlayButton.id = 'playButton';
    elementPlayButton.src = 'images/Play.png';
    elementPlayButton.alt = 'Start Streaming';
    playButton = document.getElementById('player').appendChild(elementPlayButton);
    playButton.addEventListener('click', onClickPlayButton);
  }
}

function onClickPlayButton() {
  playButton.style.display = 'none';

  // add video player
  const elementVideo = document.createElement('video');
  elementVideo.id = 'Video';
  elementVideo.style.touchAction = 'none';
  playerDiv.appendChild(elementVideo);

  setupVideoPlayer([elementVideo]).then(value => videoPlayer = value);

  // add fullscreen button
  const elementFullscreenButton = document.createElement('img');
  elementFullscreenButton.id = 'fullscreenButton';
  elementFullscreenButton.src = 'images/FullScreen.png';
  playerDiv.appendChild(elementFullscreenButton);
  elementFullscreenButton.addEventListener("click", function () {
    if (!document.fullscreenElement || !document.webkitFullscreenElement) {
      if (document.documentElement.requestFullscreen) {
        document.documentElement.requestFullscreen();
      }
      else if (document.documentElement.webkitRequestFullscreen) {
        document.documentElement.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT);
      } else {
        if (playerDiv.style.position == "absolute") {
          playerDiv.style.position = "relative";
        } else {
          playerDiv.style.position = "absolute";
        }
      }
    }
  });
  document.addEventListener('webkitfullscreenchange', onFullscreenChange);
  document.addEventListener('fullscreenchange', onFullscreenChange);

  function onFullscreenChange() {
    if (document.webkitFullscreenElement || document.fullscreenElement) {
      playerDiv.style.position = "absolute";
      elementFullscreenButton.style.display = 'none';
    }
    else {
      playerDiv.style.position = "relative";
      elementFullscreenButton.style.display = 'block';
    }
  }
}

async function setupVideoPlayer(elements) {
  const videoPlayer = new VideoPlayer(elements);

  let selectedCodecs = null;
  if (supportsSetCodecPreferences) {
    const preferredCodec = codecPreferences.options[codecPreferences.selectedIndex];
    if (preferredCodec.value !== '') {
      const [mimeType, sdpFmtpLine] = preferredCodec.value.split(' ');
      const { codecs } = RTCRtpSender.getCapabilities('video');
      const selectedCodecIndex = codecs.findIndex(c => c.mimeType === mimeType && c.sdpFmtpLine === sdpFmtpLine);
      const selectCodec = codecs[selectedCodecIndex];
      selectedCodecs = [selectCodec];
    }
  }
  codecPreferences.disabled = true;

  await videoPlayer.setupConnection(useWebSocket, selectedCodecs);
  videoPlayer.ondisconnect = onDisconnect;
  showStatsMessage();

  return videoPlayer;
}

async function onDisconnect(message) {
  clearStatsMessage();
  if (message) {
    messageDiv.style.display = 'block';
    messageDiv.innerText = message;
  }

  clearChildren(playerDiv);
  await videoPlayer.stop();
  videoPlayer = null;
  if (supportsSetCodecPreferences) {
    codecPreferences.disabled = false;
  }
  showPlayButton();
}

function clearChildren(element) {
  while (element.firstChild) {
    element.removeChild(element.firstChild);
  }
}

function showCodecSelect() {
  if (!supportsSetCodecPreferences) {
    messageDiv.style.display = 'block';
    messageDiv.innerHTML = `Current Browser does not support <a href="https://developer.mozilla.org/en-US/docs/Web/API/RTCRtpTransceiver/setCodecPreferences">RTCRtpTransceiver.setCodecPreferences</a>.`;
    return;
  }

  const codecs = RTCRtpSender.getCapabilities('video').codecs;
  codecs.forEach(codec => {
    if (['video/red', 'video/ulpfec', 'video/rtx'].includes(codec.mimeType)) {
      return;
    }
    const option = document.createElement('option');
    option.value = (codec.mimeType + ' ' + (codec.sdpFmtpLine || '')).trim();
    option.innerText = option.value;
    codecPreferences.appendChild(option);
  });
  codecPreferences.disabled = false;
}

let lastStats;
let intervalId;

function showStatsMessage() {
  intervalId = setInterval(async () => {
    if (videoPlayer == null) {
      return;
    }

    const stats = await videoPlayer.getStats();
    if (stats == null) {
      return;
    }

    const array = createDisplayStringArray(stats, lastStats);
    if (array.length) {
      messageDiv.style.display = 'block';
      messageDiv.innerHTML = array.join('<br>');
    }
    lastStats = stats;
  }, 1000);
}

function clearStatsMessage() {
  if (intervalId) {
    clearInterval(intervalId);
  }
  lastStats = null;
  intervalId = null;
  messageDiv.style.display = 'none';
  messageDiv.innerHTML = '';
}