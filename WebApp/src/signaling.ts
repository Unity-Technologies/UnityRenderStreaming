import { Request, Response, Router } from 'express';
import { v4 as uuid } from 'uuid';
import Offer from './class/offer';
import Answer from './class/answer';
import Candidate from './class/candidate';

const express = require('express');

const router: Router = express.Router();

// [{sessonId:[connectionId,...]}]
const clients: Map<string, Set<string>> = new Map<string, Set<string>>();

// [{connectionId:[sessionId1, sessionId2]}]
const connectionPair: Map<string, [string, string]> = new Map<string, [string, string]>(); // key = connectionId

// [{sessionId:[{connectionId:Offer},...]}]
const offers: Map<string, Map<string, Offer>> = new Map<string, Map<string, Offer>>();

// [{sessionId:[{connectionId:Answer},...]}]
const answers: Map<string, Map<string, Answer>> = new Map<string, Map<string, Answer>>();

// [{sessionId:[{connectionId:Candidate},...]}]
const candidates: Map<string, Map<string, Candidate[]>> = new Map<string, Map<string, Candidate[]>>(); // key = sessionId

function getOrCreateConnectionIds(sessionId): Set<string> {
  let connectionIds = null;
  if (!clients.has(sessionId)) {
    connectionIds = new Set<string>();
    clients.set(sessionId, connectionIds);
  }
  connectionIds = clients.get(sessionId);
  return connectionIds;
}

router.use((req: Request, res: Response, next) => {
  if (req.url === '/') {
    next();
    return;
  }
  const id: string = req.header('session-id');
  if (!clients.has(id)) {
    res.sendStatus(404);
    return;
  }
  next();
});

router.get('/offer', (req: Request, res: Response) => {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;
  const sessionId: string = req.header('session-id');

  const response = [];
  offers.forEach((offerMap, keySessionId) => {
    if (keySessionId == sessionId) {
      return;
    }

    offerMap.forEach((offer, keyConnectionId) => {
      if (req.app.get('isPrivate') && !clients.get(sessionId).has(keyConnectionId)) {
        return;
      }

      if (fromTime > 0) {
        if (offer.datetime > fromTime) {
          response.push({ connectionId: keyConnectionId, sdp: offer.sdp });
        }
      } else {
        response.push({ connectionId: keyConnectionId, sdp: offer.sdp });
      }
    });
  });

  res.json({ offers: response });
});

router.get('/answer', (req: Request, res: Response) => {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;

  const sessionId: string = req.header('session-id');
  const connectionIds = clients.get(sessionId);

  const response = [];
  answers.forEach((answerMap, keySessionId) => {
    if (keySessionId == sessionId) {
      return;
    }
    answerMap.forEach((answer, keyConnectionId) => {
      if (!connectionIds.has(keyConnectionId)) {
        return;
      }
      if (fromTime > 0) {
        if (answer.datetime > fromTime) {
          response.push({ connectionId: keyConnectionId, sdp: answer.sdp });
        }
      } else {
        response.push({ connectionId: keyConnectionId, sdp: answer.sdp });
      }
    });
  });

  res.json({ answers: response });
});

router.get('/candidate', (req: Request, res: Response) => {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;
  const sessionId: string = req.header('session-id');
  const connectionIds = Array.from(clients.get(sessionId));
  const arr = [];
  for (const connectionId of connectionIds) {
    const pair = connectionPair.get(connectionId);
    if (pair == null) {
      continue;
    }
    const otherSessionId = sessionId === pair[0] ? pair[1] : pair[0];
    if (!candidates.get(otherSessionId) || !candidates.get(otherSessionId).get(connectionId)) {
      continue;
    }
    const arrayCandidates = candidates.get(otherSessionId).get(connectionId)
      .filter((v) => v.datetime > fromTime)
      .map((v) => ({ candidate: v.candidate, sdpMLineIndex: v.sdpMLineIndex, sdpMid: v.sdpMid }));
    if (arrayCandidates.length === 0) {
      continue;
    }
    arr.push({ connectionId, candidates: arrayCandidates });
  }
  res.json({ candidates: arr });
});

