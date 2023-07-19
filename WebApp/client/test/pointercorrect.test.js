import {
  LetterBoxType,
  PointerCorrector
} from "../src/pointercorrect.js";

import {DOMRect} from "./domrect.js";
import {DOMHTMLVideoElement} from "./domvideoelement.js";

describe(`PointerCorrector.map`, () => {
  test('letterboxType', () => {
    const rect = new DOMRect(10, 10, 200, 200);
    const element = new DOMHTMLVideoElement(rect);
    let corrector = new PointerCorrector(50, 100, element);
    expect(corrector.letterBoxType).toBe(LetterBoxType.Vertical);
    corrector.reset(100, 50, element);
    expect(corrector.letterBoxType).toBe(LetterBoxType.Horizontal);
  });
  test('letterboxSize', () => {
    const rect = new DOMRect(0, 0, 100, 100);
    const element = new DOMHTMLVideoElement(rect);
    let corrector = new PointerCorrector(50, 100, element);
    expect(corrector.letterBoxSize).toBe(25);
  });
  test('contentRect', () => {
    const rect = new DOMRect(0, 0, 100, 100);
    const element = new DOMHTMLVideoElement(rect);
    let corrector = new PointerCorrector(50, 100, element);
    expect(corrector.contentRect.x).toBe(25);
    expect(corrector.contentRect.y).toBe(0);
    expect(corrector.contentRect.width).toBe(50);
    expect(corrector.contentRect.height).toBe(100);
  });
  test('mapping', () => {
    const rect = new DOMRect(10, 10, 200, 200);
    const element = new DOMHTMLVideoElement(rect);
    const videoWidth = 100;
    const videoHeight = 100;
    let corrector = new PointerCorrector(videoWidth, videoHeight, element);
    const position = [10, 10];
    const newPosition = corrector.map(position);
    expect(newPosition[0]).toBe(0);
    expect(newPosition[1]).toBe(100);
  });

});
