import { SendVideo } from "./sendvideo.js";
import { getServerConfig } from "../../js/config.js";

const localVideo = document.getElementById('local_video');
const remoteVideo = document.getElementById('remote_video');
const textForConnectionId = document.getElementById('text_for_connection_id');

let sendVideo = new SendVideo();
let useWebSocket;

let startButton = document.getElementById('startVideoButton');
startButton.addEventListener('click', startVideo);
let setupButton = document.getElementById('setUpButton');
setupButton.addEventListener('click', setUp);
let hangUpButton = document.getElementById('hangUpButton');
hangUpButton.addEventListener('click', hangUp);

setupConfig();

async function setupConfig() {
  const res = await getServerConfig();
  useWebSocket = res.useWebSocket;
  showWarningIfNeeded(res.startupMode);
  showPlayButton();
}

function showWarningIfNeeded(startupMode) {
  const warningDiv = document.getElementById("warning");
  if (startupMode == "public") {
    warningDiv.innerHTML = "<h4>Warning</h4> This sample is not working on Public Mode."
    warningDiv.hidden = false;
  }
}

async function startVideo() {
  startButton.disabled = true;
  setupButton.disabled = false;
  await sendVideo.startVideo(localVideo);
}

async function setUp() {
  setupButton.disabled = true;
  hangUpButton.disabled = false;
  await sendVideo.setupConnection(remoteVideo, textForConnectionId.value, useWebSocket);
}

function hangUp() {
  hangUpButton.disabled = true;
  setupButton.disabled = false;
  sendVideo.hangUp();
  remoteVideo.pause();
  remoteVideo.srcObject = null;
}