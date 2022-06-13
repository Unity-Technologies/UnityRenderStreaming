import { Request, Response } from 'express';
import Offer from './offer';
import Answer from './answer';
import Candidate from './candidate';

let isPrivate: boolean;

// [{sessonId:[connectionId,...]}]
const clients: Map<string, Set<string>> = new Map<string, Set<string>>();

// [{connectionId:[sessionId1, sessionId2]}]
const connectionPair: Map<string, [string, string]> = new Map<string, [string, string]>(); // key = connectionId

// [{sessionId:[{connectionId:Offer},...]}]
const offers: Map<string, Map<string, Offer>> = new Map<string, Map<string, Offer>>(); // key = sessionId

// [{sessionId:[{connectionId:Answer},...]}]
const answers: Map<string, Map<string, Answer>> = new Map<string, Map<string, Answer>>(); // key = sessionId

// [{sessionId:[{connectionId:Candidate},...]}]
const candidates: Map<string, Map<string, Candidate[]>> = new Map<string, Map<string, Candidate[]>>(); // key = sessionId

function getOrCreateConnectionIds(sessionId: string): Set<string> {
  let connectionIds = null;
  if (!clients.has(sessionId)) {
    connectionIds = new Set<string>();
    clients.set(sessionId, connectionIds);
  }
  connectionIds = clients.get(sessionId);
  return connectionIds;
}

function reset(mode: string): void {
  isPrivate = mode == "private";
  clients.clear();
  connectionPair.clear();
  offers.clear();
  answers.clear();
  candidates.clear();
}

function checkSessionId(req: Request, res: Response, next): void {
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
}

function getConnection(req: Request, res: Response): void {
  const sessionId: string = req.header('session-id');
  const arrayConnection = Array.from(clients.get(sessionId));
  const obj = arrayConnection.map((v) => ({ connectionId: v }));
  res.json({ connections: obj });
}

function getOffer(req: Request, res: Response): void {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;
  const sessionId: string = req.header('session-id');
  let arrayOffers: [string, Offer][] = [];

  if (offers.size != 0) {
    if (isPrivate) {
      if (offers.has(sessionId)) {
        arrayOffers = Array.from(offers.get(sessionId));
      }
    } else {
      const otherSessionMap = Array.from(offers).filter(x => x[0] != sessionId);
      arrayOffers = [].concat(...Array.from(otherSessionMap, x => Array.from(x[1], y => [y[0], y[1]])));
    }
  }

  if (fromTime > 0) {
    arrayOffers = arrayOffers.filter((v) => v[1].datetime > fromTime);
  }
  const obj = arrayOffers.map((v) => ({ connectionId: v[0], sdp: v[1].sdp, polite: v[1].polite }));
  res.json({ offers: obj });
}

function getAnswer(req: Request, res: Response): void {
  // get `fromtime` parameter from request query
  const fromTime: number = req.query.fromtime ? Number(req.query.fromtime) : 0;
  const sessionId: string = req.header('session-id');
  let arrayOffers: [string, Answer][] = [];

  if (answers.size != 0 && answers.has(sessionId)) {
    arrayOffers = Array.from(answers.get(sessionId));
  }

  if (fromTime > 0) {
    arrayOffers = arrayOffers.filter((v) => v[1].datetime > fromTime);
  }
  const obj = arrayOffers.map((v) => ({ connectionId: v[0], sdp: v[1].sdp }));
  res.json({ answers: obj });
}

function getCandidate(req: Request, res: Response): void {
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
}

function createSession(sessionId: string, res: Response): void {
  clients.set(sessionId, new Set<string>());
  offers.set(sessionId, new Map<string, Offer>());
  answers.set(sessionId, new Map<string, Answer>());
  candidates.set(sessionId, new Map<string, Candidate[]>());
  res.json({ sessionId: sessionId });
}

function deleteSession(req: Request, res: Response): void {
  const id: string = req.header('session-id');
  offers.delete(id);
  answers.delete(id);
  candidates.delete(id);
  clients.delete(id);
  res.sendStatus(200);
}

