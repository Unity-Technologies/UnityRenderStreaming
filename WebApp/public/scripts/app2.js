import { SendVideo } from "./sendvideo.js";

const localVideo = document.getElementById('local_video');
const remoteVideo = document.getElementById('remote_video');
//const textForSendSdp = document.getElementById('text_for_send_sdp');
//const textToReceiveSdp = document.getElementById('text_for_receive_sdp');

let sendVideo = new SendVideo();

let startButton = document.getElementById('startVideoButton');
startButton.addEventListener('click', startVideo);
let setupButton = document.getElementById('setUpButton');
setupButton.addEventListener('click', setUp);
let callButton = document.getElementById('callButton');
callButton.addEventListener('click', call);
let hangUpButton = document.getElementById('hangUpButton');
hangUpButton.addEventListener('click', hangUp);


async function startVideo() {
    await sendVideo.startVideo(localVideo);
}

async function setUp() {
    await sendVideo.setupConnection(remoteVideo);
}

async function call() {
    await sendVideo.call();
}

function hangUp() {
    sendVideo.hangUp();
    remoteVideo.pause();
    remoteVideo.srcObject = null;
    // textForSendSdp.value = '';
    // textToReceiveSdp.value = '';
}