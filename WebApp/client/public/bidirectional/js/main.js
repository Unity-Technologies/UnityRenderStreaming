import { SendVideo } from "./sendvideo.js";
import { getServerConfig } from "../../js/config.js";

const localVideo = document.getElementById('local_video');
const remoteVideo = document.getElementById('remote_video');
const textForConnectionId = document.getElementById('text_for_connection_id');
textForConnectionId.value = getRandom();
let videoSelect = document.querySelector('select#videoSource');
let audioSelect = document.querySelector('select#audioSource');

setUpInputSelect();

let sendVideo = new SendVideo();
sendVideo.ondisconnect = () => hangUp();

let useWebSocket;
let connectionId;

let startButton = document.getElementById('startVideoButton');
startButton.addEventListener('click', startVideo);
let setupButton = document.getElementById('setUpButton');
setupButton.addEventListener('click', setUp);
let hangUpButton = document.getElementById('hangUpButton');
hangUpButton.addEventListener('click', hangUp);

window.addEventListener('beforeunload', async () => {
  await sendVideo.stop();
}, true);

setupConfig();

async function setupConfig() {
  const res = await getServerConfig();
  useWebSocket = res.useWebSocket;
  showWarningIfNeeded(res.startupMode);
}

function showWarningIfNeeded(startupMode) {
  const warningDiv = document.getElementById("warning");
  if (startupMode == "public") {
    warningDiv.innerHTML = "<h4>Warning</h4> This sample is not working on Public Mode.";
    warningDiv.hidden = false;
  }
}

async function startVideo() {
  videoSelect.disabled = true;
  audioSelect.disabled = true;
  startButton.disabled = true;
  setupButton.disabled = false;
  await sendVideo.startVideo(localVideo, videoSelect.value, audioSelect.value);
}

async function setUp() {
  setupButton.disabled = true;
  hangUpButton.disabled = false;
  connectionId = textForConnectionId.value;
  await sendVideo.setupConnection(remoteVideo, connectionId, useWebSocket);
}

function hangUp() {
  hangUpButton.disabled = true;
  setupButton.disabled = false;
  sendVideo.hangUp(connectionId);
  textForConnectionId.value = getRandom();
  connectionId = null;
}

function getRandom() {
  const max = 99999;
  const length = String(max).length;
  const number = Math.floor(Math.random() * max);
  return (Array(length).join('0') + number).slice(-length);
}

async function setUpInputSelect() {
  const deviceInfos = await navigator.mediaDevices.enumerateDevices();

  for (let i = 0; i !== deviceInfos.length; ++i) {
    const deviceInfo = deviceInfos[i];
    if (deviceInfo.kind === 'videoinput') {
      const option = document.createElement('option');
      option.value = deviceInfo.deviceId;  
      option.text = deviceInfo.label || `camera ${videoSelect.length + 1}`;
      videoSelect.appendChild(option);
    } else if (deviceInfo.kind === 'audioinput') {
      const option = document.createElement('option');
      option.value = deviceInfo.deviceId;  
      option.text = deviceInfo.label || `mic ${audioSelect.length + 1}`;
      audioSelect.appendChild(option);
    }
  }
}