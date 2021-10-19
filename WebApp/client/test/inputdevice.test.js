import {
    FourCC,
    Mouse,
    Keyboard,
    Touchscreen,
    Gamepad,
    KeyboardState,
    MouseState,
    TouchscreenState,
    GamepadState,
    StateEvent,
    InputEvent,
    TextEvent
  } from "../public/js/inputdevice.js";

describe(`FourCC`, () => {
  test('toInt32', () => {
    const number = new FourCC('A', 'A', 'A', 'A').toInt32();
    expect(number).toBe(0x41414141);
  });
});

describe(`MouseState`, () => {
  describe(`with MouseEvent`, () => {
    let event;
    beforeEach(() => {
      event = new MouseEvent('click', { buttons:1, clientX:0, clientY:0});
    });  
    test('format', () => {
      const format = new MouseState(event).format;
      expect(format).toBe(0x4d4f5553);
    });  
    test('buffer', () => {
      const state = new MouseState(event);
      expect(state.buffer.byteLength).toBeGreaterThan(0);
    });  
  });
  describe(`with WheelEvent`, () => {
    let event;
    beforeEach(() => {
      event = new WheelEvent('wheel', { deltaX:0, deltaY:0 });
    });
    test('format', () => {
      const format = new MouseState(event).format;
      expect(format).toBe(0x4d4f5553);
    });  
    test('buffer', () => {
      const state = new MouseState(event);
      expect(state.buffer.byteLength).toBeGreaterThan(0);
    });  
  });
});

describe(`KeyboardState`, () => {
  let event;
  beforeEach(() => {
    event = new KeyboardEvent('keydown', { code: 'KeyA' });
  });
  test('format', () => {
    const format = new KeyboardState(event).format;
    expect(format).toBe(0x4b455953);
  });  
  test('buffer', () => {
    const state = new KeyboardState(event);
    expect(state.buffer.byteLength).toBeGreaterThan(0);
  });
});

describe(`TouchscreenState`, () => {
  let event;
  beforeEach(() => {
    event = new TouchEvent("touchstart", { 
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
    });
  });  
  test('format', () => {
    const format = new TouchscreenState(event, null, Date.now()).format;
    expect(format).toBe(0x54534352);
  });
  test('buffer', () => {
    const state = new TouchscreenState(event, null, Date.now());
    expect(state.buffer.byteLength).toBeGreaterThan(0);
  });
});

describe(`GamepadState`, () => {
  let event;
  beforeEach(() => {
    event = {
      type: 'gamepadupdated',
      gamepad : {
        id: 1,
        buttons: Array(16).fill({ pressed: false, value: 1 }),
        axes:[0, 0, 0, 0]
    }};
  });
  test('format', () => {
    const format = new GamepadState(event).format;
    expect(format).toBe(0x47504144);
  });  
  test('buffer', () => {
    const state = new GamepadState(event);
    expect(state.buffer.byteLength).toBeGreaterThan(0);
  });
});

describe(`StateEvent`, () => {
  let state;
  beforeEach(() => {
    const event = new KeyboardEvent('keydown', { code: 'KeyA' });
    state = new KeyboardState(event);
  });
  test('buffer', () => {
    const stateEvent = StateEvent.fromState(state, 0, Date.now());
    expect(new Int32Array(stateEvent.buffer.slice(0, 4))[0]).toBe(StateEvent.format);
  });
});

describe(`TextEvent`, () => {
  test('buffer', () => {
    const character = 0x41;
    const textEvent = TextEvent.create(0, character, Date.now());
    expect(new Int32Array(textEvent.buffer.slice(0, 4))[0]).toBe(TextEvent.format);
    const offset = InputEvent.size;
    expect(new Uint32Array(textEvent.buffer.slice(offset, offset+4))[0]).toBe(character);
  });
});
 
describe(`Mouse`, () => {
  test('alignedSizeInBytes', () => {
    let device = new Mouse("Mouse", "Mouse", 1, null, null);
    expect(device).toBeInstanceOf(Mouse);
  });
});

describe(`Keyboard`, () => {
  test('alignedSizeInBytes', () => {
    let device = new Keyboard("Keyboard", "Keyboard", 1, null, null);
    expect(device).toBeInstanceOf(Keyboard);
  });
});

describe(`Touchscreen`, () => {
  test('alignedSizeInBytes', () => {
    let device = new Touchscreen("Touchscreen", "Touchscreen", 1, null, null);
    expect(device).toBeInstanceOf(Touchscreen);
  });
});

describe(`Gamepad`, () => {
  test('alignedSizeInBytes', () => {
    let device = new Gamepad("Gamepad", "Gamepad", 1, null, null);
    expect(device).toBeInstanceOf(Gamepad);
  });
});