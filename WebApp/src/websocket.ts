import * as websocket from "ws";
import { Server } from 'http';
import Offer from './class/offer';
import Answer from './class/answer';
import Candidate from './class/candidate';

// [{sessonId:[connectionId,...]}]
const clients: Map<WebSocket, Set<string>> = new Map<WebSocket, Set<string>>();

// [{connectionId:[sessionId1, sessionId2]}]
const connectionPair: Map<string, [WebSocket, WebSocket]> = new Map<string, [WebSocket, WebSocket]>();

// [{connectionId:Offer}]
const offers: Map<string, Offer> = new Map<string, Offer>();

// [{connectionId:Answer}]
const answers: Map<string, Answer> = new Map<string, Answer>();

// [{sessionId:[{connectionId:Candidate},...]}]
const candidates: Map<WebSocket, Map<string, Candidate[]>> = new Map<WebSocket, Map<string, Candidate[]>>();

function getOrCreateConnectionIds(settion: WebSocket): Set<string> {
  let connectionIds = null;
  if (!clients.has(settion)) {
    connectionIds = new Set<string>();
    clients.set(settion, connectionIds);
  }
  connectionIds = clients.get(settion);
  return connectionIds;
}

export default class WSSignaling {
  server: Server;
  wss: websocket.Server;
  isPrivate: boolean;

  constructor(server: Server, mode: string) {
    this.server = server;
    this.wss = new websocket.Server({ server });
    this.isPrivate = mode == "private";

    this.wss.on('connection', (ws: WebSocket) => {

      clients.set(ws, new Set<string>());

      ws.onclose = (_event: CloseEvent) => {

        const connectionIds = clients.get(ws);
        connectionIds.forEach(connectionId => {
          connectionPair.delete(connectionId);
          offers.delete(connectionId);
          answers.delete(connectionId);
        });

        clients.delete(ws);
        candidates.delete(ws);
      }

      ws.onmessage = (event: MessageEvent) => {

        // type: connect, disconnect JSON Schema
        // connectionId: connect or disconnect connectionId

        // type: offer, answer, candidate JSON Schema
        // from: from connection id
        // to: to connection id
        // data: any message data structure

        const msg = JSON.parse(event.data);
        if (!msg || !this) {
          return;
        }

        console.log(msg);

        switch (msg.type) {
          case "connect":
            this.onConnect(ws, msg.connectionId);
            break;
          case "disconnect":
            this.onDisconnect(ws, msg.connectionId);
            break;
          case "offer":
            this.onOffer(ws, msg.data);
            break;
          case "answer":
            this.onAnswer(ws, msg.data);
            break;
          case "candidate":
            this.onCandidate(ws, msg.data);
            break;
          default:
            break;
        }
      };
    });
  }

  private onConnect(ws: WebSocket, connectionId: string) {
    let peerExists = false;
    if (this.isPrivate) {
      if (connectionPair.has(connectionId)) {
        const pair = connectionPair.get(connectionId);

        if (pair[0] != null && pair[1] != null) {
          ws.send(JSON.stringify({ type: "error", message: `${connectionId}: This connection id is already used.` }));
          return;
        } else if (pair[0] != null) {
          connectionPair.set(connectionId, [pair[0], ws]);
          peerExists = true;
        }
      } else {
        connectionPair.set(connectionId, [ws, null]);
      }
    }

    const connectionIds = getOrCreateConnectionIds(ws);
    connectionIds.add(connectionId);
    ws.send(JSON.stringify({ type: "connect", connectionId: connectionId, peerExists: peerExists }));
  }

  private onDisconnect(ws: WebSocket, connectionId: string) {
    const connectionIds = clients.get(ws);
    connectionIds.delete(connectionId);

    if (connectionPair.has(connectionId)) {
      const pair = connectionPair.get(connectionId);
      const otherSessionWs = pair[0] == ws ? pair[1] : pair[0];
      if (otherSessionWs) {
        otherSessionWs.send(JSON.stringify({ type: "disconnect", connectionId: connectionId }));
      }
    }
    connectionPair.delete(connectionId);
  }

  private onOffer(ws: WebSocket, message: any) {
    const connectionId = message.connectionId as string;
    const newOffer = new Offer(message.sdp, Date.now());
    offers.set(connectionId, newOffer);

    if (this.isPrivate) {
      const pair = connectionPair.get(connectionId);
      const otherSessionWs = pair[0] == ws ? pair[1] : pair[0];
      if (otherSessionWs) {
        otherSessionWs.send(JSON.stringify({ from: connectionId, to: "", type: "offer", data: newOffer }));
      } else {
        ws.send(JSON.stringify({ type: "error", message: `${connectionId}: This connection id is not ready other session.` }));
      }
      return;
    }

    connectionPair.set(connectionId, [ws, null]);
    clients.forEach((_v, k) => {
      if (k == ws) {
        return;
      }
      k.send(JSON.stringify({ from: connectionId, to: "", type: "offer", data: newOffer }));
    });
  }

  private onAnswer(ws: WebSocket, message: any) {
    const connectionId = message.connectionId as string;
    const connectionIds = getOrCreateConnectionIds(ws);
    connectionIds.add(connectionId);
    const newAnswer = new Answer(message.sdp, Date.now());
    answers.set(connectionId, newAnswer);

    let otherSessionWs = null;

    if (this.isPrivate) {
      const pair = connectionPair.get(connectionId);
      otherSessionWs = pair[0] == ws ? pair[1] : pair[0];
    } else {
      const pair = connectionPair.get(connectionId);
      otherSessionWs = pair[0];
      connectionPair.set(connectionId, [otherSessionWs, ws]);
    }

    const mapCandidates = candidates.get(otherSessionWs);
    if (mapCandidates) {
      const arrayCandidates = mapCandidates.get(connectionId);
      for (const candidate of arrayCandidates) {
        candidate.datetime = Date.now();
      }
    }

    if (this.isPrivate) {
      otherSessionWs.send(JSON.stringify({ from: connectionId, to: "", type: "answer", data: newAnswer }));
      return;
    }

    clients.forEach((_v, k) => {
      if (k == ws) {
        return;
      }
      k.send(JSON.stringify({ from: connectionId, to: "", type: "answer", data: newAnswer }));
    });
  }

  private onCandidate(ws: WebSocket, message: any) {
    const connectionId = message.connectionId;

    if (!candidates.has(ws)) {
      candidates.set(ws, new Map<string, Candidate[]>());
    }
    const map = candidates.get(ws);
    if (!map.has(connectionId)) {
      map.set(connectionId, []);
    }
    const arr = map.get(connectionId);
    const candidate = new Candidate(message.candidate, message.sdpMLineIndex, message.sdpMid, Date.now());
    arr.push(candidate);

    if (this.isPrivate) {
      const pair = connectionPair.get(connectionId);
      const otherSessionWs = pair[0] == ws ? pair[1] : pair[0];
      if (otherSessionWs) {
        otherSessionWs.send(JSON.stringify({ from: connectionId, to: "", type: "candidate", data: candidate }));
      }
      return;
    }

    clients.forEach((_v, k) => {
      if (k === ws) {
        return;
      }
      k.send(JSON.stringify({ from: connectionId, to: "", type: "candidate", data: candidate }));
    });
  }
}
