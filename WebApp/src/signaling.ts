import { Request, Response, Router } from 'express';
import { v4 as uuid } from 'uuid';

const express = require('express');
const router: Router = express.Router();

class Offer {
  sdp: string;
  datetime: number;
  constructor(sdp: string, datetime: number) {
    this.sdp = sdp;
    this.datetime = datetime;
  }
}

class Answer {
  sdp: string;
  datetime: number;
  constructor(sdp: string, datetime: number) {
    this.sdp = sdp;
    this.datetime = datetime;
  }
}

class Candidate {
  candidate: string;
  sdpMLineIndex: number;
  sdpMid: string;
  datetime: number;
  constructor(candidate: string, sdpMLineIndex: number, sdpMid:string, datetime: number) {
    this.candidate = candidate;
    this.sdpMLineIndex = sdpMLineIndex;
    this.sdpMid = sdpMid;
    this.datetime = datetime;
  }
}

// [{sessonId:[connectionId,...]}]
const clients: Map<string, Set<string>> = new Map<string, Set<string>>();

// [{connectionId:[sessionId1, sessionId2]}]
const connectionPair: Map<string, [string, string]> = new Map<string, [string, string]>(); // key = connectionId

// [{connectionId:Offer}]
const offers: Map<string, Offer> = new Map<string, Offer>();

// [{connectionId:Answer}]
const answers: Map<string, Answer> = new Map<string, Answer>();

// [{sessionId:[{connectionId:Candidate},...]}]
const candidates: Map<string, Map<string, Array<Candidate>>> = new Map<string, Map<string, Array<Candidate>>>(); // key = sessionId

function getOrCreateConnectionIds(sessionId) : Set<string> {
  let connectionIds = null;
  if(!clients.has(sessionId)) {
    connectionIds = new Set<string>();
    clients.set(sessionId, connectionIds);
  }
  connectionIds = clients.get(sessionId);
  return connectionIds;
}

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
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;

  let _offers = Array.from(offers);
  if(fromTime > 0) {
    _offers = _offers.filter(v => v[1].datetime > fromTime);
  }
  const obj = _offers.map(v => { return { "connectionId" :v[0], "sdp": v[1].sdp }});
  res.json({ offers : obj });
});

router.get('/answer', (req: Request, res: Response) => {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;

  const sessionId : string = req.header('session-id');
  let connectionIds = Array.from(clients.get(sessionId));
  connectionIds = connectionIds.filter(v => answers.has(v));

  let arr = [];
  for(let connectionId of connectionIds) {
    const answer = answers.get(connectionId);
    if(answer.datetime > fromTime) {
      arr.push({ "connectionId" :connectionId, "sdp": answer.sdp });
    }
  }
  res.json({ answers: arr });
});

router.get('/candidate', (req: Request, res: Response) => {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;
  const sessionId : string = req.header('session-id');
  let connectionIds = Array.from(clients.get(sessionId));
  let arr = [];
  for(let connectionId of connectionIds) {
    let pair = connectionPair.get(connectionId);
    if(pair == null) {
      continue;
    }
    const otherSessionId = sessionId == pair[0] ? pair[1] : pair[0];
    if(!candidates.get(otherSessionId) || !candidates.get(otherSessionId).get(connectionId)) {
      continue;
    }
    let _candidates = candidates.get(otherSessionId).get(connectionId)
      .filter(v => v.datetime > fromTime)
      .map(v => { return {'candidate':v.candidate, 'sdpMLineIndex':v.sdpMLineIndex, 'sdpMid':v.sdpMid} });
    if(_candidates.length == 0) {
      continue;
    }
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

router.put('/connection', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = uuid();
  const connectionIds = getOrCreateConnectionIds(sessionId);
  connectionIds.add(connectionId);
  res.json({ connectionId : connectionId });
});

router.delete('/connection', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = req.body.connectionId;
  const connectionIds = clients.get(sessionId);
  connectionIds.delete(connectionId);
  connectionPair.delete(connectionId);
  res.sendStatus(200);
});

router.post('/offer', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = req.body.connectionId;
  offers.set(connectionId, new Offer(req.body.sdp, Date.now()));
  connectionPair.set(connectionId, [sessionId, null]);
  res.sendStatus(200);
});

router.post('/answer', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = req.body.connectionId;
  const connectionIds = getOrCreateConnectionIds(sessionId);
  connectionIds.add(connectionId);
  answers.set(connectionId, new Answer(req.body.sdp, Date.now()));

  // add connectionPair
  let pair = connectionPair.get(connectionId);
  const otherSessionId = pair[0];
  connectionPair.set(connectionId, [otherSessionId, sessionId]);

  // update datetime for candidates
  const _candidates = candidates.get(otherSessionId).get(connectionId);
  for(let candidate of _candidates) {
    candidate.datetime = Date.now()
  }
  res.sendStatus(200);
});

router.post('/candidate', (req: Request, res: Response) => {
  const sessionId : string = req.header('session-id');
  const connectionId : string = req.body.connectionId;

  if(!candidates.has(sessionId)) {
    candidates.set(sessionId, new Map<string, Array<Candidate>>());
  }
  let map = candidates.get(sessionId);
  if(!map.has(connectionId)) {
    map.set(connectionId, new Array<Candidate>());
  }
  let arr = map.get(connectionId);
  const candidate = new Candidate(req.body.candidate, req.body.sdpMLineIndex, req.body.sdpMid, Date.now())
  arr.push(candidate);
  res.sendStatus(200);
});

export default router;
