import * as websocket from "ws";
import { Server } from 'http';
import { v4 as uuid } from 'uuid';
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

    constructor(server: Server) {
        this.server = server;
        this.wss = new websocket.Server({ server });

        this.wss.on('connection', (ws: WebSocket) => {

            clients.set(ws, new Set<string>());

            ws.onclose = (_event: CloseEvent) => {
                let connectionIds = clients.get(ws);
                connectionIds.forEach(connectionId => {
                    connectionPair.delete(connectionId);
                    offers.delete(connectionId);
                    answers.delete(connectionId);
                })
                candidates.delete(ws);
                clients.delete(ws);
            }

            ws.onmessage = (event: MessageEvent) => {

                // JSON Schema expectation
                // type: connect, disconnect, offer, answer, candidate
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
                        this.onConnect(ws);
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

    private onConnect(ws: WebSocket){
        const connectionId: string = uuid();
        const connectionIds = getOrCreateConnectionIds(ws);
        connectionIds.add(connectionId);
        ws.send(JSON.stringify({type:"connect", connectionId:connectionId}));
    }

    private onOffer(ws: WebSocket, message: any){
        const connectionId = message.connectionId as string;
        const newOffer = new Offer(message.sdp, Date.now());
        offers.set(connectionId, newOffer);
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

        const pair = connectionPair.get(connectionId);
        const otherSessionWs = pair[0];
        connectionPair.set(connectionId, [otherSessionWs, ws]);

        const mapCandidates = candidates.get(otherSessionWs);
        if (mapCandidates) {
          const arrayCandidates = mapCandidates.get(connectionId);
          for (const candidate of arrayCandidates) {
            candidate.datetime = Date.now();
          }
        }
        clients.forEach((_v, k) => {
            if (k == ws) {
                return;
            }
            k.send(JSON.stringify({ from: connectionId, to: "", type: "answer", data: newAnswer }))
        });
    }

    private onCandidate(ws: WebSocket, message: any){
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

        clients.forEach((_v, k) => {
            if (k === ws) {
                return;
            }
            k.send(JSON.stringify({from:connectionId, to:"", type:"candidate", data:candidate}));
        });
    }
}