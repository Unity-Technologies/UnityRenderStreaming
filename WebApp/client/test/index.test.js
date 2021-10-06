import { default as Signaling, WebSocketSignaling } from "../public/js/signaling.js";

test('basic', () => {
  var http = new Signaling();
  expect(http).not.toBeNull();
});

test('basic2', () => {
  var ws = new WebSocketSignaling();
  expect(ws).not.toBeNull();
});
