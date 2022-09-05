/* eslint-disable no-undef */
import fetch from "node-fetch";
import { PeerConnectionMock, SessionDescriptionMock, IceCandidateMock } from './test/peerconnectionmock';
import ResizeObserverMock from './test/resizeobservermock';

if (!window.fetch) {
  window.fetch = fetch;
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