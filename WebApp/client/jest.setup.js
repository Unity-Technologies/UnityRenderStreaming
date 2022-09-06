/* eslint-disable no-undef */
import fetch from 'node-fetch';
import { TextEncoder, TextDecoder } from 'util';
import { PeerConnectionMock, SessionDescriptionMock, IceCandidateMock } from './test/peerconnectionmock';
import ResizeObserverMock from './test/resizeobservermock';

// note: If set testEnvironment `jest-environment-jsdom`,  below classes are not defined.

if (!window.fetch) {
  window.fetch = fetch;
}

if (!window.TextEncoder) {
  window.TextEncoder = TextEncoder;
}

if (!window.TextDecoder) {
  window.TextDecoder = TextDecoder;
}

if (!window.RTCPeerConnection) {
  window.RTCPeerConnection = PeerConnectionMock;
}

if (!window.RTCSessionDescription) {
  window.RTCSessionDescription = SessionDescriptionMock;
}

if (!window.RTCIceCandidate) {
  window.RTCIceCandidate = IceCandidateMock;
}

if (!window.ResizeObserver) {
  window.ResizeObserver = ResizeObserverMock;
}