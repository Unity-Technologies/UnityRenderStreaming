import { 
  MemoryHelper,
} from "./memoryhelper.js";

import { Keymap } from "./keymap.js";
import { MouseButton } from "./mousebutton.js";
import { GamepadButton } from "./gamepadbutton.js";

export class FourCC {
  /**
   * {Number} _code;
   */

  /**
   * 
   * @param {String} a 
   * @param {String} b 
   * @param {String} c 
   * @param {String} d 
   */
  constructor(a, b, c, d) {
    this._code = (a.charCodeAt() << 24) 
    | (b.charCodeAt() << 16) 
    | (c.charCodeAt() << 8) 
    | d.charCodeAt();
  }

  /**
   * @returns {Number}
   */
  toInt32() {
    return this._code;
  }
}


export class InputDevice {

  /**
   * 
   * name;
   * layout;
   * deviceId;
   * variants;
   * description;
   * 
   * _inputState;
   */
  
  /**
   * 
   * @param {Number} name 
   * @param {String} layout 
   * @param {Number} deviceId 
   * @param {String} variants 
   * @param {Object} description 
   */
  constructor(name, layout, deviceId, variants, description) {
    this.name = name;
    this.layout = layout;
    this.deviceId = deviceId;
    this.variants = variants;
    this.description = description;
  }

  /**
   * 
   * @param {IInputState} state 
   */
  updateState(state) {
    this._inputState = state;
  }

  queueEvent(event) {
    throw new Error(`Please implement this method. event:${event}`);
  }
  
  /**
   * @returns {IInputState}
   */
  get currentState() {
    return this._inputState;
  }
}

export class Mouse extends InputDevice {
  /**
   * @param {(MouseEvent|WheelEvent)} event 
   */
  queueEvent(event) {
    this.updateState(new MouseState(event));
  }
}

export class Keyboard extends InputDevice {
  static get keycount() { return 110; }
  /**
   * 
   * @param {KeyboardEvent} event 
   */
  queueEvent(event) {
    this.updateState(new KeyboardState(event));
  }
}

export class Touchscreen extends InputDevice {
  /**
   * @param {TouchScreenEvent} event 
   */
  queueEvent(event) {
    this.updateState(new TouchscreenState(event));
  }
}

export class Gamepad extends InputDevice {
  /**
   * @param {GamepadButtonEvent | GamepadAxisEvent} event 
   */
  queueEvent(event) {
    this.updateState(new GamepadState(event));
  }
}

export class InputEvent {
  static get invalidEventId() { return 0; }
  static get size() { return 20; }

  /** 
   * field offset 0
   * @member {Number} type;
   * 
   * field offset 4
   * @member {Number} sizeInBytes;
   * 
   * field offset 6
   * @member {Number} deviceId;
   * 
   * field offset 8
   * @member {Number} time;
   * 
   * field offset 16
   * @member {Number} eventId;
   */

  /**
   * 
   * @param {Number} type 
   * @param {Number} sizeInBytes
   * @param {Number} deviceId 
   * @param {Number} time 
   */
  constructor(type, sizeInBytes, deviceId, time) {
    this.type = type;
    this.sizeInBytes = sizeInBytes;
    this.deviceId = deviceId;
    this.time = time;
    this.eventId = InputEvent.invalidEventId;
  }

  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    let _buffer = new ArrayBuffer(InputEvent.size);
    let view = new DataView(_buffer);
    view.setInt32(0, this.type, true);
    view.setInt16(4, this.sizeInBytes, true);
    view.setInt16(6, this.deviceId, true);
    view.setFloat64(8, this.time, true);
    view.setInt16(16, this.sizeInBytes, true);
    return _buffer;
  }
}

export class IInputState {
  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    throw new Error('Please implement this field');
  }
  /**
   * @returns {Number}
   */
   get format() {
    throw new Error('Please implement this field');    
  }
}

export class MouseState extends IInputState {
  static get size() { return 30; }
  static get format() { return new FourCC('M', 'O', 'U', 'S').toInt32(); }

  /**
   * field offset 0
   * @member {Array} position;
   * 
   * field offset 8
   * @member {Array} delta;
   * 
   * field offset 16
   * @member {Array} scroll;
   * 
   * field offset 24
   * @member {ArrayBuffer} buttons;
   * 
   * field offset 26
   * @member {Array} displayIndex;
   * 
   * field offset 28
   * @member {Array} clickCount;
   */

