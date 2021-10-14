/* eslint-disable no-undef */
import fetch from "node-fetch";
import PeerConnectionMock from './test/peerconnectionmock';

if (!window.fetch) {
  window.fetch = fetch;
}

if (!window.RTCPeerConnection) {
  window.RTCPeerConnection = PeerConnectionMock;
}
