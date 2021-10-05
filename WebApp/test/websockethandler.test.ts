import WS from "jest-websocket-mock";
import Answer from "../src/class/answer";
import Candidate from "../src/class/candidate";
import Offer from "../src/class/offer";
import * as wsHandler from '../src/class/websockethandler';

Date.now = jest.fn(() => 1482363367071);

describe('websocket signaling test in public mode', () => {
  let server: WS;
  let client: WebSocket;
  let client2: WebSocket;
  const connectionId = "12345";
  const connectionId2 = "67890";
  const testsdp = "test sdp";

  beforeAll(async () => {
    wsHandler.reset("public");
    server = new WS("ws://localhost:1234", { jsonProtocol: true });
    client = new WebSocket("ws://localhost:1234");
    await server.connected;
    client2 = new WebSocket("ws://localhost:1234");
    await server.connected;
  });

  afterAll(() => {
    WS.clean();
  });

  test('create session1', async () => {
    expect(client).not.toBeNull();
    await wsHandler.add(client);
  });

  test('create session2', async () => {
    expect(client2).not.toBeNull();
    await wsHandler.add(client2);
  });

  test('create connection from session1', async () => {
    await wsHandler.onConnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "connect", connectionId: connectionId, polite: true });
    expect(server).toHaveReceivedMessages([{ type: "connect", connectionId: connectionId, polite: true }]);
  });

  test('create connection from session2', async () => {
    await wsHandler.onConnect(client2, connectionId2);
    await expect(server).toReceiveMessage({ type: "connect", connectionId: connectionId2, polite: true });
    expect(server).toHaveReceivedMessages([{ type: "connect", connectionId: connectionId2, polite: true }]);
  });

  test('send offer from session1', async () => {
    await wsHandler.onOffer(client, { connectionId: connectionId, sdp: testsdp });
    const receiveOffer = new Offer(testsdp, Date.now(), false);
    await expect(server).toReceiveMessage({ from: connectionId, to: "", type: "offer", data: receiveOffer });
    expect(server).toHaveReceivedMessages([{ from: connectionId, to: "", type: "offer", data: receiveOffer }]);
  });

  test('send answer from session2', async () => {
    await wsHandler.onAnswer(client2, { connectionId: connectionId, sdp: testsdp });
    const receiveAnswer = new Answer(testsdp, Date.now());
    await expect(server).toReceiveMessage({ from: connectionId, to: "", type: "answer", data: receiveAnswer });
    expect(server).toHaveReceivedMessages([{ from: connectionId, to: "", type: "answer", data: receiveAnswer }]);
  });

  test('send candidate from sesson1', async () => {
    const msg = { connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: "0" };
    await wsHandler.onCandidate(client, msg);
    const receiveCandidate = new Candidate("testcandidate", 0, "0", Date.now());
    await expect(server).toReceiveMessage({ from: connectionId, to: "", type: "candidate", data: receiveCandidate });
    expect(server).toHaveReceivedMessages([{ from: connectionId, to: "", type: "candidate", data: receiveCandidate }]);
  });

  test('delete connection from session2', async () => {
    await wsHandler.onDisconnect(client2, connectionId);
    // disconnect send to client
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
    // disconnect send to client2
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
    // server received total 2 disconnect messages
    expect(server).toHaveReceivedMessages([{ type: "disconnect", connectionId: connectionId }, { type: "disconnect", connectionId: connectionId }]);
  });

  test('delete connection from session1', async () => {
    await wsHandler.onDisconnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
    expect(server).toHaveReceivedMessages([{ type: "disconnect", connectionId: connectionId }, { type: "disconnect", connectionId: connectionId }]);
  });

  test('delete session2', async () => {
    expect(client).not.toBeNull();
    await wsHandler.remove(client2);
  });

  test('delete session1', async () => {
    expect(client2).not.toBeNull();
    await wsHandler.remove(client);
  });
});

describe('websocket signaling test in private mode', () => {
  let server: WS;
  let client: WebSocket;
  let client2: WebSocket;
  const connectionId = "12345";
  const testsdp = "test sdp";

  beforeAll(async () => {
    wsHandler.reset("private");
    server = new WS("ws://localhost:1234", { jsonProtocol: true });
    client = new WebSocket("ws://localhost:1234");
    await server.connected;
    client2 = new WebSocket("ws://localhost:1234");
    await server.connected;
  });

  afterAll(() => {
    WS.clean();
  });

  test('create session1', async () => {
    expect(client).not.toBeNull();
    await wsHandler.add(client);
  });

  test('create session2', async () => {
    expect(client2).not.toBeNull();
    await wsHandler.add(client2);
  });

  test('create connection from session1', async () => {
    await wsHandler.onConnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "connect", connectionId: connectionId, polite: false });
    expect(server).toHaveReceivedMessages([{ type: "connect", connectionId: connectionId, polite: false }]);
  });

  test('create connection from session2', async () => {
    await wsHandler.onConnect(client2, connectionId);
    await expect(server).toReceiveMessage({ type: "connect", connectionId: connectionId, polite: true });
    expect(server).toHaveReceivedMessages([{ type: "connect", connectionId: connectionId, polite: true }]);
  });

  test('send offer from session1', async () => {
    await wsHandler.onOffer(client, { connectionId: connectionId, sdp: testsdp });
    const receiveOffer = new Offer(testsdp, Date.now(), true);
    await expect(server).toReceiveMessage({ from: connectionId, to: "", type: "offer", data: receiveOffer });
    expect(server).toHaveReceivedMessages([{ from: connectionId, to: "", type: "offer", data: receiveOffer }]);
  });

  test('send answer from session2', async () => {
    await wsHandler.onAnswer(client2, { connectionId: connectionId, sdp: testsdp });
    const receiveAnswer = new Answer(testsdp, Date.now());
    await expect(server).toReceiveMessage({ from: connectionId, to: "", type: "answer", data: receiveAnswer });
    expect(server).toHaveReceivedMessages([{ from: connectionId, to: "", type: "answer", data: receiveAnswer }]);
  });

  test('send candidate from sesson1', async () => {
    const msg = { connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: "0" };
    await wsHandler.onCandidate(client, msg);
    const receiveCandidate = new Candidate("testcandidate", 0, "0", Date.now());
    await expect(server).toReceiveMessage({ from: connectionId, to: "", type: "candidate", data: receiveCandidate });
    expect(server).toHaveReceivedMessages([{ from: connectionId, to: "", type: "candidate", data: receiveCandidate }]);
  });

  test('delete connection from session2', async () => {
    await wsHandler.onDisconnect(client2, connectionId);
    // disconnect send to client
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
    // disconnect send to client2
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
    // server received total 2 disconnect messages
    expect(server).toHaveReceivedMessages([{ type: "disconnect", connectionId: connectionId }, { type: "disconnect", connectionId: connectionId }]);
  });

  test('delete connection from session1', async () => {
    await wsHandler.onDisconnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
    expect(server).toHaveReceivedMessages([{ type: "disconnect", connectionId: connectionId }, { type: "disconnect", connectionId: connectionId }]);
  });

  test('delete session2', async () => {
    expect(client).not.toBeNull();
    await wsHandler.remove(client2);
  });

  test('delete session1', async () => {
    expect(client2).not.toBeNull();
    await wsHandler.remove(client);
  });
});
