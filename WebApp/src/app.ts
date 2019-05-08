let videoPlayer: VideoPlayer = null;
let socket: SocketIOClient.Socket;
const logInput: boolean = true;
let playButton: HTMLElement;
let isPlayMode: boolean = false;
let isDebug: boolean = true;
let customDocument: CustomDocument = document;

enum LogLevel{
    info,
    log,
    warn,
    error,
}
function log(level: LogLevel, ...args: any[]): void {
  if (isDebug && window.console) {
    switch (level){
      case LogLevel.log:
        console.log(...args);
        break;
      case LogLevel.info:
        console.info(...args);
        break;
      case LogLevel.warn:
        console.warn(...args);
        break;
      case LogLevel.error:
        console.error(...args);
        break;
    }
  }
}

enum InputEvent
{
  KeyDown = 0,
  KeyUp,
  MouseDown,
  MouseUp,
  MouseMove,
  MouseWheel,
}

interface RTCConfig extends RTCConfiguration{
  sdpSemantics?: string;
}
interface VideoElement extends HTMLVideoElement{
  playsInline?: boolean;
  requestPointerLock?: () => void;
  mozRequestPointerLock?: () => void;
  webkitRequestPointerLock?: () => void;
}
interface MouseWheelEv extends WheelEvent{
  wheelDelta?: number;
}

interface CustomDocument extends Document{
  pointerLockElement?: HTMLElement;
  mozPointerLockElement?: HTMLElement;
  webkitPointerLockElement?: HTMLElement;
}
class VideoPlayer{

  public pc: RTCPeerConnection;
  public channel: RTCDataChannel;
  public offerOptions: RTCOfferOptions;
  public cfg: RTCConfig;
  public video: VideoElement;

  constructor(options: RTCConfig) {
    this.cfg = options;
    this.cfg.sdpSemantics = 'unified-plan';
    this.cfg.iceServers = [{ urls: ['stun:stun.l.google.com:19302'] }];
    this.pc = null;
    this.channel = null;
    this.offerOptions = {
      offerToReceiveAudio: true,
      offerToReceiveVideo: true,
    };
    this.video = document.createElement('video');
    this.video.id = 'Video';
    this.video.playsInline = true;
    this.video.addEventListener('loadedmetadata', () => {
      {
        this.video.play();
      }
    },                          true);
  }
  public onReceiveCandidate(iceCandidate: RTCIceCandidateInit): void {
    log(LogLevel.log, 'Received candidate');
    const candidate: RTCIceCandidate = new RTCIceCandidate(iceCandidate);
    this.pc.addIceCandidate(candidate).then(() => {
      log(LogLevel.log, 'Added ice candidate');
    });
  }
  public onReceiveAnswer(answer: RTCSessionDescriptionInit): void {
    log(LogLevel.log, 'Received answer', answer);
    const answerDesc: RTCSessionDescription = new RTCSessionDescription(answer);
    this.pc.setRemoteDescription(answerDesc);
  }

