// import { Signaling, WebSocketSignaling } from "../public/js/signaling";
import { MockSignaling, reset } from "./mocksignaling";
import { waitFor, sleep } from "./testutils";

describe.each([
  { mode:"mock", signaling1: new MockSignaling(), signaling2: new MockSignaling() },
  // { mode:"http", signaling1: new Signaling(), signaling2: new Signaling() },
  // { mode:"websocket", signaling1: new WebSocketSignaling(), signaling2: new WebSocketSignaling() },
])('signaling test in public mode', ({ mode, signaling1, signaling2 }) => {
  const connectionId1 = "12345";
  const connectionId2 = "67890";
  const testsdp = "test sdp";
  const testcandidate = "test candidate";

  beforeAll(async () => {
    if (mode == "mock") {
      reset(false);
      return;
    }

    // ToDO: on http/websocket mode, run webserver process
  });

  afterAll(async () => {
    if (mode == "mock") {
      return;
    }

    // ToDO: on http/websocket mode, kill webserver process
  });

  beforeEach(async () => {
    await signaling1.start();
    await signaling2.start();
  });

  afterEach(async () => {
    await signaling1.stop();
    await signaling2.stop();
  });

  test('onConnect', async () => {
    let connectRes;
    let disconnectRes;
    signaling1.addEventListener('connect', (e) => connectRes = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes = e.detail);

    await signaling1.createConnection(connectionId1);
    await waitFor(() => connectRes != null);
    expect(connectRes.connectionId).toBe(connectionId1);
    expect(connectRes.polite).toBe(true);

    await signaling1.deleteConnection(connectionId1);
    await waitFor(() => disconnectRes != null);
    expect(disconnectRes.connectionId).toBe(connectionId1);
  });

  test('onOffer', async () => {
    let connectRes1;
    let disconnectRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);

    await signaling1.createConnection(connectionId1);
    await signaling2.createConnection(connectionId2);
    await waitFor(() => connectRes1 != null && connectRes2 != null);
    expect(connectRes1.connectionId).toBe(connectionId1);
    expect(connectRes2.connectionId).toBe(connectionId2);

    await signaling1.sendOffer(connectionId1, testsdp);
    await waitFor(() => offerRes2 != null);
    expect(offerRes2.connectionId).toBe(connectionId1);
    expect(offerRes2.polite).toBe(false);

    await signaling1.deleteConnection(connectionId1);
    await waitFor(() => disconnectRes1 != null);
    expect(disconnectRes1.connectionId).toBe(connectionId1);
    await signaling2.deleteConnection(connectionId2);
    await waitFor(() => disconnectRes2 != null);
    expect(disconnectRes2.connectionId).toBe(connectionId2);
  });

  test('onAnswer', async () => {
    let connectRes1;
    let disconnectRes1;
    let answerRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);
    signaling1.addEventListener('answer', (e) => answerRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);

    await signaling1.createConnection(connectionId1);
    await signaling2.createConnection(connectionId2);
    await waitFor(() => connectRes1 != null && connectRes2 != null);

    await signaling1.sendOffer(connectionId1, testsdp);
    await waitFor(() => offerRes2 != null);
    expect(offerRes2.connectionId).toBe(connectionId1);
    expect(offerRes2.sdp).toBe(testsdp);

    signaling2.sendAnswer(connectionId1, testsdp);
    await waitFor(() => answerRes1 != null);
    expect(answerRes1.connectionId).toBe(connectionId1);
    expect(answerRes1.sdp).toBe(testsdp);

    await signaling1.deleteConnection(connectionId1);
    await waitFor(() => disconnectRes1 != null);
    await signaling2.deleteConnection(connectionId2);
    await waitFor(() => disconnectRes2 != null);
  });

  test('onCandidate', async () => {
    let connectRes1;
    let disconnectRes1;
    let answerRes1;
    let candidateRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);
    signaling1.addEventListener('answer', (e) => answerRes1 = e.detail);
    signaling1.addEventListener('candidate', (e) => candidateRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    let candidateRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);
    signaling2.addEventListener('candidate', (e) => candidateRes2 = e.detail);

    await signaling1.createConnection(connectionId1);
    await signaling2.createConnection(connectionId2);
    await waitFor(() => connectRes1 != null && connectRes2 != null);

    await signaling1.sendOffer(connectionId1, testsdp);
    await waitFor(() => offerRes2 != null);
    expect(offerRes2.connectionId).toBe(connectionId1);
    expect(offerRes2.sdp).toBe(testsdp);

    signaling2.sendAnswer(connectionId1, testsdp);
    await waitFor(() => answerRes1 != null);
    expect(answerRes1.connectionId).toBe(connectionId1);
    expect(answerRes1.sdp).toBe(testsdp);

    await signaling2.sendCandidate(connectionId1, testcandidate, 1, 1);
    await waitFor(() => candidateRes1 != null);
    expect(candidateRes1.connectionId).toBe(connectionId1);
    expect(candidateRes1.candidate).toBe(testcandidate);
    expect(candidateRes1.sdpMid).toBe(1);
    expect(candidateRes1.sdpMLineIndex).toBe(1);

    await signaling1.sendCandidate(connectionId1, testcandidate, 1, 1);
    await waitFor(() => candidateRes2 != null);
    expect(candidateRes2.connectionId).toBe(connectionId1);
    expect(candidateRes2.candidate).toBe(testcandidate);
    expect(candidateRes2.sdpMid).toBe(1);
    expect(candidateRes2.sdpMLineIndex).toBe(1);

    await signaling1.deleteConnection(connectionId1);
    await waitFor(() => disconnectRes1 != null);
    await signaling2.deleteConnection(connectionId2);
    await waitFor(() => disconnectRes2 != null);
  });
});

