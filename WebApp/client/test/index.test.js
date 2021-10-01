// test for client

import { default as Signaling, WebSocketSignaling } from "../public/js/signaling.js";

test('basic', () => {
  var http = new Signaling();
  var ws = new WebSocketSignaling();
  expect('hello').toBe('hello');
});

test('basic2', () => {
  expect(1 + 1).toBe(2);
});
