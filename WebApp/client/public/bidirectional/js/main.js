import { SendVideo } from "./sendvideo.js";
import { getServerConfig } from "../../js/config.js";

const localVideo = document.getElementById('local_video');
const remoteVideo = document.getElementById('remote_video');
const textForConnectionId = document.getElementById('text_for_connection_id');
textForConnectionId.value = getRandom();
let videoSelect = document.querySelector('select#videoSource');
let audioSelect = document.querySelector('select#audioSource');

const codecPreferences = document.getElementById('codecPreferences');
const supportsSetCodecPreferences = window.RTCRtpTransceiver &&
  'setCodecPreferences' in window.RTCRtpTransceiver.prototype;
const messageDiv = document.getElementById('message');
messageDiv.style.display = 'none';

setUpInputSelect();
showCodecSelect();
showStatsMessage();

let sendVideo = new SendVideo();
sendVideo.ondisconnect = async (message) => {
  if (message) {
    messageDiv.style.display = 'block';
    messageDiv.innerText = message;
  }

  await hangUp();
};

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

  await sendVideo.setupConnection(remoteVideo, connectionId, useWebSocket, selectedCodecs);
}

async function hangUp() {
  hangUpButton.disabled = true;
  setupButton.disabled = false;
  await sendVideo.hangUp(connectionId);
  textForConnectionId.value = getRandom();
  connectionId = null;
  if (supportsSetCodecPreferences) {
    codecPreferences.disabled = false;
  }
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

function showStatsMessage() {
  setInterval(async () => {
    if (sendVideo == null || connectionId == null) {
      return;
    }

    const stats = await sendVideo.getStats(connectionId);
    if (stats == null) {
      return;
    }

    let message = "";
    stats.forEach(stat => {
      if (stat.type === 'inbound-rtp' && stat.kind === 'video' && stat.codecId !== undefined) {
        const codec = stats.get(stat.codecId);
        message += `Using for receive video ${codec.mimeType} ${codec.sdpFmtpLine}, payloadType=${codec.payloadType}. Decoder: ${stat.decoderImplementation} \n`;
      }
      if (stat.type === 'outbound-rtp' && stat.kind === 'video' && stat.codecId !== undefined) {
        const codec = stats.get(stat.codecId);
        message += `Using for send video ${codec.mimeType} ${codec.sdpFmtpLine}, payloadType=${codec.payloadType}. Encoder: ${stat.encoderImplementation} \n`;
      }
    });

    if (message != "") {
      messageDiv.style.display = 'block';
      messageDiv.innerText = message;
    }
  }, 1000);
}