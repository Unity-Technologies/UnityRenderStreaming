import { 
  MemoryHelper
} from "../public/js/memoryhelper.js";

describe(`MemoryHelper.writeSingleBit`, () => {
  test('turn on with offset 0', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 0, false);

    // check 00 00 00
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);
  });
  test('turn off with offset 0', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 0, true);

    // check 00 00 01
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(1);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);

    MemoryHelper.writeSingleBit(bytes, 0, false);

    // check 00 00 00
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);
  });
  test('turn on with offset 32', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 8, true);

    // check 00 01 00
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(1);
    expect(view[2]).toBe(0);

    MemoryHelper.writeSingleBit(bytes, 0, true);

    // check 00 01 01
    expect(view[0]).toBe(1);
    expect(view[1]).toBe(1);
    expect(view[2]).toBe(0);
  });
  test('turn on with offset 15', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 15, true);

    // check 00 80 00
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(128);
    expect(view[2]).toBe(0);

    MemoryHelper.writeSingleBit(bytes, 15, false);

    // check 00 00 00
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);
  });  
});