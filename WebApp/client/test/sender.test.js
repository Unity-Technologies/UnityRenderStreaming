import { 
  InputRemoting,
} from "../public/js/inputremoting.js";

import {
  Sender,
  Observer
} from "../public/js/sender.js";

import {jest} from '@jest/globals';
import {DOMRect} from "./domrect.js";

// mock

class RTCDataChannel {
  get readyState() {
    return "open";
  }
  /* eslint-disable no-unused-vars */
  send(message) {
  }
}
  
describe(`Sender`, () => {
  let inputRemoting = null;
  let sender = null;
  let observer = null;
  let events = {};
  let dc = null;
  beforeEach(async () => {
      // Empty our events before each test case
      events = {};

      // Define the addEventListener method with a Jest mock function
      document.addEventListener = jest.fn((event, callback) => {
        events[event] = callback;
      });
      
      document.removeEventListener = jest.fn((event, callback) => {
        delete events[event];
      });
      document.getBoundingClientRect = function(){ return new DOMRect(0,0,0,0); };
      sender = new Sender(document);
      inputRemoting = new InputRemoting(sender);
      dc = new RTCDataChannel(); 
      observer = new Observer(dc);
  });
  test('devices', () => {
    sender.addMouse();
    expect(sender.devices.length).toBe(1);
    sender.addKeyboard();
    expect(sender.devices.length).toBe(2);
  });
  test('send messages while called startSending', () => {
    jest.spyOn(dc, 'send');
    sender.addMouse();
    sender.addKeyboard();
    inputRemoting.subscribe(observer);
    inputRemoting.startSending();
    expect(dc.send).toHaveBeenCalled();
  });
  describe('mouse', () => {
    test('click', () => {
      jest.spyOn(dc, 'send');
      sender.addMouse();
      inputRemoting.subscribe(observer);
      inputRemoting.startSending();
      events.click(
        new MouseEvent('click', { buttons:1, clientX:0, clientY:0} ));
      expect(dc.send).toBeCalledWith(expect.any(ArrayBuffer));
    });
    test('mousemove', () => {
      jest.spyOn(dc, 'send');
      sender.addMouse();
      inputRemoting.subscribe(observer);
      inputRemoting.startSending();
      events.mousemove(
        new MouseEvent('mousemove', { buttons:1, deltaX:0, deltaY:0 }));
      expect(dc.send).toBeCalledWith(expect.any(ArrayBuffer));
    });    
    test('wheel', () => {
      jest.spyOn(dc, 'send');
      sender.addMouse();
      inputRemoting.subscribe(observer);
      inputRemoting.startSending();
      events.wheel(
        new WheelEvent('wheel', { wheelDelta:0, deltaX:0, deltaY:0 }));
      expect(dc.send).toBeCalledWith(expect.any(ArrayBuffer));
    });
  });
  describe('keyboard', () => {
    test('keydown', () => {
      jest.spyOn(dc, 'send');
      sender.addKeyboard();
      inputRemoting.subscribe(observer);
      inputRemoting.startSending();
      events.keydown(
        new KeyboardEvent('keydown', { code: 'KeyA' }));
      expect(dc.send).toBeCalledWith(expect.any(ArrayBuffer));
    });
    test('keydown repeat', () => {
      jest.spyOn(dc, 'send');
      sender.addKeyboard();
      inputRemoting.subscribe(observer);
      inputRemoting.startSending();
      events.keydown(
        new KeyboardEvent('keydown', { code: 'KeyA', repeat: true }));
      expect(dc.send).toBeCalledWith(expect.any(ArrayBuffer));
    });    
  });
  describe('touchscreen', () => {
    test('touchstart', () => {
      jest.spyOn(dc, 'send');
      sender.addTouchscreen();
      inputRemoting.subscribe(observer);
      inputRemoting.startSending();
      events.touchstart(
        new TouchEvent("touchstart", { 
          changedTouches: [{ // InputInit
            identifier: 0,
            target: null,
            clientX: 0,
            clientY: 0,
            screenX: 0,
            screenY: 0,
            pageX: 0,
            pageY: 0,
            radiusX: 0,
            radiusY: 0,
            rotationAngle: 0,
            force: 0,
            altitudeAngle: 0,
            azimuthAngle:0,
            touchType: "direct"
          }]
        }));
      expect(dc.send).toBeCalledWith(expect.any(ArrayBuffer));
    });
  });
  describe('gamepad', () => {
    //todo
  });
});