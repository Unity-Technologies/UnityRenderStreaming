import { 
  MemoryHelper,
} from "./memoryhelper.js";

export class FourCC {
  _code;
  constructor(a, b, c, d) {
    this._code = (a.charCodeAt() << 24) 
    | (b.charCodeAt() << 16) 
    | (c.charCodeAt() << 8) 
    | d.charCodeAt();
  }

  toInt32() {
    return this._code;
  }
}


export class InputDevice {
  name;
  layout;
  deviceId;
  variants;
  description;

  _inputState;

  constructor(name, layout, deviceId, variants, description) {
    this.name = name;
    this.layout = layout;
    this.deviceId = deviceId;
    this.variants = variants;
    this.description = description;
  }

  updateState(state) {
    this._inputState = state;
  }

  queueEvent(e) {
    throw new Error('Please implement this method');
  }
  
  /**
   * @returns {IInputState}
   */
  get currentState() {
    return this._inputState;
  }
}

export class Mouse extends InputDevice {
  queueEvent(e) {
    this.updateState(new MouseState(e));
  }
}

export class Keyboard extends InputDevice {
  static keycount = 110;
  /**
   * 
   * @param {KeyboardEvent} event 
   */
  queueEvent(event) {
    this.updateState(new KeyboardState(event));
  }
}

export class Touchscreen extends InputDevice {
  queueEvent(e) {
    this.updateState(new TouchscreenState(e));
  }
}

export class Gamepad extends InputDevice {
  queueEvent(e) {
    this.updateState(new GamepadState(e));
  }
}

class NativeInputEvent {
  static size = 20;
  // field offset 0
  type;
  // field offset 4
  sizeInBytes;
  // field offset 6
  deviceId;
  // field offset 8
  time;
  // field offset 16
  eventId;

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
  }
}

export class InputEvent {
  static invalidEventId = 0;
  static size = 20;

  // field offset 0
  type;
  // field offset 4
  sizeInBytes;
  // field offset 6
  deviceId;
  // field offset 8
  time;
  // field offset 16
  eventId;

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
   * @returns {Number}
   */
  get type() {
    return this.type;
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
  static size = 30;
  static format = new FourCC('M', 'O', 'U', 'S').toInt32();
  // field offset 0
  position;
  // field offset 8
  delta;
  // field offset 16
  scroll;
  // field offset 24
  buttons;
  // field offset 26
  displayIndex;
  // field offset 28
  clickCount;

  constructor(event) {
    super();
    this.position = [0, 0];
    this.delta = [0, 0];
    this.scroll = [0, 0];
    this.buttons = new ArrayBuffer(2);
  }

  /**
   * @returns {ArrayBuffer}
   */
  get buffer() {
    const size = MouseState.size;
    let _buffer = new ArrayBuffer(size);
    let view = new DataView(_buffer);
    view.setFloat32(0, this.position[0], true);
    view.setFloat32(4, this.position[1], true);
    view.setFloat32(8, this.delta[0], true);
    view.setFloat32(12, this.delta[1], true);
    view.setFloat32(16, this.scroll[0], true);
    view.setFloat32(20, this.scroll[1], true);
    new Uint8Array(_buffer).set(new Uint8Array(this.buttons), 24);
//    view.setFloat32(16, displayIndex, true); //todo
//    view.setFloat32(20, clickCount, true); //todo
    return _buffer;
  }

  /**
   * @returns {Number}
   */
  get format() {
    return MouseState.format;
  }  
  get sizeInBits() {
    return MouseState.size * 8; // todo
  }
}

export class KeyboardState extends IInputState {
  static sizeInBits = Keyboard.keycount;
  static sizeInBytes = (KeyboardState.sizeInBits + 7) / 8;
  static format = new FourCC('K', 'E', 'Y', 'S').toInt32();
  keys;

  /**
   * 
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
    MemoryHelper.writeSingleBit(this.keys, event.keyCode, value);
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
  get sizeInBits() {
    return KeyboardState.sizeInBits;
  }
}

export class TouchscreenState extends IInputState {
  static size = 56;  // todo
  static maxTouches = 10;
  static format = new FourCC('T', 'S', 'C', 'R').toInt32();
  // field offset 0
  primaryTouchData;
  // field offset 56
  touchData;

  /**
   * 
   * @param {TouchEvent} event 
   */
  constructor(event) {
    super();
  }

  /**
   * @returns {ArrayBuffer}
   */
   get buffer() {
    const size = TouchscreenState.size; // todo
    let _buffer = new ArrayBuffer(size);
    let _data = new DataView(_buffer); //todo
    return _buffer;
  }

  /**
   * @returns {Number}
   */
   get format() {
    return TouchscreenState.format;
  }
  get sizeInBits() {
    return TouchscreenState.size * 8;
  }
}

export class GamepadState extends IInputState {
  static sizeInBytes = 28;
  static format = new FourCC('G', 'P', 'A', 'D').toInt32();
  // field offset 0
  buttons;
  // field offset 4
  leftStick;
  // field offset 12
  rightStick;
  // field offset 20
  leftTrigger;
  // field offset 24
  rightTrigger;

  constructor(event) {
    super();
  }

  /**
   * @returns {ArrayBuffer}
   */
   get buffer() {
    const size = GamepadState.sizeInBytes;
    let _buffer = new ArrayBuffer(size);
    let view = new DataView(_buffer);
    view.setInt32(this.buttons);
    view.setFloat64(this.leftStick);
    view.setFloat64(this.rightStick);
    view.setInt32(this.leftTrigger);
    view.setInt32(this.rightTrigger);
    return _buffer;
  }

  /**
   * @returns {Number}
   */
   get format() {
    return GamepadState.format;
  }

  get sizeInBits() {
    return GamepadState.sizeInBytes * 8;
  }
}

export class TextEvent {
  static format = new FourCC('T', 'E', 'X', 'T').toInt32();
  // field offset 0
  baseEvent;
  // field offset 20
  character;

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
  static format = new FourCC('S', 'T', 'A', 'T').toInt32();
  // field offset 0
  baseEvent;
  // field offset 20
  stateFormat;
  // field offset 24
  stateData;

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

    // const array = new Uint8Array(_buffer);
    // console.log(`${String.fromCharCode(array[0])} ${String.fromCharCode(array[1])} ${String.fromCharCode(array[2])} ${String.fromCharCode(array[3])}`);
    return _buffer;
  }
}