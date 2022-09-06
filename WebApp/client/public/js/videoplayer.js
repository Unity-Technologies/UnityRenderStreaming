import { Observer, Sender } from "../module/sender.js";
import { InputRemoting } from "../module/inputremoting.js";

export class VideoPlayer {
  constructor() {
    this.playerElement = null;
    this.lockMouseCheck = null;
    this.videoElement = null;
    this.fullScreenButtonElement = null;
    this.inputRemoting = null;
    this.sender = null;
    this.inputSenderChannel = null;
  }

  /**
 * @param {Element} playerElement parent element for create video player
 * @param {HTMLInputElement} lockMouseCheck use checked propety for lock mouse 
 */
  createPlayer(playerElement, lockMouseCheck) {
    this.playerElement = playerElement;
    this.lockMouseCheck = lockMouseCheck;

    this.videoElement = document.createElement('video');
    this.videoElement.id = 'Video';
    this.videoElement.style.touchAction = 'none';
    this.videoElement.playsInline = true;
    this.videoElement.srcObject = new MediaStream();
    this.videoElement.addEventListener('loadedmetadata', this._onLoadedVideo.bind(this), true);
    this.playerElement.appendChild(this.videoElement);

    // add fullscreen button
    this.fullScreenButtonElement = document.createElement('img');
    this.fullScreenButtonElement.id = 'fullscreenButton';
    this.fullScreenButtonElement.src = '../images/FullScreen.png';
    this.fullScreenButtonElement.addEventListener("click", this._onClickFullscreenButton.bind(this));
    this.playerElement.appendChild(this.fullScreenButtonElement);

    document.addEventListener('webkitfullscreenchange', this._onFullscreenChange.bind(this));
    document.addEventListener('fullscreenchange', this._onFullscreenChange.bind(this));
    this.videoElement.addEventListener("click", this._mouseClick.bind(this), false);
  }

  _onLoadedVideo() {
    this.videoElement.play();
    this.resizeVideo();
  }

  _onClickFullscreenButton() {
    if (!document.fullscreenElement || !document.webkitFullscreenElement) {
      if (document.documentElement.requestFullscreen) {
        document.documentElement.requestFullscreen();
      }
      else if (document.documentElement.webkitRequestFullscreen) {
        document.documentElement.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT);
      } else {
        if (this.playerElement.style.position == "absolute") {
          this.playerElement.style.position = "relative";
        } else {
          this.playerElement.style.position = "absolute";
        }
      }
    }
  }

  _onFullscreenChange() {
    if (document.webkitFullscreenElement || document.fullscreenElement) {
      this.playerElement.style.position = "absolute";
      this.fullScreenButtonElement.style.display = 'none';

      if (this.lockMouseCheck.checked) {
        if (document.webkitFullscreenElement.requestPointerLock) {
          document.webkitFullscreenElement.requestPointerLock();
        } else if (document.fullscreenElement.requestPointerLock) {
          document.fullscreenElement.requestPointerLock();
        } else if (document.mozFullScreenElement.requestPointerLock) {
          document.mozFullScreenElement.requestPointerLock();
        }

        // Subscribe to events
        document.addEventListener('mousemove', this._mouseMove.bind(this), false);
        document.addEventListener('click', this._mouseClickFullScreen.bind(this), false);
      }
    }
    else {
      this.playerElement.style.position = "relative";
      this.fullScreenButtonElement.style.display = 'block';

      document.removeEventListener('mousemove', this._mouseMove.bind(this), false);
      document.removeEventListener('click', this._mouseClickFullScreen.bind(this), false);
    }
  }

  _mouseMove(event) {
    // Forward mouseMove event of fullscreen player directly to sender
    // This is required, as the regular mousemove event doesn't fire when in fullscreen mode
    this.sender._onMouseEvent(event);
  }

  _mouseClick() {
    // Restores pointer lock when we unfocus the player and click on it again
    if (this.lockMouseCheck.checked) {
      if (this.videoElement.requestPointerLock) {
        this.videoElement.requestPointerLock().catch(function () { });
      }
    }
  }

  _mouseClickFullScreen() {
    // Restores pointer lock when we unfocus the fullscreen player and click on it again
    if (this.lockMouseCheck.checked) {
      if (document.webkitFullscreenElement.requestPointerLock) {
        document.webkitFullscreenElement.requestPointerLock();
      } else if (document.fullscreenElement.requestPointerLock) {
        document.fullscreenElement.requestPointerLock();
      } else if (document.mozFullScreenElement.requestPointerLock) {
        document.mozFullScreenElement.requestPointerLock();
      }
    }
  }

  /**
   * @param {MediaStreamTrack} track 
   */
  addTrack(track) {
    if (!this.videoElement.srcObject) {
      return;
    }

    this.videoElement.srcObject.addTrack(track);
  }

  resizeVideo() {
    if (!this.videoElement) {
      return;
    }

    const clientRect = this.videoElement.getBoundingClientRect();
    const videoRatio = this.videoWidth / this.videoHeight;
    const clientRatio = clientRect.width / clientRect.height;

    this._videoScale = videoRatio > clientRatio ? clientRect.width / this.videoWidth : clientRect.height / this.videoHeight;
    const videoOffsetX = videoRatio > clientRatio ? 0 : (clientRect.width - this.videoWidth * this._videoScale) * 0.5;
    const videoOffsetY = videoRatio > clientRatio ? (clientRect.height - this.videoHeight * this._videoScale) * 0.5 : 0;
    this._videoOriginX = clientRect.left + videoOffsetX;
    this._videoOriginY = clientRect.top + videoOffsetY;
  }

  get videoWidth() {
    return this.videoElement.videoWidth;
  }

  get videoHeight() {
    return this.videoElement.videoHeight;
  }

  get videoOriginX() {
    return this._videoOriginX;
  }

  get videoOriginY() {
    return this._videoOriginY;
  }

  get videoScale() {
    return this._videoScale;
  }

  deletePlayer() {
    if (this.inputRemoting) {
      this.inputRemoting.stopSending();
    }
    this.inputRemoting = null;
    this.sender = null;
    this.inputSenderChannel = null;

    while (this.playerElement.firstChild) {
      this.playerElement.removeChild(this.playerElement.firstChild);
    }

    this.playerElement = null;
    this.lockMouseCheck = null;
  }

  _isTouchDevice() {
    return (('ontouchstart' in window) ||
      (navigator.maxTouchPoints > 0) ||
      (navigator.msMaxTouchPoints > 0));
  }

  /**
   * setup datachannel for player input (muouse/keyboard/touch/gamepad)
   * @param {RTCDataChannel} channel 
   */
  setupInput(channel) {
    this.sender = new Sender(this.videoElement);
    this.sender.addMouse();
    this.sender.addKeyboard();
    if (this._isTouchDevice()) {
      this.sender.addTouchscreen();
    }
    this.sender.addGamepad();
    this.inputRemoting = new InputRemoting(this.sender);

    this.inputSenderChannel = channel;
    this.inputSenderChannel.onopen = this._onOpenInputSenderChannel.bind(this);
    this.inputRemoting.subscribe(new Observer(this.inputSenderChannel));
  }

  async _onOpenInputSenderChannel() {
    await new Promise(resolve => setTimeout(resolve, 100));
    this.inputRemoting.startSending();
  }
}