function createConnection(req: Request, res: Response): void {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  if (connectionId == null) {
    res.status(400).send({ error: new Error(`connectionId is required`) });
    return;
  }
  let polite = true;
  if (isPrivate) {
    if (connectionPair.has(connectionId)) {
      const pair = connectionPair.get(connectionId);

      if (pair[0] != null && pair[1] != null) {
        const err = new Error(`${connectionId}: This connection id is already used.`);
        console.log(err);
        res.status(400).send({ error: err });
        return;
      } else if (pair[0] != null) {
        connectionPair.set(connectionId, [pair[0], sessionId]);
        const map = getOrCreateConnectionIds(pair[0]);
        map.add(connectionId);
      }
    } else {
      connectionPair.set(connectionId, [sessionId, null]);
      polite = false;
    }
  }

  const connectionIds = getOrCreateConnectionIds(sessionId);
  connectionIds.add(connectionId);
  res.json({ connectionId: connectionId, polite: polite });
}

function deleteConnection(req: Request, res: Response): void {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  clients.get(sessionId).delete(connectionId);

  if (connectionPair.has(connectionId)) {
    const pair = connectionPair.get(connectionId);
    const otherSessionId = pair[0] == sessionId ? pair[1] : pair[0];
    if (otherSessionId) {
      if (clients.has(otherSessionId)) {
        clients.get(otherSessionId).delete(connectionId);
      }
    }
  }
  connectionPair.delete(connectionId);
  offers.get(sessionId).delete(connectionId);
  answers.get(sessionId).delete(connectionId);
  candidates.get(sessionId).delete(connectionId);
  res.json({ connectionId: connectionId });
}

function postOffer(req: Request, res: Response): void {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  let keySessionId = null;
  let polite = false;

  if (isPrivate) {
    if (connectionPair.has(connectionId)) {
      const pair = connectionPair.get(connectionId);
      keySessionId = pair[0] == sessionId ? pair[1] : pair[0];
      if (keySessionId != null) {
        polite = true;
        const map = offers.get(keySessionId);
        map.set(connectionId, new Offer(req.body.sdp, Date.now(), polite));
      }
    }
    res.sendStatus(200);
    return;
  }

  connectionPair.set(connectionId, [sessionId, null]);
  keySessionId = sessionId;
  const map = offers.get(keySessionId);
  map.set(connectionId, new Offer(req.body.sdp, Date.now(), polite));

  res.sendStatus(200);
}

function postAnswer(req: Request, res: Response): void {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;
  const connectionIds = getOrCreateConnectionIds(sessionId);
  connectionIds.add(connectionId);

  if (!connectionPair.has(connectionId)) {
    res.sendStatus(200);
    return;
  }

  // add connectionPair
  const pair = connectionPair.get(connectionId);
  const otherSessionId = pair[0] == sessionId ? pair[1] : pair[0];

  if (!isPrivate) {
    connectionPair.set(connectionId, [otherSessionId, sessionId]);
  }

  const map = answers.get(otherSessionId);
  map.set(connectionId, new Answer(req.body.sdp, Date.now()));

  // update datetime for candidates
  const mapCandidates = candidates.get(otherSessionId);
  if (mapCandidates) {
    const arrayCandidates = mapCandidates.get(connectionId);
    if (arrayCandidates) {
      for (const candidate of arrayCandidates) {
        candidate.datetime = Date.now();
      }
    }
  }
  res.sendStatus(200);
}

function postCandidate(req: Request, res: Response): void {
  const sessionId: string = req.header('session-id');
  const { connectionId } = req.body;

  const map = candidates.get(sessionId);
  if (!map.has(connectionId)) {
    map.set(connectionId, []);
  }
  const arr = map.get(connectionId);
  const candidate = new Candidate(req.body.candidate, req.body.sdpMLineIndex, req.body.sdpMid, Date.now());
  arr.push(candidate);
  res.sendStatus(200);
}

export { reset, checkSessionId, getConnection, getOffer, getAnswer, getCandidate, createSession, deleteSession, createConnection, deleteConnection, postOffer, postAnswer, postCandidate };