router.put('', (req: Request, res: Response) => {
  const id: string = uuid();
  clients.set(id, new Set<string>());
  res.json({ sessionId: id });
});

router.delete('', (req: Request, res: Response) => {
  const id: string = req.header('session-id');
  const connectionIds = clients.get(id);
  if (connectionIds) {
    connectionIds.forEach(connectionId => {
      connectionPair.delete(connectionId);
      offers.delete(connectionId);
      answers.delete(connectionId);
    });
  }
  candidates.delete(id);
  clients.delete(id);
  res.sendStatus(200);
});

router.put('/connection', (req: Request, res: Response) => {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  if(connectionId == null) {
    res.status(400).send({ error: new Error(`connectionId is required`) });
    return;
  }
  let peerExists = false;
  if (req.app.get('isPrivate')) {
    if (connectionPair.has(connectionId)) {
      const pair = connectionPair.get(connectionId);

      if (pair[0] != null && pair[1] != null) {
        const err = new Error(`${connectionId}: This connection id is already used.`);
        console.log(err);
        res.status(400).send({ error: err });
        return;
      } else if (pair[0] != null) {
        connectionPair.set(connectionId, [pair[0], sessionId]);
        peerExists = true;
      }
    } else {
      connectionPair.set(connectionId, [sessionId, null]);
    }
  }

  const connectionIds = getOrCreateConnectionIds(sessionId);
  connectionIds.add(connectionId);
  res.json({ connectionId: connectionId, peerExists: peerExists });
});

router.delete('/connection', (req: Request, res: Response) => {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  const connectionIds = clients.get(sessionId);
  connectionIds.delete(connectionId);
  connectionPair.delete(connectionId);
  res.sendStatus(200);
});

router.post('/offer', (req: Request, res: Response) => {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;

  if (!offers.has(sessionId)) {
    offers.set(sessionId, new Map<string, Offer>());
  }
  const map = offers.get(sessionId);
  map.set(connectionId, new Offer(req.body.sdp, Date.now()));

  if (res.app.get('isPrivate')) {
    const pair = connectionPair.get(connectionId);
    const otherSessionId = pair[0] == sessionId ? pair[1] : pair[0];
    if (otherSessionId == null) {
      const err = new Error(`${connectionId}: This connection id is not ready other session.`);
      console.log(err);
      res.status(400).send({ error: err });
      return;
    }
  } else {
    connectionPair.set(connectionId, [sessionId, null]);
  }

  res.sendStatus(200);
});

router.post('/answer', (req: Request, res: Response) => {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  const connectionIds = getOrCreateConnectionIds(sessionId);
  connectionIds.add(connectionId);

  if (!answers.has(sessionId)) {
    answers.set(sessionId, new Map<string, Answer>());
  }
  const map = answers.get(sessionId);
  map.set(connectionId, new Answer(req.body.sdp, Date.now()));

  // add connectionPair
  const pair = connectionPair.get(connectionId);
  const otherSessionId = pair[0];

  if (!res.app.get('isPrivate')) {
    connectionPair.set(connectionId, [otherSessionId, sessionId]);
  }

  // update datetime for candidates
  const mapCandidates = candidates.get(otherSessionId);
  if (mapCandidates) {
    const arrayCandidates = mapCandidates.get(connectionId);
    for (const candidate of arrayCandidates) {
      candidate.datetime = Date.now();
    }
  }
  res.sendStatus(200);
});

router.post('/candidate', (req: Request, res: Response) => {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;

  if (!candidates.has(sessionId)) {
    candidates.set(sessionId, new Map<string, Candidate[]>());
  }
  const map = candidates.get(sessionId);
  if (!map.has(connectionId)) {
    map.set(connectionId, []);
  }
  const arr = map.get(connectionId);
  const candidate = new Candidate(req.body.candidate, req.body.sdpMLineIndex, req.body.sdpMid, Date.now());
  arr.push(candidate);
  res.sendStatus(200);
});

export default router;