describe.each([
  { mode:"mock", signaling1: new MockSignaling(), signaling2: new MockSignaling() },
  // { mode:"http", signaling1: new Signaling(), signaling2: new Signaling() },
  // { mode:"websocket", signaling1: new WebSocketSignaling(), signaling2: new WebSocketSignaling() },
])('signaling test in private mode', ({ mode, signaling1, signaling2 }) => {
  const connectionId = "12345";
  const testsdp = "test sdp";
  const testcandidate = "test candidate";

  beforeAll(async () => {
    if (mode == "mock") {
      reset(false);
      return;
    }

    // ToDO: on http/websocket mode, run webserver process
  });

  afterAll(async () => {
    if (mode == "mock") {
      return;
    }

    // ToDO: on http/websocket mode, kill webserver process
  });

  beforeEach(async () => {
    signaling1 = new MockSignaling();
    await signaling1.start();
    signaling2 = new MockSignaling();
    await signaling2.start();
  });

  afterEach(async () => {
    await signaling1.stop();
    signaling1 = null;
    await signaling2.stop();
    signaling2 = null;
  });

  test('onConnect', async () => {
    let connectRes1;
    let disconnectRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);

    await signaling1.createConnection(connectionId);
    await waitFor(() => connectRes1 != null);
    await signaling2.createConnection(connectionId);
    await waitFor(() => connectRes2 != null);

    expect(connectRes1.connectionId).toBe(connectionId);
    expect(connectRes2.connectionId).toBe(connectionId);
    expect(connectRes1.polite).toBe(false);
    expect(connectRes2.polite).toBe(true);

    await signaling1.deleteConnection(connectionId);
    await waitFor(() => disconnectRes1 != null && disconnectRes2 != null);
    expect(disconnectRes1.connectionId).toBe(connectionId);
    expect(disconnectRes2.connectionId).toBe(connectionId);

    await signaling2.deleteConnection(connectionId);
  });

  test('onOffer', async () => {
    let connectRes1;
    let disconnectRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);

    await signaling1.createConnection(connectionId);
    await waitFor(() => connectRes1 != null);
    expect(connectRes1.connectionId).toBe(connectionId);

    signaling1.sendOffer(connectionId, testsdp);
    await sleep(signaling1.interval * 10);
    // Do not receive offer other signaling if not connected same sendoffer connectionId in private mode
    expect(offerRes2).toBeUndefined();

    await signaling2.createConnection(connectionId);
    await waitFor(() => connectRes2 != null);
    expect(connectRes2.connectionId).toBe(connectionId);

    await signaling1.sendOffer(connectionId, testsdp);
    await waitFor(() => offerRes2 != null);
    expect(offerRes2.connectionId).toBe(connectionId);
    expect(offerRes2.polite).toBe(true);

    await signaling1.deleteConnection(connectionId);
    await waitFor(() => disconnectRes1 != null && disconnectRes2 != null);
    expect(disconnectRes1.connectionId).toBe(connectionId);
    expect(disconnectRes2.connectionId).toBe(connectionId);

    await signaling2.deleteConnection(connectionId);
  });

  test('onAnswer', async () => {
    let connectRes1;
    let disconnectRes1;
    let answerRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);
    signaling1.addEventListener('answer', (e) => answerRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);

    await signaling1.createConnection(connectionId);
    await signaling2.createConnection(connectionId);
    await waitFor(() => connectRes1 != null && connectRes2 != null);

    await signaling1.sendOffer(connectionId, testsdp);
    await waitFor(() => offerRes2 != null);
    expect(offerRes2.connectionId).toBe(connectionId);
    expect(offerRes2.sdp).toBe(testsdp);

    await signaling2.sendAnswer(connectionId, testsdp);
    await waitFor(() => answerRes1 != null);
    expect(answerRes1.connectionId).toBe(connectionId);
    expect(answerRes1.sdp).toBe(testsdp);

    await signaling1.deleteConnection(connectionId);
    await waitFor(() => disconnectRes1 != null && disconnectRes2 != null);
    expect(disconnectRes1.connectionId).toBe(connectionId);
    expect(disconnectRes2.connectionId).toBe(connectionId);

    await signaling2.deleteConnection(connectionId);
  });

  test('onCandidate', async () => {
    let connectRes1;
    let disconnectRes1;
    let answerRes1;
    let candidateRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);
    signaling1.addEventListener('answer', (e) => answerRes1 = e.detail);
    signaling1.addEventListener('candidate', (e) => candidateRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    let candidateRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);
    signaling2.addEventListener('candidate', (e) => candidateRes2 = e.detail);

    await signaling1.createConnection(connectionId);
    await signaling2.createConnection(connectionId);
    await waitFor(() => connectRes1 != null && connectRes2 != null);

    await signaling1.sendOffer(connectionId, testsdp);
    await waitFor(() => offerRes2 != null);
    expect(offerRes2.connectionId).toBe(connectionId);
    expect(offerRes2.sdp).toBe(testsdp);

    await signaling2.sendAnswer(connectionId, testsdp);
    await waitFor(() => answerRes1 != null);
    expect(answerRes1.connectionId).toBe(connectionId);
    expect(answerRes1.sdp).toBe(testsdp);

    await signaling2.sendCandidate(connectionId, testcandidate, 1, 1);
    await waitFor(() => candidateRes1 != null);
    expect(candidateRes1.connectionId).toBe(connectionId);
    expect(candidateRes1.candidate).toBe(testcandidate);
    expect(candidateRes1.sdpMLineIndex).toBe(1);
    expect(candidateRes1.sdpMid).toBe(1);

    await signaling1.sendCandidate(connectionId, testcandidate, 1, 1);
    await waitFor(() => candidateRes2 != null);
    expect(candidateRes2.connectionId).toBe(connectionId);
    expect(candidateRes2.candidate).toBe(testcandidate);
    expect(candidateRes2.sdpMLineIndex).toBe(1);
    expect(candidateRes2.sdpMid).toBe(1);

    await signaling1.deleteConnection(connectionId);
    await waitFor(() => disconnectRes1 != null && disconnectRes2 != null);
    expect(disconnectRes1.connectionId).toBe(connectionId);
    expect(disconnectRes2.connectionId).toBe(connectionId);

    await signaling2.deleteConnection(connectionId);
  });

  test('notReceiveOwnOfferAnswer', async () => {
    let connectRes1;
    let disconnectRes1;
    let offerRes1;
    let answerRes1;
    signaling1.addEventListener('connect', (e) => connectRes1 = e.detail);
    signaling1.addEventListener('disconnect', (e) => disconnectRes1 = e.detail);

    let connectRes2;
    let disconnectRes2;
    let offerRes2;
    let answerRes2;
    signaling2.addEventListener('connect', (e) => connectRes2 = e.detail);
    signaling2.addEventListener('disconnect', (e) => disconnectRes2 = e.detail);

    await signaling1.createConnection(connectionId);
    await signaling2.createConnection(connectionId);
    await waitFor(() => connectRes1 != null && connectRes2 != null);

    signaling1.addEventListener('offer', (e) => offerRes1 = e.detail);
    signaling2.addEventListener('offer', (e) => offerRes2 = e.detail);
    await signaling1.sendOffer(connectionId, testsdp);
    await sleep(signaling1.interval * 5);
    expect(offerRes1).toBeUndefined();
    expect(offerRes2.connectionId).toBe(connectionId);
    expect(offerRes2.sdp).toBe(testsdp);

    signaling1.addEventListener('answer', (e) => answerRes1 = e.detail);
    signaling2.addEventListener('answer', (e) => answerRes2 = e.detail);
    await signaling2.sendAnswer(connectionId, testsdp);
    await sleep(signaling2.interval * 5);
    expect(answerRes1.connectionId).toBe(connectionId);
    expect(answerRes1.sdp).toBe(testsdp);
    expect(answerRes2).toBeUndefined();

    await signaling1.deleteConnection(connectionId);
    await waitFor(() => disconnectRes1 != null && disconnectRes2 != null);
    expect(disconnectRes1.connectionId).toBe(connectionId);
    expect(disconnectRes2.connectionId).toBe(connectionId);

    await signaling2.deleteConnection(connectionId);
  });
});
