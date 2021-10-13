import Peer from "../public/js/peer";

describe(`PeerConnection test`, () => {
  const connectionId = "12345";
  test(`constructor`, () => {
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();

    const rtcPeer = peer.pc;
    expect(rtcPeer).not.toBeNull();
    expect(rtcPeer.ontrack).not.toBeNull();
    expect(rtcPeer.onicecandidate).not.toBeNull();
    expect(rtcPeer.onnegotiationneeded).not.toBeNull();
    expect(rtcPeer.onsignalingstatechange).not.toBeNull();
    expect(rtcPeer.oniceconnectionstatechange).not.toBeNull();
    expect(rtcPeer.onicegatheringstatechange).not.toBeNull();
  });
});