  public close(): void {
    if (this.pc) {
      log(LogLevel.log, 'Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }
  }

  public setupConnection(): void {
    // close current RTCPeerConnection
    if (this.pc) {
      log(LogLevel.log, 'Close current PeerConnection');
      this.pc.close();
      this.pc = null;
    }
    // Create peerConnection with proxy server and set up handlers
    this.pc = new RTCPeerConnection(this.cfg);
    this.pc.onsignalingstatechange = (e) => {
      log(LogLevel.log, 'signalingState changed:', e);
    };
    this.pc.oniceconnectionstatechange = (e) => {
      log(LogLevel.log, 'iceConnectionState changed:', e);
    };
    this.pc.onicegatheringstatechange = (e) => {
      log(LogLevel.log, 'iceGatheringState changed:', e);
    };
    this.pc.ontrack = (e) => {
      log(LogLevel.log, 'New track added: ', e.streams);
      this.video.srcObject = e.streams[0];
    };
    this.pc.onicecandidate = (e) => {
      log(LogLevel.log, 'Send ICE candidate', e);
      e.candidate && socket.emit('iceCandidate', JSON.stringify(e.candidate));
    };
    // Create data channel with proxy server and set up handlers
    this.channel = this.pc.createDataChannel('ProxyDataChannel');
    log(LogLevel.log, 'Create datachannel.');
    this.channel.onopen = () => {
      log(LogLevel.log, 'Datachannel connected.');
    };
    this.channel.onerror = (e) => {
      log(LogLevel.error, `The error ${e.error.message} occurred
                            while handling data with proxy server.`);
    };
    this.channel.onclose = () => {
      log(LogLevel.log, 'Datachannel disconnected.');
    };
    // create offer and sendMsg to proxy server
    this.pc.createOffer(this.offerOptions).then((offer) => {
      offer.sdp = offer.sdp.replace(/useinbandfec=1/,
                                    'useinbandfec=1;stereo=1;maxaveragebitrate=1048576');
      this.pc.setLocalDescription(offer);
      socket.emit('offer', JSON.stringify(offer));
    }).catch((reason) => {
      log(LogLevel.error, `The error ${reason.message} occurred when creating offer.`);
    });
  }

  public sendMsg(msg: string | ArrayBuffer | ArrayBufferView): void {
    switch (this.channel.readyState) {
      case 'connecting':
        log(LogLevel.log, 'Connection not ready');
        break;
      case 'open':
        this.channel.send(msg);
        break;
      case 'closing':
        log(LogLevel.warn, 'Attempt to sendMsg message while closing');
        break;
      case 'closed':
        log(LogLevel.warn, 'Attempt to sendMsg message while connection closed.');
        break;
    }
  }
}

function load(): void {
  registerKeyboardEvents();
  showPlayButton();
}

function registerKeyboardEvents(): void {
  document.addEventListener('keydown', (e) => {
    logInput && log(LogLevel.log, `key down ${e.keyCode}, repeat = ${e.repeat}`);
    videoPlayer && videoPlayer.sendMsg(new Uint8Array([InputEvent.KeyDown, e.keyCode]).buffer);
  },                        false);
  document.addEventListener('keyup', (e) => {
    logInput && log(LogLevel.log, `key up ${e.keyCode}`);
    videoPlayer && videoPlayer.sendMsg(new Uint8Array([InputEvent.KeyUp, e.keyCode]).buffer);
  },                        false);
}

function showPlayButton(): void {
  if (!document.getElementById('playButton')) {
    const playButtonImg: HTMLImageElement = document.createElement('img');
    playButtonImg.id = 'playButton';
    playButtonImg.src = 'images/Play.png';
    playButtonImg.alt = 'Start Streaming';
    if (!playButton) {
      playButton = document.getElementById('player').appendChild(playButtonImg);
    }
    playButton.addEventListener('click', () => {
      setupSignalingConnection();
      playButton.style.display = 'none';
    });
  } else {
    playButton.style.display = 'block';
  }

}

// connect to signaling server
function setupSignalingConnection(): void {
  socket = io();
  socket.on('clientConfig', (rtcConfig: RTCConfig) => {
    onRtcConfig(rtcConfig);
  });

  socket.on('connect', () => {
    log(LogLevel.log, 'Connected to signaling server');
  });

  socket.on('error', (error: any) => {
    log(LogLevel.error, `signaling server connection error ${error}`);
  });

  socket.on('disconnect', (reason: any) => {
    log(LogLevel.log, `signaling server connection is closed: ${reason}`);
    socket.disconnect();
        // remove existing video
    const playerDiv: HTMLElement = document.getElementById('player');
    if (videoPlayer) {
      playerDiv.removeChild(videoPlayer.video);
      videoPlayer.close();
      videoPlayer = undefined;
    }
    showPlayButton();
  });
}

// receive proxy config data from signaling server
function onRtcConfig(clientConfig: RTCConfig): void {
  log(LogLevel.log, `clientConfig = ${JSON.stringify(clientConfig)}`);
  const playerDiv: HTMLElement = document.getElementById('player');
  const playerElement: VideoElement = setupVideoPlayer(playerDiv, clientConfig);
  registerMouseEvents(playerElement);
}

function setupVideoPlayer(htmlElement: HTMLElement, clientConfig: RTCConfig): VideoElement {
  videoPlayer = new VideoPlayer(clientConfig);
  htmlElement.appendChild(videoPlayer.video);

  socket.on('answer', (webRTCData: RTCSessionDescriptionInit) => {
    videoPlayer.onReceiveAnswer(webRTCData);
  });

  socket.on('iceCandidate', (iceCandidate: RTCIceCandidateInit) => {
    if (videoPlayer) {
      videoPlayer.onReceiveCandidate(iceCandidate);
    }
  });

  videoPlayer.setupConnection();
  return videoPlayer.video;
}

function sendMouseDown(e: MouseEvent): void {
  logInput && log(LogLevel.log, `mouse button ${e.button} down`);
  const data = new DataView(new ArrayBuffer(2));
  data.setUint8(0, InputEvent.MouseDown);
  data.setUint8(1, e.button);
  videoPlayer && videoPlayer.sendMsg(data.buffer);
}

function sendMouseUp(e: MouseEvent): void {
  logInput && log(LogLevel.log, `mouse button ${e.button} up`);
  const data = new DataView(new ArrayBuffer(2));
  data.setUint8(0, InputEvent.MouseUp);
  data.setUint8(1, e.button);
  videoPlayer && videoPlayer.sendMsg(data.buffer);
}

function sendMouseWheel(e: MouseWheelEv): void {
  logInput && log(LogLevel.log, `mouse wheel with delta ${e.wheelDelta}`);
  const data = new DataView(new ArrayBuffer(3));
  data.setUint8(0, InputEvent.MouseWheel);
  data.setInt16(1, e.wheelDelta, true);
  videoPlayer && videoPlayer.sendMsg(data.buffer);
}

function registerMouseEvents(playerElement: VideoElement): void {
  playerElement.requestPointerLock = playerElement.requestPointerLock ||
      playerElement.mozRequestPointerLock || playerElement.webkitRequestPointerLock;
    // Listen to lock state change events
  document.addEventListener('pointerlockchange', pointerLockChange, false);
  document.addEventListener('mozpointerlockchange', pointerLockChange, false);
  document.addEventListener('webkitpointerlockchange', pointerLockChange, false);
    // Listen to mouse events
  playerElement.addEventListener('click', () => {
    if (videoPlayer.video.paused) {
      videoPlayer.video.play();
    }
    if (!isPlayMode) {
      playerElement.requestPointerLock();
      isPlayMode = true;
    }
  },                             false);
  playerElement.addEventListener('mousedown', sendMouseDown, false);
  playerElement.addEventListener('mouseup', sendMouseUp, false);
  playerElement.addEventListener('mousewheel', sendMouseWheel, false);
    // ios workaround for not allowing auto-play
  playerElement.addEventListener('touchend', () => {
    if (videoPlayer.video.paused) { videoPlayer.video.play(); }
  },                             false);

  function pointerLockChange(): void {
    if (customDocument.pointerLockElement === playerElement ||
      customDocument.mozPointerLockElement === playerElement ||
      customDocument.webkitPointerLockElement === playerElement) {
      isPlayMode = false;
      log(LogLevel.log, 'Pointer locked');
      document.addEventListener('mousemove', sendMousePosition, false);
    } else {
      log(LogLevel.log, 'The pointer lock status is now unlocked');
      document.removeEventListener('mousemove', sendMousePosition, false);
    }
  }

  function sendMousePosition(e: MouseEvent): void {
    logInput && log(LogLevel.log, `deltaX: ${e.movementX}, deltaY: ${e.movementY}`);
    const data = new DataView(new ArrayBuffer(5));
    data.setUint8(0, InputEvent.MouseMove);
    data.setInt16(1, e.movementX, true);
    data.setInt16(3, e.movementY, true);
    videoPlayer && videoPlayer.sendMsg(data.buffer);
  }
}
