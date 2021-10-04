// test for server
import { getMockReq, getMockRes } from '@jest-mock/express';
import * as httpHandler from '../src/class/httphandler';

import WS from "jest-websocket-mock";
import * as wsHandler from '../src/class/websockethandler';

describe('http signaling test in public mode', () => {
  const sessionId = "abcd1234";
  const headerMock = (): string => sessionId;
  const connectionId = "12345";

  const { res, mockClear } = getMockRes()
  const req = getMockReq({ header: headerMock, body: { connectionId: connectionId } });

  beforeAll(() => {
    httpHandler.reset("public");
  });

  beforeEach(() => {
    mockClear();
  });

  test('create session', async () => {
    await httpHandler.createSession(sessionId, res);
    expect(res.json).toHaveBeenCalledWith({ sessionId: sessionId });
  });

  test('create connection', async () => {
    await httpHandler.createConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId, polite: true });
  });

  test('delete connection', async () => {
    await httpHandler.deleteConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId });
  });

  test('delete session', async () => {
    const req = getMockReq({ header: headerMock });
    await httpHandler.deleteSession(req, res)
    expect(res.sendStatus).toBeCalledWith(200);
  });
});

describe('http signaling test in private mode', () => {
  const sessionId = "abcd1234";
  const headerMock = (): string => sessionId;
  const connectionId = "12345";

  const { res, mockClear } = getMockRes()
  const req = getMockReq({ header: headerMock, body: { connectionId: connectionId } });

  beforeAll(() => {
    httpHandler.reset("private");
  });

  beforeEach(() => {
    mockClear();
  });

  test('create session', async () => {
    await httpHandler.createSession(sessionId, res);
    expect(res.json).toHaveBeenCalledWith({ sessionId: sessionId });
  });

  test('create connection', async () => {
    await httpHandler.createConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId, polite: false });
  });

  test('delete connection', async () => {
    await httpHandler.deleteConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId });
  });

  test('delete session', async () => {
    const req = getMockReq({ header: headerMock });
    await httpHandler.deleteSession(req, res)
    expect(res.sendStatus).toBeCalledWith(200);
  });
});



describe('websocket signaling test in public mode', () => {
  let server: WS;
  let client: WebSocket;
  const connectionId = "12345";

  beforeAll(async () => {
    wsHandler.reset("public");
    server = new WS("ws://localhost:1234", { jsonProtocol: true });
    client = new WebSocket("ws://localhost:1234");
    await server.connected;
  });

  afterAll(() => {
    WS.clean();
  });

  test('create session', async () => {
    await wsHandler.add(client);
  });

  test('create connection', async () => {
    await wsHandler.onConnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "connect", connectionId: connectionId, polite: true });
  });

  test('delete connection', async () => {
    await wsHandler.onDisconnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
  });

  test('delete session', async () => {
    await wsHandler.remove(client);
  });
});

describe('websocket signaling test in private mode', () => {
  let server: WS;
  let client: WebSocket;
  const connectionId = "12345";

  beforeAll(async () => {
    wsHandler.reset("private");
    server = new WS("ws://localhost:1234", { jsonProtocol: true });
    client = new WebSocket("ws://localhost:1234");
    await server.connected;
  });

  afterAll(() => {
    WS.clean();
  });

  test('create session', async () => {
    await wsHandler.add(client);
  });

  test('create connection', async () => {
    await wsHandler.onConnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "connect", connectionId: connectionId, polite: false });
  });

  test('delete connection', async () => {
    await wsHandler.onDisconnect(client, connectionId);
    await expect(server).toReceiveMessage({ type: "disconnect", connectionId: connectionId });
  });

  test('delete session', async () => {
    await wsHandler.remove(client);
  });
});
