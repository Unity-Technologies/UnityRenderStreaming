import Peer from "../public/js/peer";
import { waitFor, sleep, getUniqueId } from "./testutils";

// let trackEvent;
// let offer;
// let answer;
// let candidate;
// const peer = new Peer(connectionId, true);
// expect(peer).not.toBeNull();
// peer.addEventListener('disconnect', () => { });
// peer.addEventListener('trackevent', (e) => trackEvent = e.detail);
// peer.addEventListener('sendoffer', (e) => offer = e.detail);
// peer.addEventListener('sendanswer', (e) => answer = e.detail);
// peer.addEventListener('sendcandidate', (e) => candidate = e.detail);
// peer.pc.addEventListener('negotiated', () => { });

describe(`peer connection test`, () => {
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

  test(`fire sendoffer when addtrack`, async () => {
    let offer;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => offer = e.detail);

    const track = { id: getUniqueId(), kind: "audio" };
    peer.addTrack(connectionId, track);
    await waitFor(() => offer != null);
    expect(offer.connectionId).toBe(connectionId);
  });

  test(`re-fire sendoffer when don't get answer`, async () => {
    let sendOfferCount = 0;
    let offer;
    const peer = new Peer(connectionId, true, 100);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => { 
      offer = e.detail; 
      sendOfferCount++;
    });

    const track = { id: getUniqueId(), kind: "audio" };
    peer.addTrack(connectionId, track);
    await waitFor(() => sendOfferCount > 2);
    expect(offer.connectionId).toBe(connectionId);
    expect(sendOfferCount).toBeGreaterThan(2);
  });

  test(`fire sendanswer when on got offer description in polite`, async () => {
    let answer;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendanswer', (e) => answer = e.detail);

    const testDesc = { type: "offer", sdp: "testsdp" };
    peer.onGotDescription(connectionId, testDesc);
    await waitFor(() => answer != null);
    expect(answer.connectionId).toBe(connectionId);
  });

  test(`fire sendanswer when on got offer description in impolite that don't have offer`, async () => {
    let answer;
    const peer = new Peer(connectionId, false);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendanswer', (e) => answer = e.detail);

    const testDesc = { type: "offer", sdp: "testsdp" };
    peer.onGotDescription(connectionId, testDesc);
    await waitFor(() => answer != null);
    expect(answer.connectionId).toBe(connectionId);
  });

  test(`don't fire sendanswer when on got offer description in impolite that have offer`, async () => {
    let offer;
    let answer;
    const peer = new Peer(connectionId, false);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => offer = e.detail);
    peer.addEventListener('sendanswer', (e) => answer = e.detail);

    const track = { id: getUniqueId(), kind: "audio" };
    peer.addTrack(connectionId, track);
    await waitFor(() => offer != null);
    expect(offer.connectionId).toBe(connectionId);

    const testDesc = { type: "offer", sdp: "testsdp" };
    peer.onGotDescription(connectionId, testDesc);
    await sleep(100);
    expect(answer).toBeUndefined();
  });

  test(`fire nagotiated when on got answer description that have offer`, async () => {
    let offer;
    let negotiated = false;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => offer = e.detail);
    peer.pc.addEventListener('negotiated', () => negotiated = true);

    const track = { id: getUniqueId(), kind: "audio" };
    peer.addTrack(connectionId, track);
    await waitFor(() => offer != null);
    expect(offer.connectionId).toBe(connectionId);

    const answerDesc = { type: "answer", sdp: "testsdp" };
    peer.onGotDescription(connectionId, answerDesc);
    await waitFor(() => negotiated);
    expect(negotiated).toBeTruthy();
  });
});