
import GatewaySocket from './GatewaySocket.js';

var socket = new GatewaySocket("ws://10.81.32.188/ws");
//var socket = new GatewaySocket("ws://10.86.34.255/ws");
var peerName = Math.random().toString(36).substring(2, 15) + Math.random().toString(36).substring(2, 15);

socket.addListener('connect', () => {
  socket.signIn(peerName);
});

socket.addListener('disconnect', () => {
  socket.isReady = false;
});

socket.addListener('reconnecting', () => {
});

socket.addListener('dispatch', (data) => {
  if (data.task == 'SIGN_IN' && data.status == 'SUCCESS') {
    socket.send({"type":"furioos","task":"SET_STREAM_PARAMS","protocol":"WebRTC","codec":"MP4_H264","fps":25,"quality":480,"ratio":3.449748743718593});
  }
  if (data.task == 'SET_STREAM_PARAMS' && data.status == 'SUCCESS') {
    socket.send({"type":"furioos","task":"START_STREAM"});
  }
  if (data.task == 'START_STREAM' && data.status == 'SUCCESS') {
    socket.isReady = true;

    if (_currentOffer != null) {
      socket.sendOffer(_currentOffer);

      _currentCandidates.forEach(candidate => {
        socket.sendCandidate(candidate);
      });
    }
  }

  if (data.type == "answer") {
    if (_answerCallback) {
      _answerCallback(data);
    }
  }

  if (data.candidate) {
    if (_candidateCallback) {
      _candidateCallback(data);
    }
  }
});

socket.connect();

var _currentOffer;
var _currentCandidates = [];
var _answerCallback;
var _candidateCallback;

export default class Signaling {
  static sendOffer(offer) {
    _currentOffer = offer;
    _currentCandidates = [];
    
    if (socket.isReady) {
        socket.sendOffer(offer);
    }
  };

  static sendCandidate(candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate' : candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
    };

    _currentCandidates.push(data);

    if (socket.isReady) {
      socket.sendCandidate(data);
    }
  };

  static onAnswer(answerCallback) {
    _answerCallback = answerCallback;
  }

  static onCandidate(candidateCallback) {
    _candidateCallback = candidateCallback;
  }
}
