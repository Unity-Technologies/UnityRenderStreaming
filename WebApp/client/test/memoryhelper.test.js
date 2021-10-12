import { 
  MemoryHelper
} from "../public/js/memoryhelper";

describe(`MemoryHelper.writeSingleBit`, () => {
  test('turn on with offset 0', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 0, false);

    // check 0000 0000 0000
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);
  });
  test('turn off with offset 0', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 0, true);

    // check 0000 0000 0001
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(1);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);

    MemoryHelper.writeSingleBit(bytes, 0, false);

    // check 0000 0000 0000
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(0);
    expect(view[2]).toBe(0);
  });
  test('turn on with offset 32', () => {
    let bytes = new ArrayBuffer(3);
    MemoryHelper.writeSingleBit(bytes, 32, true);

    // check 0000 0001 0000
    const view = new Uint8Array(bytes);
    expect(view[0]).toBe(0);
    expect(view[1]).toBe(1);
    expect(view[2]).toBe(0);

    MemoryHelper.writeSingleBit(bytes, 0, true);

    // check 0000 0001 0001
    expect(view[0]).toBe(1);
    expect(view[1]).toBe(1);
    expect(view[2]).toBe(0);
  });
});