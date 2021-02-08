import { SendVideo } from "./sendvideo.js";

const localVideo = document.getElementById('local_video');
const remoteVideo = document.getElementById('remote_video');
const textForConnectionId = document.getElementById('text_for_connection_id');

let sendVideo = new SendVideo();

let startButton = document.getElementById('startVideoButton');
startButton.addEventListener('click', startVideo);
let setupButton = document.getElementById('setUpButton');
setupButton.addEventListener('click', setUp);
let hangUpButton = document.getElementById('hangUpButton');
hangUpButton.addEventListener('click', hangUp);


async function startVideo() {
  await sendVideo.startVideo(localVideo);
}

async function setUp() {
  await sendVideo.setupConnection(remoteVideo, textForConnectionId.value);
}

function hangUp() {
  sendVideo.hangUp();
  remoteVideo.pause();
  remoteVideo.srcObject = null;
}