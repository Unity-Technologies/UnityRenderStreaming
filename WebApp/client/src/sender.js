import {
  Mouse,
  Keyboard,
  Gamepad,
  Touchscreen,
  StateEvent,
  TextEvent
} from "./inputdevice.js";

import { LocalInputManager } from "./inputremoting.js";
import { GamepadHandler } from "./gamepadhandler.js";
import { PointerCorrector } from "./pointercorrect.js";

export class Sender extends LocalInputManager {
  constructor(elem) {
    super();
    this._devices = [];
    this._elem = elem;
    this._corrector = new PointerCorrector(
      this._elem.videoWidth,
      this._elem.videoHeight,
      this._elem.getBoundingClientRect()
      );

    //since line 27 cannot complete resize initialization but can only monitor div dimension changes, line 26 needs to be reserved
    this._elem.addEventListener('resize', this._onResizeEvent.bind(this), false);
    const observer = new ResizeObserver(this._onResizeEvent.bind(this));
    observer.observe(this._elem);
  }

  addMouse() {
    const descriptionMouse = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Mouse",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""
    };
    this.mouse = new Mouse("Mouse", "Mouse", 1, null, descriptionMouse);
    this._devices.push(this.mouse);

    this._elem.addEventListener('click', this._onMouseEvent.bind(this), false);
    this._elem.addEventListener('mousedown', this._onMouseEvent.bind(this), false);
    this._elem.addEventListener('mouseup', this._onMouseEvent.bind(this), false);
    this._elem.addEventListener('mousemove', this._onMouseEvent.bind(this), false);
    this._elem.addEventListener('wheel', this._onWheelEvent.bind(this), false);
  }

  addKeyboard() {
    const descriptionKeyboard = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Keyboard",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""
    };
    this.keyboard = new Keyboard("Keyboard", "Keyboard", 2, null, descriptionKeyboard);
    this._devices.push(this.keyboard);

    document.addEventListener('keyup', this._onKeyEvent.bind(this), false);
    document.addEventListener('keydown', this._onKeyEvent.bind(this), false);
  }

  addGamepad() {
    const descriptionGamepad = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Gamepad",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""
    };
    this.gamepad = new Gamepad("Gamepad", "Gamepad", 3, null, descriptionGamepad);
    this._devices.push(this.gamepad);

    window.addEventListener("gamepadconnected", this._onGamepadEvent.bind(this), false);
    window.addEventListener("gamepaddisconnected", this._onGamepadEvent.bind(this), false);
    this._gamepadHandler = new GamepadHandler();
    this._gamepadHandler.addEventListener("gamepadupdated", this._onGamepadEvent.bind(this), false);
  }

  addTouchscreen() {
    const descriptionTouch = {
      m_InterfaceName: "RawInput",
      m_DeviceClass: "Touch",
      m_Manufacturer: "",
      m_Product: "",
      m_Serial: "",
      m_Version: "",
      m_Capabilities: ""
    };
    this.touchscreen = new Touchscreen("Touchscreen", "Touchscreen", 4, null, descriptionTouch);
    this._devices.push(this.touchscreen);

    this._elem.addEventListener('touchend', this._onTouchEvent.bind(this), false);
    this._elem.addEventListener('touchstart', this._onTouchEvent.bind(this), false);
    this._elem.addEventListener('touchcancel', this._onTouchEvent.bind(this), false);
    this._elem.addEventListener('touchmove', this._onTouchEvent.bind(this), false);
    this._elem.addEventListener('click', this._onTouchEvent.bind(this), false);
  }

  /**
   * @returns {InputDevice[]}
   */
  get devices() {
    return this._devices;
  }

  _onResizeEvent() {
    this._corrector.reset(
      this._elem.videoWidth,
      this._elem.videoHeight,
      this._elem.getBoundingClientRect()
    );
  }
  _onMouseEvent(event) {
    this.mouse.queueEvent(event);
    this.mouse.currentState.position = this._corrector.map(this.mouse.currentState.position);
    this._queueStateEvent(this.mouse.currentState, this.mouse);
  }
  _onWheelEvent(event) {
    this.mouse.queueEvent(event);
    this._queueStateEvent(this.mouse.currentState, this.mouse);
  }
  _onKeyEvent(event) {
    if(event.type == 'keydown') {
      if(!event.repeat) { // StateEvent
        this.keyboard.queueEvent(event);
        this._queueStateEvent(this.keyboard.currentState, this.keyboard);
      }
      // TextEvent
      const key = event.key.charCodeAt(0);
      this._queueTextEvent(this.keyboard, key);
    }
    else if(event.type == 'keyup') {
      this.keyboard.queueEvent(event);
      this._queueStateEvent(this.keyboard.currentState, this.keyboard);
    }
  }
  _onTouchEvent(event) {
    this.touchscreen.queueEvent(event, this.timeSinceStartup);
    for(let touch of this.touchscreen.currentState.touchData) {
      let clone = touch.copy();
      clone.position = this._corrector.map(clone.position);
      this._queueStateEvent(clone, this.touchscreen);
    }
  }
  _onGamepadEvent(event) {
    switch(event.type) {
      case 'gamepadconnected': {
        this._gamepadHandler.addGamepad(event.gamepad);
        break;
      }
      case 'gamepaddisconnected': {
        this._gamepadHandler.removeGamepad(event.gamepad);
        break;
      }
      case 'gamepadupdated': {
        this.gamepad.queueEvent(event);
        this._queueStateEvent(this.gamepad.currentState, this.gamepad);
        break;
      }
    }
  }

  _queueStateEvent(state, device) {
    const stateEvent =
      StateEvent.fromState(state, device.deviceId, this.timeSinceStartup);
    const e = new CustomEvent(
      'event', {detail: { event: stateEvent, device: device}});
    super.onEvent.dispatchEvent(e);
  }
  _queueTextEvent(device, character) {
    const textEvent = TextEvent.create(device.deviceId, character, this.timeSinceStartup);
    const e = new CustomEvent(
      'event', {detail: { event: textEvent, device: device}});
    super.onEvent.dispatchEvent(e);
  }
  _queueDeviceChange(device, usage) {
    const e = new CustomEvent(
      'changedeviceusage', {detail: { device: device, usage: usage }});
    super.onEvent.dispatchEvent(e);
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
    this.channel.send(message.buffer);
  }
}
