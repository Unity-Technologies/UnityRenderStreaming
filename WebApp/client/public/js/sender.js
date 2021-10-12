import { 
  Mouse,
  Keyboard,
  Gamepad,
  Touchscreen,
  StateEvent,
  TextEvent
} from "./inputdevice.js";

import { 
  LocalInputManager
} from "./input-remoting.js";


export class Sender extends LocalInputManager {
  constructor(elem) {
    super();

    const descriptionKeyboard = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Keyboard",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""  
    };

    const descriptionMouse = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Mouse",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""  
    };

    const descriptionTouch = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Touch",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""  
    };

    const descriptionGamepad = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Gamepad",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""
    };

    this.mouse = new Mouse("Mouse", "Mouse", 1, "Default", descriptionMouse);
    this.keyboard = new Keyboard("Keyboard", "Keyboard", 2, "Default", descriptionKeyboard);
    this.gamepad = new Gamepad("Gamepad", "Gamepad", 3, "Default", descriptionGamepad);
    this.touchscreen = new Touchscreen("Touchscreen", "Touchscreen", 4, "Default", descriptionTouch);
    this._devices = [
      this.mouse, 
      this.keyboard, 
      this.gamepad, 
      this.touchscreen
    ];
    // mouse
    elem.addEventListener('click', this._onMouseEvent.bind(this), false);
    elem.addEventListener('mousedown', this._onMouseEvent.bind(this), false);
    elem.addEventListener('mouseup', this._onMouseEvent.bind(this), false);
    elem.addEventListener('mousemove', this._onMouseEvent.bind(this), false);
    elem.addEventListener('wheel', this._onWheelEvent.bind(this), false);

    // keyboard
    document.addEventListener('keyup', this._onKeyEvent.bind(this), false);
    document.addEventListener('keydown', this._onKeyEvent.bind(this), false);

    // gamepad
    // todo:

    // touchscreen
    elem.addEventListener('touchend', this._onTouchEvent.bind(this), false);
    elem.addEventListener('touchstart', this._onTouchEvent.bind(this), false);
    elem.addEventListener('touchcancel', this._onTouchEvent.bind(this), false);
    elem.addEventListener('touchmove', this._onTouchEvent.bind(this), false);
  }

  get devices() {
    return this._devices;
  }

  _onMouseEvent(event) {
    this.mouse.queueEvent(event);
    this._queueStateEvent(this.mouse);
  }
  _onWheelEvent(event) {
    this.mouse.queueEvent(event);
    this._queueStateEvent(this.mouse);
  }
  _onKeyEvent(event) {
    if(event.type == 'keydown') {
      if(!event.repeat) {
        this.keyboard.queueEvent(event);
        this._queueStateEvent(this.keyboard);
      }else{
        this._queueTextEvent(this.keyboard, event.code);
      }  
    }
    else if(event.type == 'keyup') {
      this.keyboard.queueEvent(event);
      this._queueStateEvent(this.keyboard);
    }
  }
  _onTouchEvent(event) {
    this.touchscreen.queueEvent(event);
    this._queueStateEvent(this.touchscreen);
  }
  _onGamepadEvent(event) {
    this.gamepad.queueEvent(event);
    this._queueStateEvent(this.gamepad);
  }


  _queueStateEvent(device) {
    const stateEvent = StateEvent.from(device, Date.now());
    const e = new CustomEvent('event', {detail: { event: stateEvent, device: device}});
    super.onevent.dispatchEvent(e);
  }
  _queueTextEvent(device, character) {
    const textEvent = TextEvent.create(device.deviceId, character, Date.now());
    const e = new CustomEvent('event', {detail: { event: textEvent, device: device}});
    super.onevent.dispatchEvent(e);
  }
}

export class Observer {
  /**
   * 
   * @param {RTCDataChannel} channel 
   */
  constructor(channel) {
    this.channel = channel;    
  }
  /**
   * 
   * @param {Message} message 
   */
  onNext(message) {
    if(this.channel == null || this.channel.readyState != 'open') {
      return;
    }
    this.channel.send(message.serialize());
  }
}