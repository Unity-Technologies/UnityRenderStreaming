import { getMockReq, getMockRes } from '@jest-mock/express';
import * as httpHandler from '../src/class/httphandler';

const RetriesToForceTimeout = 11; // Waits a second each time, timeout is 10 sec for httphandler.

describe('http signaling test in public mode', () => {
  const sessionId = "abcd1234";
  const sessionId2 = "abcd5678";
  const sessionId3 = "abcd9101112";
  const connectionId = "12345";
  const connectionId2 = "67890";
  const testsdp = "test sdp";

  const { res, next, mockClear } = getMockRes();
  const req = getMockReq({ header: jest.fn(() => sessionId) });
  const req2 = getMockReq({ header: jest.fn(() => sessionId2) });
  const req3 = getMockReq({ header: jest.fn(() => sessionId3) });

  beforeAll(() => {
    httpHandler.reset("public");
  });

  beforeEach(() => {
    mockClear();
    httpHandler.checkSessionId(req, res, next);
    httpHandler.checkSessionId(req2, res, next);
  });

  test('throw check has session', async () => {
    httpHandler.checkSessionId(req, res, next);
    expect(res.sendStatus).toHaveBeenCalledWith(404);
    expect(next).not.toHaveBeenCalled();
  });

  test('create session', async () => {
    await httpHandler.createSession(sessionId, res);
    expect(res.json).toHaveBeenCalledWith({ sessionId: sessionId });
  });

  test('create session2', async () => {
    await httpHandler.createSession(sessionId2, res);
    expect(res.json).toHaveBeenCalledWith({ sessionId: sessionId2 });
  });

  test('create connection from session1', async () => {
    const body = { connectionId: connectionId };
    req.body = body;
    await httpHandler.createConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId, polite: true, datetime: expect.anything(), type: "connect" });
  });

  test('create connection from session2', async () => {
    const body = { connectionId: connectionId2 };
    req2.body = body;
    await httpHandler.createConnection(req2, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId2, polite: true, datetime: expect.anything(), type: "connect" });
  });

  test('get connection from session1', async () => {
    await httpHandler.getConnection(req, res);
    const connect = { connectionId: connectionId, datetime: expect.anything(), type: "connect" };
    expect(res.json).toHaveBeenCalledWith({ connections: expect.arrayContaining([connect]) });
  });

  test('get all from session1', async () => {
    await httpHandler.getAll(req, res);
    const connect = { connectionId: connectionId, datetime: expect.anything(), type: "connect" };
    expect(res.json).toHaveBeenCalledWith({ messages: expect.arrayContaining([connect]), datetime: expect.anything() });
  });

  test('post offer from session1', async () => {
    const body = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer" };
    req.body = body;
    await httpHandler.postOffer(req, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('get offer from session1', async () => {
    await httpHandler.getOffer(req, res);
    expect(res.json).toHaveBeenCalledWith({ offers: [] });
  });

  test('get offer from session2', async () => {
    await httpHandler.getOffer(req2, res);
    expect(res.json).toHaveBeenCalledWith({ offers: [{ connectionId: connectionId, sdp: testsdp, polite: false, datetime: expect.anything(), type: "offer" }] });
  });

  test('post answer from session2', async () => {
    const body = { connectionId: connectionId, sdp: testsdp };
    req2.body = body;
    await httpHandler.postAnswer(req2, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('get answer from session1', async () => {
    await httpHandler.getAnswer(req, res);
    expect(res.json).toHaveBeenCalledWith({ answers: [{ connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "answer" }] });
  });

  test('get answer from session2', async () => {
    await httpHandler.getAnswer(req2, res);
    expect(res.json).toHaveBeenCalledWith({ answers: [] });
  });

  test('post candidate from sesson1', async () => {
    const body = { connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: 0 };
    req.body = body;
    await httpHandler.postCandidate(req, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('get candidate from session1', async () => {
    await httpHandler.getCandidate(req, res);
    expect(res.json).toHaveBeenCalledWith({ candidates: [] });
  });

  test('get candidate from session2', async () => {
    await httpHandler.getCandidate(req2, res);
    expect(res.json).toHaveBeenCalledWith({ candidates: [{ connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: 0, type: "candidate", datetime: expect.anything() }] });
  });

  test('delete connection from session2', async () => {
    const body = { connectionId: connectionId };
    req2.body = body;
    await httpHandler.deleteConnection(req2, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId });
  });

  test('disconnection get from session1', async () => {
    await httpHandler.getAll(req, res);
    const disconnect = { connectionId: connectionId, datetime: expect.anything(), type: "disconnect" };
    expect(res.json).toHaveBeenCalledWith({ messages: expect.arrayContaining([disconnect]), datetime: expect.anything() });
  });

  test('delete connection from session1', async () => {
    const body = { connectionId: connectionId };
    req.body = body;
    await httpHandler.deleteConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId });
  });

  test('delete session1', async () => {
    const req = getMockReq({ header: jest.fn(() => sessionId) });
    await httpHandler.deleteSession(req, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('delete session2', async () => {
    const req2 = getMockReq({ header: jest.fn(() => sessionId2) });
    await httpHandler.deleteSession(req2, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('disconnection get when session2 disconnects before session1 answer', async () => {
    httpHandler.reset("public");

    await httpHandler.createSession(sessionId, res);
    await httpHandler.createSession(sessionId2, res);

    await httpHandler.getAll(req, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: [], datetime: expect.anything() });

    const connectBody = { connectionId: connectionId };
    req.body = connectBody;
    await httpHandler.createConnection(req, res);

    const offerBody = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer" };
    req.body = offerBody;
    await httpHandler.postOffer(req, res);

    const offer = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer", polite: false };
    await httpHandler.getAll(req, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.not.arrayContaining([offer]), datetime: expect.anything() });
    await httpHandler.getAll(req2, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.arrayContaining([offer]), datetime: expect.anything() });

    const deleteBody = { connectionId: connectionId };
    req2.body = deleteBody;
    await httpHandler.deleteConnection(req, res);
    await httpHandler.deleteSession(req, res);
    expect(res.sendStatus).toHaveBeenLastCalledWith(200);

    const answerBody = { connectionId: connectionId, sdp: testsdp };
    req2.body = answerBody;
    await httpHandler.postAnswer(req2, res);

    const disconnect = { connectionId: connectionId, type: "disconnect", datetime: expect.anything() };
    await httpHandler.getAll(req2, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.arrayContaining([disconnect]), datetime: expect.anything() });

    await httpHandler.deleteSession(req2, res);
  });

  test('Timed out session2 deleted after session1 resends offer', async () => {
    httpHandler.reset("public");

    await httpHandler.createSession(sessionId, res);
    await httpHandler.createSession(sessionId2, res);

    req.url = "";
    req2.url = "";
    await httpHandler.checkSessionId(req, res, next);
    await httpHandler.checkSessionId(req2, res, next);

    await httpHandler.getAll(req, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: [], datetime: expect.anything() });

    const connectBody = { connectionId: connectionId };
    req.body = connectBody;
    await httpHandler.createConnection(req, res);

    const offerBody = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer" };
    req.body = offerBody;
    await httpHandler.postOffer(req, res);

    const offer = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer", polite: false };
    await httpHandler.getAll(req, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.not.arrayContaining([offer]), datetime: expect.anything() });
    await httpHandler.getAll(req2, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.arrayContaining([offer]), datetime: expect.anything() });

    const answerBody = { connectionId: connectionId, sdp: testsdp };
    req2.body = answerBody;
    await httpHandler.postAnswer(req2, res);

    // resend offer after answer to simulate PeerCandidate entering into failed state
    req.body = offerBody;
    await httpHandler.postOffer(req, res);

    // Wait a second and then checkSession for only session1 to force timeout of session2.
    for (let i = 0; i < RetriesToForceTimeout + 1; ++i)
    {
      await httpHandler.checkSessionId(req, res, next);
      await new Promise(resolve => setTimeout(resolve, 1000));
    }

    // Get all for session1 to trigger cleaning up associated session that timed out.
    await httpHandler.getAll(req, res);

    // Check that we do have session1 still
    await httpHandler.checkSessionId(req, res, next);
    expect(res.sendStatus).toHaveBeenLastCalledWith(200);

    // Check that we no longer have session2
    await httpHandler.checkSessionId(req2, res, next);
    expect(res.sendStatus).toHaveBeenLastCalledWith(404);

    await httpHandler.deleteSession(req, res);
  }, 16000);

test('Timed out sessions are deleted when other sessions check', async () => {
  httpHandler.reset("public");

  await httpHandler.createSession(sessionId, res);
  await httpHandler.createSession(sessionId2, res);
  await httpHandler.createSession(sessionId3, res);

  req.url = "";
  req2.url = "";
  req3.url = "";
  await httpHandler.checkSessionId(req, res, next);
  await httpHandler.checkSessionId(req2, res, next);
  await httpHandler.checkSessionId(req3, res, next);

  await httpHandler.getAll(req, res);
  expect(res.json).toHaveBeenLastCalledWith({ messages: [], datetime: expect.anything() });

  const connectBody = { connectionId: connectionId };
  req.body = connectBody;
  await httpHandler.createConnection(req, res);

  const offerBody = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer" };
  req.body = offerBody;
  await httpHandler.postOffer(req, res);

  const offer = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer", polite: false };
  await httpHandler.getAll(req, res);
  expect(res.json).toHaveBeenLastCalledWith({ messages: expect.not.arrayContaining([offer]), datetime: expect.anything() });
  await httpHandler.getAll(req2, res);
  expect(res.json).toHaveBeenLastCalledWith({ messages: expect.arrayContaining([offer]), datetime: expect.anything() });

  const answerBody = { connectionId: connectionId, sdp: testsdp };
  req2.body = answerBody;
  await httpHandler.postAnswer(req2, res);

  // Wait a second and then checkSession for only session3 to force timeout of session1 & session2.
  for (let i = 0; i < RetriesToForceTimeout + 1; ++i)
  {
    await httpHandler.checkSessionId(req3, res, next);
    await new Promise(resolve => setTimeout(resolve, 1000));
  }

  // Get all for session3 to trigger cleaning up sessions that timed out.
  await httpHandler.getAll(req3, res);

  // Check that we do have session3 still
  await httpHandler.checkSessionId(req3, res, next);
  expect(res.sendStatus).toHaveBeenLastCalledWith(200);

  // Check that we do have session1 still
  await httpHandler.checkSessionId(req, res, next);
  expect(res.sendStatus).toHaveBeenLastCalledWith(404);

  // Check that we no longer have session2
  await httpHandler.checkSessionId(req2, res, next);
  expect(res.sendStatus).toHaveBeenLastCalledWith(404);

  await httpHandler.deleteSession(req3, res);
}, 16000);
});

describe('http signaling test in private mode', () => {
  const sessionId = "abcd1234";
  const sessionId2 = "abcd5678";
  const connectionId = "12345";
  const testsdp = "test sdp";

  const { res, next, mockClear } = getMockRes();
  const req = getMockReq({ header: jest.fn(() => sessionId) });
  const req2 = getMockReq({ header: jest.fn(() => sessionId2) });

  beforeAll(() => {
    httpHandler.reset("private");
  });

  beforeEach(() => {
    mockClear();

    httpHandler.checkSessionId(req, res, next);
    httpHandler.checkSessionId(req2, res, next);
  });

  test('throw check has session', async () => {
    httpHandler.checkSessionId(req, res, next);
    expect(res.sendStatus).toHaveBeenCalledWith(404);
    expect(next).not.toHaveBeenCalled();
  });

  test('create session', async () => {
    await httpHandler.createSession(sessionId, res);
    expect(res.json).toHaveBeenCalledWith({ sessionId: sessionId });
  });

  test('create session2', async () => {
    await httpHandler.createSession(sessionId2, res);
    expect(res.json).toHaveBeenCalledWith({ sessionId: sessionId2 });
  });

  test('create connection from session1', async () => {
    const body = { connectionId: connectionId };
    req.body = body;
    await httpHandler.createConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId, polite: false, datetime: expect.anything(), type: "connect" });
  });

  test('create connection from session2', async () => {
    const body = { connectionId: connectionId };
    req2.body = body;
    await httpHandler.createConnection(req2, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId, polite: true, datetime: expect.anything(), type: "connect" });
  });

  test('response status 400 if connecctionId does not set', async () => {
    const req3 = getMockReq({ header: jest.fn(() => sessionId) });
    await httpHandler.createConnection(req3, res);
    expect(res.status).toHaveBeenCalledWith(400);
    expect(res.send).toHaveBeenCalledWith({ error: new Error(`connectionId is required`) });
  });

  test('response status 400 if aleady used connection', async () => {
    const sessionId3 = "session3";
    await httpHandler.createSession(sessionId3, res);
    const body = { connectionId: connectionId };
    const req3 = getMockReq({ header: jest.fn(() => sessionId3) });
    req3.body = body;
    await httpHandler.createConnection(req3, res);
    expect(res.status).toHaveBeenCalledWith(400);
    expect(res.send).toHaveBeenCalledWith({ error: new Error(`${connectionId}: This connection id is already used.`) });
  });

  test('not connection get from session1', async () => {
    await httpHandler.getConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connections: [{ connectionId: connectionId, datetime: expect.anything(), type: "connect" }] });
  });

  test('post offer from session1', async () => {
    const body = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer" };
    req.body = body;
    await httpHandler.postOffer(req, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('get offer from session1', async () => {
    await httpHandler.getOffer(req, res);
    expect(res.json).toHaveBeenCalledWith({ offers: [] });
  });

  test('get offer from session2', async () => {
    await httpHandler.getOffer(req2, res);
    expect(res.json).toHaveBeenCalledWith({ offers: [{ connectionId: connectionId, sdp: testsdp, polite: true, datetime: expect.anything(), type: "offer" }] });
  });

  test('post answer from session2', async () => {
    const body = { connectionId: connectionId, sdp: testsdp };
    req2.body = body;
    await httpHandler.postAnswer(req2, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('get answer from session1', async () => {
    await httpHandler.getAnswer(req, res);
    expect(res.json).toHaveBeenCalledWith({ answers: [{ connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "answer" }] });
  });

  test('get answer from session2', async () => {
    await httpHandler.getAnswer(req2, res);
    expect(res.json).toHaveBeenCalledWith({ answers: [] });
  });

  test('post candidate from sesson1', async () => {
    const body = { connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: 0 };
    req.body = body;
    await httpHandler.postCandidate(req, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('get candidate from session1', async () => {
    await httpHandler.getCandidate(req, res);
    expect(res.json).toHaveBeenCalledWith({ candidates: [] });
  });

  test('get candidate from session2', async () => {
    await httpHandler.getCandidate(req2, res);
    expect(res.json).toHaveBeenCalledWith({ candidates: [{ connectionId: connectionId, candidate: "testcandidate", sdpMLineIndex: 0, sdpMid: 0, type: "candidate", datetime: expect.anything() }] });
  });

  test('delete connection from session2', async () => {
    const body = { connectionId: connectionId };
    req2.body = body;
    await httpHandler.deleteConnection(req2, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId });
  });

  test('get connection from session1', async () => {
    await httpHandler.getConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connections: [] });
  });

  test('delete connection from session1', async () => {
    const body = { connectionId: connectionId };
    req.body = body;
    await httpHandler.deleteConnection(req, res);
    expect(res.json).toHaveBeenCalledWith({ connectionId: connectionId });
  });

  test('delete session1', async () => {
    const req = getMockReq({ header: jest.fn(() => sessionId) });
    await httpHandler.deleteSession(req, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('delete session2', async () => {
    const req2 = getMockReq({ header: jest.fn(() => sessionId2) });
    await httpHandler.deleteSession(req2, res);
    expect(res.sendStatus).toHaveBeenCalledWith(200);
  });

  test('disconnection get when session2 disconnects before session1 answer', async () => {
    httpHandler.reset("private");

    await httpHandler.createSession(sessionId, res);
    await httpHandler.createSession(sessionId2, res);

    await httpHandler.getAll(req, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: [], datetime: expect.anything() });

    const connectBody = { connectionId: connectionId };
    req.body = connectBody;
    await httpHandler.createConnection(req, res);
    req2.body = connectBody;
    await httpHandler.createConnection(req2, res);

    const offerBody = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer" };
    req.body = offerBody;
    await httpHandler.postOffer(req, res);

    const offer = { connectionId: connectionId, sdp: testsdp, datetime: expect.anything(), type: "offer", polite: true };
    await httpHandler.getAll(req, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.not.arrayContaining([offer]), datetime: expect.anything() });
    await httpHandler.getAll(req2, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.arrayContaining([offer]), datetime: expect.anything() });

    const deleteBody = { connectionId: connectionId };
    req2.body = deleteBody;
    await httpHandler.deleteConnection(req, res);
    await httpHandler.deleteSession(req, res);
    expect(res.sendStatus).toHaveBeenLastCalledWith(200);

    const answerBody = { connectionId: connectionId, sdp: testsdp };
    req2.body = answerBody;
    await httpHandler.postAnswer(req2, res);

    const disconnect = { connectionId: connectionId, type: "disconnect", datetime: expect.anything() };
    await httpHandler.getAll(req2, res);
    expect(res.json).toHaveBeenLastCalledWith({ messages: expect.arrayContaining([disconnect]), datetime: expect.anything() });

    await httpHandler.deleteSession(req2, res);
  });
});
