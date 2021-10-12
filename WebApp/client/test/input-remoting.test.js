import { 
  InputDevice,
  MouseState,
  KeyboardState,
  TouchscreenState,
  GamepadState
} from "../public/js/inputdevice";

import { 
  MessageType,
  NewDeviceMsg,
  NewEventsMsg,
  RemoveDeviceMsg,
  InputRemoting,
  LocalInputManager,
} from "../public/js/input-remoting";

import {
  Observer
} from "../public/js/sender";

describe(`InputRemoting`, () => {
  let manager = null;
  let inputRemoting = null;
  let observer = null;
  beforeEach(async () => {
      manager = new LocalInputManager();
      inputRemoting = new InputRemoting(manager);
      let dc = null; // todo::RTCDataChannel is not defined 
      observer = new Observer(dc);
  });
  test('startSending', () => {
    inputRemoting.startSending();
  });
  test('stopSending', () => {
    inputRemoting.startSending();
    inputRemoting.stopSending();
  });
  test('subscribe', () => {
    inputRemoting.subscribe(observer);
  });
});

test('create NewDeviceMsg', () => {
  const device = new InputDevice("Keyboard", "Keyboard", 0, null, null);
  const msg = NewDeviceMsg.create(device);
  expect(msg.participant_id).toBe(0);
  expect(msg.type).toBe(MessageType.NewDevice);
  expect(msg.data).toBeInstanceOf(ArrayBuffer);
  expect(msg.data.byteLength).toBeGreaterThan(0);
});

describe('create NewEventMsg', () => {
  test('using MouseState', () => {
    const event = new MouseState(null);
    const msg = NewEventsMsg.create(event);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
  test('using KeyboardState', () => {
    const event = new KeyboardEvent("keydown", { keyCode: 39 });
    const state = new KeyboardState(event);
    const msg = NewEventsMsg.create(state);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
  test('using TouchscreenState', () => {
    const event = new TouchscreenState(null);
    const msg = NewEventsMsg.create(event);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
  test('using GamepadState', () => {
    const event = new GamepadState(null);
    const msg = NewEventsMsg.create(event);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
});

test('create RemoveDeviceMsg', () => {
  const device = new InputDevice("Keyboard", "Keyboard", 0, null, null);
  const msg = RemoveDeviceMsg.create(device);
  expect(msg.participant_id).toBe(0);
  expect(msg.type).toBe(MessageType.RemoveDevice);
  expect(msg.data).toBeInstanceOf(ArrayBuffer);
  expect(msg.data.byteLength).toBeGreaterThan(0);
});
  