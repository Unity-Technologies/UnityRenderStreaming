'use strict';

import Signaling, { WebSocketSignaling } from "../../js/signaling.js";
import * as Config from "../../js/config.js";
import * as Logger from "../../js/logger.js";
import { getServerConfig } from "../../js/config.js";

const audioInputSelect = document.querySelector('select#audioSource');
const callButton = document.querySelector('button#callButton');
const cfg = Config.getRTCConfiguration();

hangupButton.disabled = true;
callButton.onclick = call;
hangupButton.onclick = hangup;

let pc;
let localStream;
let signaling;

function uuid4() {
  var temp_url = URL.createObjectURL(new Blob());
  var uuid = temp_url.toString();
  URL.revokeObjectURL(temp_url);
  return uuid.split(/[:\/]/g).pop().toLowerCase(); // remove prefixes
}

function gotDevices(deviceInfos) {
  while (audioInputSelect.firstChild) {
    select.removeChild(select.firstChild);
  }
  for (let i = 0; i !== deviceInfos.length; ++i) {
    const deviceInfo = deviceInfos[i];
    const option = document.createElement('option');
    option.value = deviceInfo.deviceId;
    if (deviceInfo.kind === 'audioinput') {
      option.text = deviceInfo.label || `microphone ${audioInputSelect.length + 1}`;
      audioInputSelect.appendChild(option);
    }
  }
}

function gotStream(stream) {
  localStream = stream;  
  const audioTracks = localStream.getAudioTracks();
  if (audioTracks.length > 0) {
    console.log(`Using Audio device: ${audioTracks[0].label}`);
  }

  localStream.getTracks().forEach(track => pc1.addTrack(track, localStream));

  // Refresh button list in case labels have become available
  return navigator.mediaDevices.enumerateDevices();
}

function handleError(error) {
  console.log('navigator.MediaDevices.getUserMedia error: ', error.message, error.name);
}

navigator.mediaDevices.enumerateDevices().then(gotDevices).catch(handleError);


async function call() {
  callButton.disabled = true;
  console.log('Starting call');
  pc = new RTCPeerConnection(cfg);

  pc.onsignalingstatechange = function (e) {
    Logger.log('signalingState changed:', e);
  };
  pc.oniceconnectionstatechange = function (e) {
    Logger.log('iceConnectionState changed:', e);
    Logger.log('pc.iceConnectionState:' + pc.iceConnectionState);
    if (pc.iceConnectionState === 'disconnected') {
      ondisconnect();
    }
  };
  pc.onicegatheringstatechange = function (e) {
    Logger.log('iceGatheringState changed:', e);
  };

  const connectionId = uuid4();

  // create offer
  const offer = await pc.createOffer();
  
  // set local sdp
  const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
  await pc.setLocalDescription(desc);
  await signaling.sendOffer(connectionId, offer.sdp);  
}

function hangup() {
  console.log('Ending call');
  localStream.getTracks().forEach(track => track.stop());
  pc1.close();
  pc2.close();
  pc1 = null;
  pc2 = null;
  hangupButton.disabled = true;
  callButton.disabled = false;
  codecSelector.disabled = false;
}

function start() {
  if (window.stream) {
    window.stream.getTracks().forEach(track => {
      track.stop();
    });
  }
  const audioSource = audioInputSelect.value;
  const constraints = {
    audio: {deviceId: audioSource ? {exact: audioSource} : undefined},
  };
  navigator.mediaDevices.getUserMedia(constraints).then(gotStream).then(gotDevices).catch(handleError);
}
audioInputSelect.onchange = start;

const res = await getServerConfig();
const useWebSocket = res.useWebSocket;
if (useWebSocket) {
  signaling = new WebSocketSignaling();
} else {
  signaling = new Signaling();
}

/*
signaling.addEventListener('offer', async (e) => {
  const offer = e.detail;
  if (pc == null) {
    //_this.prepareNewPeerConnection(offer.connectionId, offer.polite);
    //addTracks(offer.connectionId);
  }
  const desc = new RTCSessionDescription({ sdp: offer.sdp, type: "offer" });
  await pc.onGotDescription(offer.connectionId, desc);
});
*/

signaling.addEventListener('answer', async (e) => {
  const answer = e.detail;
  const desc = new RTCSessionDescription({ sdp: answer.sdp, type: "answer" });
  if (pc != null) {
    await pc.setRemoteDescription(desc);
  }
});

signaling.addEventListener('candidate', async (e) => {
  const candidate = e.detail;
  const iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
  if (pc != null) {
    await pc.addIceCandidate(iceCandidate);
  }
});

await signaling.start();

start();