  /**
   * @param {MouseEvent | WheelEvent} event 
   */
  constructor(event) {
    super();
    
    this.position = [event.clientX, event.clientY];
    this.delta = [event.movementX, -event.movementY];
    this.scroll = [0, 0];
    if(event.type === 'wheel') {
      this.scroll = [event.deltaX, event.deltaY];
    }
    this.buttons = new ArrayBuffer(2);

    const left = event.buttons & 1 << 0;
    const right = event.buttons & 1 << 1;
    const middle = event.buttons & 1 << 2;
    const back = event.buttons & 1 << 3;
    const forward = event.buttons & 1 << 4;

    MemoryHelper.writeSingleBit(this.buttons, MouseButton.Left, left);
    MemoryHelper.writeSingleBit(this.buttons, MouseButton.Right, right);
    MemoryHelper.writeSingleBit(this.buttons, MouseButton.Middle, middle);
    MemoryHelper.writeSingleBit(this.buttons, MouseButton.Forward, forward);
    MemoryHelper.writeSingleBit(this.buttons, MouseButton.Back, back);
  }

  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    const size = MouseState.size;
    const buttons = new Uint16Array(this.buttons)[0];
    let _buffer = new ArrayBuffer(size);
    let view = new DataView(_buffer);
    view.setFloat32(0, this.position[0], true);
    view.setFloat32(4, this.position[1], true);
    view.setFloat32(8, this.delta[0], true);
    view.setFloat32(12, this.delta[1], true);
    view.setFloat32(16, this.scroll[0], true);
    view.setFloat32(20, this.scroll[1], true);
    view.setUint16(24, buttons, true);
    view.setUint16(26, this.displayIndex, true);
    view.setUint16(28, this.clickCount, true);
    return _buffer;
  }

  /**
   * @returns {Number}
   */
  get format() {
    return MouseState.format;
  }
}

export class KeyboardState extends IInputState {
  static get sizeInBits() { return Keyboard.keycount; }
  static get sizeInBytes() { return (KeyboardState.sizeInBits + 7) >> 3; }
  static get format() { return new FourCC('K', 'E', 'Y', 'S').toInt32(); }

  /**
   * field offset 0
   * @number {ArrayBuffer} keys;
   */

  /**
   * @param {KeyboardEvent} event 
   */
  constructor(event) {
    super();
    this.keys = new ArrayBuffer(KeyboardState.sizeInBytes);
    let value = false;
    switch(event.type) {
      case 'keydown':
        value = true;
        break;
      case 'keyup':
        value = false;
        break;
      default:
        throw new Error(`unknown event type ${event.type})`);
    }
    const key = Keymap[event.code];
    MemoryHelper.writeSingleBit(this.keys, key, value);
  }

  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    return this.keys;
  }

  /**
   * @returns {Number}
   */
   get format() {
    return KeyboardState.format;
  }
}

export class TouchscreenState extends IInputState {
  static get size() { return 56; }  // todo
  static get maxTouches() { return 10; } 
  static get format() { return new FourCC('T', 'S', 'C', 'R').toInt32(); }

  /**
   * field offset 0
   * @number {Array} primaryTouchData;
   * field offset 56
   * @number {Array} touchData;
   */

  /**
   * @param {TouchEvent} event 
   */
  constructor(event) {
    super();
    this.touchData = event.touches; // todo
  }

  /**
   * @returns {ArrayBuffer}
   */
   get buffer() {
    const size = TouchscreenState.size; // todo
    let _buffer = new ArrayBuffer(size);
//    let _data = new DataView(_buffer); //todo
    return _buffer;
  }

  /**
   * @returns {Number}
   */
   get format() {
    return TouchscreenState.format;
  }
}

export class GamepadState extends IInputState {
  static get size() { return 28; }
  static get format() { return new FourCC('G', 'P', 'A', 'D').toInt32(); }
  
  /**
   * field offset 0
   * @member buttons;
   * 
   * field offset 4
   * @member leftStick;
   * 
   * field offset 12
   * @member rightStick;
   * 
   * field offset 20
   * @member leftTrigger;
   * 
   * field offset 24
   * @member rightTrigger;
   */

