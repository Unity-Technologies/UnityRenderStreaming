import { 
  PointerCorrector
} from "../public/js/pointercorrect.js";

import {DOMRect} from "./domrect.js";

describe(`PointerCorrector.map`, () => {
  test('mapping', () => {
    const rect = new DOMRect(10, 10, 200, 200);
    let corrector = new PointerCorrector(100, 100, rect);
    const position = [10, 10];
    const newPosition = corrector.map(position);
    expect(newPosition[0]).toBe(0);
    expect(newPosition[1]).toBe(100);
  });
});