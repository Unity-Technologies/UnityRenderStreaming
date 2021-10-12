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
    expect.assertions(0);
    inputRemoting.startSending();
  });
  test('stopSending', () => {
    expect.assertions(0);
    inputRemoting.startSending();
    inputRemoting.stopSending();
  });
  test('subscribe', () => {
    expect.assertions(0);
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
    const event = new MouseEvent('click', { buttons:0, clientX:0, clientY:0} );
    const state = new MouseState(event);
    const msg = NewEventsMsg.create(state);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
  test('using KeyboardState', () => {
    const event = new KeyboardEvent("keydown", { code: 'KeyA' });
    const state = new KeyboardState(event);
    const msg = NewEventsMsg.create(state);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
  test('using TouchscreenState', () => {
    const event = new TouchEvent('touchstart', { changedTouches: [], touches: [] });
    const state = new TouchscreenState(event);
    const msg = NewEventsMsg.create(state);
    expect(msg.participant_id).toBe(0);
    expect(msg.type).toBe(MessageType.NewEvents);
    expect(msg.data).toBeInstanceOf(ArrayBuffer);
    expect(msg.data.byteLength).toBeGreaterThan(0);
  });
  test('using GamepadState', () => {
    class GamepadEvent extends CustomEvent {} // todo
    const event = new GamepadEvent('connect', {});
    const state = new GamepadState(event);
    const msg = NewEventsMsg.create(state);
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
  