import { Receiver } from "../../js/receiver.js";
import { RenderStreaming } from "../../js/renderstreaming.js";
import { getServerConfig } from "../../js/config.js";
import { createDisplayStringArray } from "../../js/stats.js";
import { Observer, Sender } from "../../js/sender.js";
import { InputRemoting } from "../../js/inputremoting.js";

/** @enum {number} */
const ActionType = {
  ChangeLabel: 0
};

let playButton;
let receiver;
let renderstreaming;
let useWebSocket;
let elementVideo;

let sender;
let inputRemoting;
let inputSenderChannel;
let multiplayChannel;

const playerDiv = document.getElementById('player');
const codecPreferences = document.getElementById('codecPreferences');
const lockMouseCheck = document.getElementById('lockMouseCheck');
const supportsSetCodecPreferences = window.RTCRtpTransceiver &&
  'setCodecPreferences' in window.RTCRtpTransceiver.prototype;
const messageDiv = document.getElementById('message');
messageDiv.style.display = 'none';

setup();

window.document.oncontextmenu = function () {
  return false;     // cancel default menu
};

window.addEventListener('resize', function () {
  receiver.resizeVideo();
}, true);

window.addEventListener('beforeunload', async () => {
  await renderstreaming.stop();
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
    elementPlayButton.src = '../images/Play.png';
    elementPlayButton.alt = 'Start Streaming';
    playButton = document.getElementById('player').appendChild(elementPlayButton);
    playButton.addEventListener('click', onClickPlayButton);
  }
}

function onClickPlayButton() {

  playButton.style.display = 'none';

  // add video player
  elementVideo = document.createElement('video');
  elementVideo.id = 'Video';
  elementVideo.style.touchAction = 'none';
  playerDiv.appendChild(elementVideo);
  receiver = new Receiver(elementVideo);

  setupRenderStreaming();

  // add fullscreen button
  const elementFullscreenButton = document.createElement('img');
  elementFullscreenButton.id = 'fullscreenButton';
  elementFullscreenButton.src = '../images/FullScreen.png';
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

  elementVideo.addEventListener("click", _mouseClick, false);

  function onFullscreenChange() {
    if (document.webkitFullscreenElement || document.fullscreenElement) {
      playerDiv.style.position = "absolute";
      elementFullscreenButton.style.display = 'none';

      if (lockMouseCheck.checked) {
        if (document.webkitFullscreenElement.requestPointerLock) {
          document.webkitFullscreenElement.requestPointerLock();
        } else if (document.fullscreenElement.requestPointerLock) {
          document.fullscreenElement.requestPointerLock();
        } else if (document.mozFullScreenElement.requestPointerLock) {
          document.mozFullScreenElement.requestPointerLock();
        }

        // Subscribe to events
        document.addEventListener('mousemove', _mouseMove, false);
        document.addEventListener('click', _mouseClickFullScreen, false);
      }
    }
    else {
      playerDiv.style.position = "relative";
      elementFullscreenButton.style.display = 'block';

      document.removeEventListener('mousemove', _mouseMove, false);
      document.removeEventListener('click', _mouseClickFullScreen, false);
    }
  }

  function _mouseMove(event) {
    // Forward mouseMove event of fullscreen player directly to sender
    // This is required, as the regular mousemove event doesn't fire when in fullscreen mode
    sender._onMouseEvent(event);
  }

  function _mouseClick() {
    // Restores pointer lock when we unfocus the player and click on it again
    if (lockMouseCheck.checked) {
      if (elementVideo.requestPointerLock) {
        elementVideo.requestPointerLock().catch(function () { });
      }
    }
  }

  function _mouseClickFullScreen() {
    // Restores pointer lock when we unfocus the fullscreen player and click on it again
    if (lockMouseCheck.checked) {
      if (document.webkitFullscreenElement.requestPointerLock) {
        document.webkitFullscreenElement.requestPointerLock();
      } else if (document.fullscreenElement.requestPointerLock) {
        document.fullscreenElement.requestPointerLock();
      } else if (document.mozFullScreenElement.requestPointerLock) {
        document.mozFullScreenElement.requestPointerLock();
      }
    }
  }
}

async function setupRenderStreaming() {
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

  renderstreaming = new RenderStreaming(useWebSocket, selectedCodecs);
  renderstreaming.onConnect = onConnect;
  renderstreaming.onDisconnect = onDisconnect;
  renderstreaming.onTrackEvent = (data) =>{
    receiver.addTrack(data.track);
  };

  await renderstreaming.start();
  await renderstreaming.createConnection();
}

function onConnect() {
  setupInput();
  showStatsMessage();
}

async function onDisconnect(connectionId) {
  clearStatsMessage();
  messageDiv.style.display = 'block';
  messageDiv.innerText = `Disconnect peer on ${connectionId}.`;

  clearChildren(playerDiv);
  await renderstreaming.stop();
  renderstreaming = null;
  sender = null;
  inputRemoting = null;
  inputSenderChannel = null;
  multiplayChannel = null;
  receiver = null;
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

function isTouchDevice() {
  return (('ontouchstart' in window) ||
    (navigator.maxTouchPoints > 0) ||
    (navigator.msMaxTouchPoints > 0));
}

function setupInput() {
  sender = new Sender(elementVideo);
  sender.addMouse();
  sender.addKeyboard();
  if (isTouchDevice()) {
    sender.addTouchscreen();
  }
  sender.addGamepad();
  inputRemoting = new InputRemoting(sender);

  inputSenderChannel = renderstreaming.createDataChannel("input");
  inputSenderChannel.onopen = _onOpenInputSenderChannel;
  inputRemoting.subscribe(new Observer(inputSenderChannel));

  multiplayChannel = renderstreaming.createDataChannel("multiplay");
  multiplayChannel.onopen = _onOpenMultiplayChannel;
}

async function _onOpenMultiplayChannel() {
  await new Promise(resolve => setTimeout(resolve, 100));
  const num = Math.floor(Math.random() * 100000);
  _changeLabel(String(num));
}

async function _onOpenInputSenderChannel() {
  await new Promise(resolve => setTimeout(resolve, 100));
  inputRemoting.startSending();
}

function _changeLabel(label) {
  const json = JSON.stringify({ type: ActionType.ChangeLabel, argument: label });
  multiplayChannel.send(json);
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
    if (renderstreaming == null) {
      return;
    }

    const stats = await renderstreaming.getStats();
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