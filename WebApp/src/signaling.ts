import { Request, Response, Router } from 'express';
import { v4 as uuid } from 'uuid';

const express = require('express');
const router: Router = express.Router();

const clients: Map<string, Set<string>> = new Map<string, Set<string>>();

const offers: Map<string, string> = new Map<string, string>();
const answers: Map<string, string> = new Map<string,string>();
const candidates: Map<string, string> = new Map<string, string>();

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
  connectionIds = connectionIds.filter(v => candidates.has(v));
  const _candidates = connectionIds.map(v => { return [v, candidates.get(v)] });
  const obj = _candidates.map(v => { return { "connectionId" :v[0], "candidate": v[1], }});
  res.json({ candidates : obj });
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
  res.json({ connectionId : connectionId });
});

router.post('/answer', (req: Request, res: Response) => {
  const connectionId : string = req.body.connectionId;
  answers.set(connectionId, req.body.sdp);
  res.sendStatus(200);
});

router.post('/candidate', (req: Request, res: Response) => {
  const connectionId : string = req.body.connectionId;
  candidates.set(connectionId, req.body.candidate);
  res.sendStatus(200);
});

export default router;
