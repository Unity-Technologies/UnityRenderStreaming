// test for server

import Offer from '../src/class/offer';

test('basic', () => {
    const offer = new Offer("", 1, false);
    expect('hello').toBe('hello');
});

test('basic2', () => {
    expect(1+1).toBe(2);
});
