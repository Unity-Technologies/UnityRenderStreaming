import { getMockReq, getMockRes } from '@jest-mock/express';
import * as httpHandler from '../src/class/httphandler';

describe('http signaling test in public mode', () => {
  const sessionId = "abcd1234";
  const sessionId2 = "abcd5678";
  const connectionId = "12345";
  const connectionId2 = "67890";
  const testsdp = "test sdp";

  const { res, next, mockClear } = getMockRes()
  const req = getMockReq({ header: (): string => sessionId });
  const req2 = getMockReq({ header: (): string => sessionId2 });

  beforeAll(() => {
    httpHandler.reset("public");
  });

  beforeEach(() => {
    mockClear();
  });

  test('throw check has session', async () => {
    httpHandler.checkSessionId(req, res, next);
    expect(res.sendStatus).toBeCalledWith(404);
    expect(next).not.toBeCalled();
  });

  test('create session', async () => {
    await httpHandler.createSession(sessionId, res);
    expect(res.json).toBeCalledWith({ sessionId: sessionId });
  });

  test('create session2', async () => {
    await httpHandler.createSession(sessionId2, res);
    expect(res.json).toBeCalledWith({ sessionId: sessionId2 });
  });

  test('create connection from session1', async () => {
    const body = { connectionId: connectionId };
    req.body = body;
    await httpHandler.createConnection(req, res);
    expect(res.json).toBeCalledWith({ connectionId: connectionId, polite: true });
  });

  test('create connection from session2', async () => {
    const body = { connectionId: connectionId2 };
    req2.body = body;
    await httpHandler.createConnection(req2, res);
    expect(res.json).toBeCalledWith({ connectionId: connectionId2, polite: true });
  });

  test('get connection from session1', async () => {
    await httpHandler.getConnection(req, res);
    expect(res.json).toBeCalledWith({ connections: [{ connectionId: connectionId }] });
  });

  test('post offer from session1', async () => {
    const body = { connectionId: connectionId, sdp: testsdp };
    req.body = body;
    await httpHandler.postOffer(req, res);
    expect(res.sendStatus).toBeCalledWith(200);
  });

  test('get offer from session1', async () => {
    await httpHandler.getOffer(req, res);
    expect(res.json).toBeCalledWith({ offers: [] });
  });

  test('get offer from session2', async () => {
    await httpHandler.getOffer(req2, res);
    expect(res.json).toBeCalledWith({ offers: [{ connectionId: connectionId, sdp: testsdp, polite: false }] });
  });

  test('post answer from session2', async () => {
    const body = { connectionId: connectionId, sdp: testsdp };
    req2.body = body;
    await httpHandler.postAnswer(req2, res);
    expect(res.sendStatus).toBeCalledWith(200);
  });

  test('get answer from session1', async () => {
    await httpHandler.getAnswer(req, res);
    expect(res.json).toBeCalledWith({ answers: [{ connectionId: connectionId, sdp: testsdp }] });
  });

  test('get answer from session2', async () => {
    await httpHandler.getAnswer(req2, res);
    expect(res.json).toBeCalledWith({ answers: [] });
  });

  test('post candidate from sesson1', async () => {
    const body = { connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: 0 };
    req.body = body;
    await httpHandler.postCandidate(req, res);
    expect(res.sendStatus).toBeCalledWith(200);
  });

  test('get candidate from session1', async () => {
    await httpHandler.getCandidate(req, res);
    expect(res.json).toBeCalledWith({ candidates: [] });
  });

  test('get candidate from session2', async () => {
    await httpHandler.getCandidate(req2, res);
    expect(res.json).toBeCalledWith({ candidates: [{ connectionId, candidates: [{ candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: 0 }] }] });
  });

  test('delete connection from session2', async () => {
    const body = { connectionId: connectionId };
    req2.body = body;
    await httpHandler.deleteConnection(req2, res);
    expect(res.json).toBeCalledWith({ connectionId: connectionId });
  });

  test('get connection from session1', async () => {
    await httpHandler.getConnection(req, res);
    expect(res.json).toBeCalledWith({ connections: [] });
  });

  test('delete connection from session1', async () => {
    const body = { connectionId: connectionId };
    req.body = body;
    await httpHandler.deleteConnection(req, res);
    expect(res.json).toBeCalledWith({ connectionId: connectionId });
  });

  test('delete session1', async () => {
    const req = getMockReq({ header: (): string => sessionId });
    await httpHandler.deleteSession(req, res)
    expect(res.sendStatus).toBeCalledWith(200);
  });

  test('delete session2', async () => {
    const req2 = getMockReq({ header: (): string => sessionId2 });
    await httpHandler.deleteSession(req2, res)
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
