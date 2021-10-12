import { 
  StateEvent,
} from "./inputdevice.js";

export class LocalInputManager {
  // _onevent;
  constructor() {
    this._onevent = new EventTarget();
  }

  get onevent() {
    return this._onevent;
  }

  get devices() {
    return null;
  }
}

export class InputRemoting {
  // _localManager;
  // _subscribers;
  // _sending = false;

  constructor(manager) {
    this._localManager = manager;
    this._subscribers = new Array();
    this._sending = false;
  }

  startSending() {
    if(this._sending) {
      return;
    }
    this._sending = true;
    this._localManager.onevent.addEventListener("event", this._onsendEvent.bind(this));
    this._sendInitialMessages();
  }

  stopSending() {
    if (!this._sending) {
      return;
    }
    this._sending = false;
  }

  /**
   * 
   * @param {Observer} observer 
   */
  subscribe(observer) {
    this._subscribers.push(observer);
  }

  _sendInitialMessages() {
   this._sendAllGeneratedLayouts();
   this._sendAllDevices();
  }

  _sendAllGeneratedLayouts() {
  }

  _sendAllDevices() {
    var devices = this._localManager.devices;
    if(devices == null)
      return;
    for (const device of devices) {
      this._sendDevice(device);
    }
  }

  _sendDevice(device) {
    const newDeviceMessage = NewDeviceMsg.create(device);
    this._send(newDeviceMessage);

    // todo:
    // Send current state. We do this here in this case as the device
    // may have been added some time ago and thus have already received events.

    // const stateEventMessage = NewEventsMsg.createStateEvent(device);
    // this._send(stateEventMessage);
  }

  _onsendEvent(e) {
    const stateEvent = e.detail.event;
    const message = NewEventsMsg.create(stateEvent);
    this._send(message);
  }

  /**
   * 
   * @param {Message} message 
   */
  _send(message) {
    for(let subscriber of this._subscribers) {
      subscriber.onNext(message);
    }
  }
}

export const MessageType = {
  Connect: 0,
  Disconnect: 1,
  NewLayout: 2,
  NewDevice: 3,
  NewEvents: 4,
  RemoveDevice: 5,
  RemoveLayout: 6,
  ChangeUsages: 7,
  StartSending: 8,
  StopSending: 9,
};

export class Message {
  /**
   * field offset 0
   * {Number} participant_id;
   * 
   * field offset 4
   * {Number} type;
   * 
   * field offset 8
   * {Number} length;
   * 
   * field offset 12
   * {ArrayBuffer} data;
   */

  /**
   * 
   * @param {number} participantId 
   * @param {MessageType} type 
   * @param {ArrayBuffer} data 
   */
  constructor(participantId, type, data) {
    this.participant_id = participantId;
    this.type = type;
    this.length = data.byteLength;
    this.data = data;
  }

  /**
   * 
   * @returns {ArrayBuffer}
   */
  serialize() {
    const sizeOfInt = 4;
    const totalSize = sizeOfInt + sizeOfInt + sizeOfInt + this.data.byteLength;
    let buffer = new ArrayBuffer(totalSize);
    let dataView = new DataView(buffer);
    let uint8view =  new Uint8Array(buffer);
    dataView.setUint32(0, this.participant_id, true);
    dataView.setUint32(4, this.type, true);
    dataView.setUint32(8, this.length, true);
    uint8view.set(new Uint8Array(this.data), 12);
    return buffer;
  }
}

export class NewDeviceMsg {
  static create(device) {
    const data = {
      name: device.name,
      layout: device.layout,
      deviceId: device.deviceId,
      variants: device.variants,
      description: device.description
    };
    const json = JSON.stringify(data);
    let buffer = new ArrayBuffer(json.length*2); // 2 bytes for each char
    let view = new Uint8Array(buffer);
    const length = json.length;
    for (let i = 0; i < length; i++) {
      view[i] = json.charCodeAt(i);
    }
    return new Message(0, MessageType.NewDevice, buffer);
  }
}

export class NewEventsMsg {
  /**
   * 
   * @param {InputDevice} device 
   * @returns {Message}
   */
  static createStateEvent(device) {
    const events = StateEvent.from(device);
    return NewEventsMsg.create(events);
  }

  /**
   * 
   * @param {StateEvent} event 
   * @returns {Message}
   */
  static create(event) {
      return new Message(0, MessageType.NewEvents, event.buffer);
  }
}

export class RemoveDeviceMsg {
  /**
   * 
   * @param {InputDevice} device 
   * @returns {Message}
   */
   static create(device) {
    const sizeOfInt = 4;
    let buffer = new ArrayBuffer(sizeOfInt);
    let view = new DataView(buffer);
    view.setInt32(device.deviceId);
    return new Message(0, MessageType.RemoveDevice, buffer);
  }
}
