import Peer from "../public/js/peer";
import { waitFor, sleep, getUniqueId } from "./testutils";


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

  test(`close peer`, async () => {
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();

    peer.close();
    expect(peer.connectionId).toBeNull();
    expect(peer.pc).toBeNull();
  });

  test(`transceiver direction is sendonly if using addtrack`, () => {
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();

    const track = { id: getUniqueId(), kind: "audio" };
    const sender = peer.addTrack(connectionId, track);
    const transceiver = peer.getTransceivers(connectionId).find(t => t.sender == sender);
    expect(transceiver.direction).toBe("sendonly");
  });

  test(`fire trackevent when addtrack`, async () => {
    let trackEvent;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('trackevent', (e) => trackEvent = e.detail);

    const track = { id: getUniqueId(), kind: "audio" };
    peer.addTrack(connectionId, track);
    await waitFor(() => trackEvent != null);
    expect(trackEvent.track).toBe(track);
  });

  test(`fire trackevent when on got offer description include track`, async () => {
    let trackEvent;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('trackevent', (e) => trackEvent = e.detail);

    const testDesc = { type: "offer", sdp: "newtracksdp" };
    peer.onGotDescription(connectionId, testDesc);
    await waitFor(() => trackEvent != null);
    expect(trackEvent.track).not.toBeNull();
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

  test(`fire sendoffer when addTransceiver`, async () => {
    let offer;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => offer = e.detail);

    peer.addTransceiver(connectionId, "video");
    await waitFor(() => offer != null);
    expect(offer.connectionId).toBe(connectionId);
  });

  test(`fire sendoffer when createDataChannel`, async () => {
    let offer;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => offer = e.detail);

    peer.createDataChannel(connectionId, "testChannel");
    await waitFor(() => offer != null);
    expect(offer.connectionId).toBe(connectionId);
  });

  test(`re-fire sendoffer if get answer not yet`, async () => {
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

    const testDesc = { type: "offer", sdp: "newtracksdp" };
    peer.onGotDescription(connectionId, testDesc);
    await waitFor(() => answer != null);
    expect(answer.connectionId).toBe(connectionId);
  });

  test(`fire sendanswer when on got offer description in polite that have offer`, async () => {
    let offer;
    let answer;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendoffer', (e) => offer = e.detail);
    peer.addEventListener('sendanswer', (e) => answer = e.detail);

    const track = { id: getUniqueId(), kind: "audio" };
    peer.addTrack(connectionId, track);
    await waitFor(() => offer != null);
    expect(offer.connectionId).toBe(connectionId);

    const testDesc = { type: "offer", sdp: "newtracksdp" };
    peer.onGotDescription(connectionId, testDesc);
    await waitFor(() => answer != null);
    expect(answer.connectionId).toBe(connectionId);
  });

  test(`fire sendanswer when on got offer description in impolite that don't have offer`, async () => {
    let answer;
    const peer = new Peer(connectionId, false);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendanswer', (e) => answer = e.detail);

    const testDesc = { type: "offer", sdp: "newtracksdp" };
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

    const testDesc = { type: "offer", sdp: "newtracksdp" };
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

    const answerDesc = { type: "answer", sdp: "newtracksdp" };
    peer.onGotDescription(connectionId, answerDesc);
    await waitFor(() => negotiated);
    expect(negotiated).toBeTruthy();
  });

  test(`fire sendcandidate when on addTransceiver`, async () => {
    let candidate;
    const peer = new Peer(connectionId, true);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendcandidate', (e) => candidate = e.detail);

    peer.addTransceiver(connectionId, { id: getUniqueId(), kind: "video" });
    await waitFor(() => candidate != null);
    expect(candidate.connectionId).toBe(connectionId);
  });

  test(`accept candidate when on got candidate that have remote description`, async () => {
    let answer;
    const peer = new Peer(connectionId, false);
    expect(peer).not.toBeNull();
    peer.addEventListener('sendanswer', (e) => answer = e.detail);

    const testDesc = { type: "offer", sdp: "newtracksdp" };
    peer.onGotDescription(connectionId, testDesc);
    await waitFor(() => answer != null);
    expect(answer.connectionId).toBe(connectionId);

    const testCandidate = { candidate: getUniqueId(), sdpMLineIndex: 0, sdpMid: 0 };
    peer.onGotCandidate(connectionId, testCandidate);
    await waitFor(() => peer.pc.candidates.length > 0);
    expect(peer.pc.candidates.length).toBeGreaterThan(0);
  });

  test(`don't accept candidate when on got candidate that don't have remote description`, async () => {
    const peer = new Peer(connectionId, false);
    expect(peer).not.toBeNull();

    const testCandidate = { candidate: getUniqueId(), sdpMLineIndex: 0, sdpMid: 0 };
    peer.onGotCandidate(connectionId, testCandidate);
    await sleep(100);
    expect(peer.pc.candidates.length).toBe(0);
  });
});