  /**
   * 
   * @param {GamepadButtonEvent | GamepadAxisEvent} event 
   */
  constructor(event) {
    super();
    const gamepad = event.gamepad;
    const buttons = event.gamepad.buttons;

    this.buttons = new ArrayBuffer(4);
    this.leftStick = [ gamepad.axes[0], -gamepad.axes[1] ];
    this.rightStick = [ gamepad.axes[2], -gamepad.axes[3] ];
    this.leftTrigger = buttons[6].value;
    this.rightTrigger = buttons[7].value;
    
    // see https://w3c.github.io/gamepad/#remapping
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.A, buttons[0].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.B, buttons[1].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.X, buttons[2].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.Y, buttons[3].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.LeftShoulder, buttons[4].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.RightShoulder, buttons[5].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.LeftTrigger, buttons[6].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.RightTrigger, buttons[7].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.Select, buttons[8].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.Start, buttons[9].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.LeftStick, buttons[10].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.RightStick, buttons[11].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.DpadUp, buttons[12].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.DpadDown, buttons[13].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.DpadLeft, buttons[14].pressed);
    MemoryHelper.writeSingleBit(this.buttons, GamepadButton.DpadRight, buttons[15].pressed);    
  }

  /**
   * @returns {ArrayBuffer}
   */
   get buffer() {
    const size = GamepadState.size;
    let _buffer = new ArrayBuffer(size);
    let view = new DataView(_buffer);
    view.setUint32(0, new Uint32Array(this.buttons)[0], true);
    view.setFloat32(4, this.leftStick[0], true);
    view.setFloat32(8, this.leftStick[1], true);
    view.setFloat32(12, this.rightStick[0], true);
    view.setFloat32(16, this.rightStick[1], true);
    view.setFloat32(20, this.leftTrigger, true);
    view.setFloat32(24, this.rightTrigger, true);
    return _buffer;
  }

  /**
   * @returns {Number}
   */
   get format() {
    return GamepadState.format;
  }
}

export class TextEvent {
  static get format() { return new FourCC('T', 'E', 'X', 'T').toInt32(); }

  /**
   * field offset 0
   * @member {InputEvent} baseEvent;
   * 
   * field offset 20
   * @member {Number} character;
   */

  /**
   * 
   * @param {Number} deviceId 
   * @param {Number} character
   * @param {Number} time 
   * @returns {TextEvent}

   */
  static create(deviceId, character, time) {
    const sizeOfInt = 4;
    const eventSize = InputEvent.size + sizeOfInt;

    let event = new TextEvent();
    event.baseEvent = new InputEvent(TextEvent.format, eventSize, deviceId, time); 
    event.character = character;
    return event;
  }

  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    const sizeOfInt = 4;
    const size = InputEvent.size + sizeOfInt;
    let _buffer = new ArrayBuffer(size);
    let arrayView = new Uint8Array(_buffer);
    let dataView = new DataView(_buffer);
    arrayView.set(new Uint8Array(this.baseEvent.buffer), 0);
    dataView.setInt32(InputEvent.size, this.character, true);
    return _buffer;
  }
}

export class StateEvent {
  static get format() { return new FourCC('S', 'T', 'A', 'T').toInt32(); }

  /**
   * field offset 0
   * @member {InputEvent} baseEvent;
   * 
   * field offset 20
   * @member {Number} stateFormat;
   * 
   * field offset 24
   * @member {ArrayBuffer} stateData;
   */

  /**
   * 
   * @param {InputDevice} device 
   * @param {Number} time 
   * @returns {StateEvent}
   */
  static from(device, time) {
    return StateEvent.fromState(device.currentState, device.deviceId, time);
  }

  /**
   * 
   * @param {IInputState} state 
   * @param {Number} deviceId
   * @param {Number} time 
   */
  static fromState(state, deviceId, time) {
    const sizeOfInt = 4;
    const stateData = state.buffer;
    const stateSize = stateData.byteLength;
    const eventSize = InputEvent.size + sizeOfInt + stateSize;
    
    let stateEvent = new StateEvent();
    stateEvent.baseEvent = new InputEvent(StateEvent.format, eventSize, deviceId, time);
    stateEvent.stateFormat = state.format;
    stateEvent.stateData = stateData;
    return stateEvent;
  }

  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    const sizeOfInt = 4;
    const stateSize = this.stateData.byteLength;
    const size = InputEvent.size + sizeOfInt + stateSize;
    let _buffer = new ArrayBuffer(size);
    let uint8View = new Uint8Array(_buffer);
    let dataView = new DataView(_buffer);
    uint8View.set(new Uint8Array(this.baseEvent.buffer), 0);
    dataView.setInt32(InputEvent.size, this.stateFormat, true);
    uint8View.set(new Uint8Array(this.stateData), InputEvent.size+sizeOfInt);
    return _buffer;
  }
}