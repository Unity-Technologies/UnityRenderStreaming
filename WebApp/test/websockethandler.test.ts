import WS from "jest-websocket-mock";
import * as wsHandler from '../src/class/websockethandler';

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
