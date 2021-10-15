export class MemoryHelper {
  /**
   * @param {ArrayBuffer} buffer 
   * @param {number} bitOffset
   * @param {boolean} value
   */
  static writeSingleBit(buffer, bitOffset, value) {
    let view = new Uint8Array(buffer);
    const index = Math.floor(bitOffset / 8);
    bitOffset = bitOffset % 8;
    const byte = view[index];
    let newByte = 1 << bitOffset;
    if(value) {
      newByte = newByte | byte;
    }
    else {
      newByte = ~newByte & byte;
    }
    view[index] = newByte;
  }

  /**
   * @return {Number}
   */
  static get sizeOfInt() {
    return 4;
  }
}