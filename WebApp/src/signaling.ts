import { Request, Response, Router } from 'express';
import { v4 as uuid } from 'uuid';

const express = require('express');
const router: Router = express.Router();

const clients: Map<string, Set<string>> = new Map<string, Set<string>>();    // key = sessionId
const connectionPair: Map<string, [string, string]> = new Map<string, [string, string]>(); // key = connectionId

const offers: Map<string, string> = new Map<string, string>();
const answers: Map<string, string> = new Map<string,string>();
const candidates: Map<string, Map<string, Array<any>>> = new Map<string, Map<string, Array<any>>>();

router.use((req: Request, res: Response, next) => {
  if (req.url == '/') {
    next();
    return;
  }
  const id : string = req.header('session-id');
  if (!clients.has(id)) {
    res.sendStatus(404);
    return;
  }
  next();
});

router.get('/offer', (req: Request, res: Response) => {
  const _offers = Array.from(offers);
  const obj = _offers.map(v => { return { "connectionId" :v[0], "sdp": v[1], }});
  res.json({ offers : obj });
});

router.get('/answer', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  let connectionIds = Array.from(clients.get(sessionId));
  connectionIds = connectionIds.filter(v => answers.has(v));
  const _answers = connectionIds.map(v => { return [v, answers.get(v)] });
  const obj = _answers.map(v => { return { "connectionId" :v[0], "sdp": v[1], }});
  res.json({ answers: obj });
});

router.get('/candidate', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  let connectionIds = Array.from(clients.get(sessionId));

  let arr = new Array();

  for(let connectionId of connectionIds)
  {
    let pair = connectionPair.get(connectionId);
    const otherSessionId = sessionId == pair[0] ? pair[1] : pair[0];
    if(!candidates.get(otherSessionId) || !candidates.get(otherSessionId).get(connectionId)) {
      continue;
    }
    const _candidates = candidates.get(otherSessionId).get(connectionId);
    arr.push({'connectionId':connectionId, 'candidates':_candidates });
  }
  res.json({ candidates : arr });
});

router.put('', (req: Request, res: Response) => {
  const id: string = uuid();
  clients.set(id, new Set<string>());
  res.json({ sessionId : id });
});

router.delete('', (req: Request, res: Response) => {
  const id : string = req.header('session-id');
  clients.delete(id);
  res.sendStatus(200);
});

router.post('/offer', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  let connectionIds = clients.get(sessionId);
  const connectionId : string = uuid();
  connectionIds.add(connectionId);
  offers.set(connectionId, req.body.sdp);
  connectionPair.set(connectionId, [sessionId, null]);

  res.json({ connectionId : connectionId });
});

router.post('/answer', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = req.body.connectionId;
  answers.set(connectionId, req.body.sdp);

  let connectionIds = clients.get(sessionId);
  connectionIds.add(connectionId);

  let pair = connectionPair.get(connectionId);
  connectionPair.set(connectionId, [pair[0], sessionId]);
  res.sendStatus(200);
});

router.post('/candidate', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = req.body.connectionId;
  if(!candidates.has(sessionId)) {
    let _map = new Map<string, Array<string>>();
    candidates.set(sessionId, _map);
  }
  let map = candidates.get(sessionId);
  if(!map.has(connectionId)) {
    let _arr = new Array<string>();
    map.set(connectionId, _arr);
  }
  let arr = map.get(connectionId);
  const value = {
    'candidate' : req.body.candidate,
    'sdpMLineIndex' : req.body.sdpMLineIndex,
    'sdpMid' : req.body.sdpMid

  }
  arr.push(value);

  res.sendStatus(200);
});

export default router;
