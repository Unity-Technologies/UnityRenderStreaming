import * as Logger from "./logger.js";

export default class Signaling extends EventTarget {

  constructor() {
    super();
    this.interval = 3000;
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));
  }

  headers() {
    if (this.sessionId !== undefined) {
      return { 'Content-Type': 'application/json', 'Session-Id': this.sessionId };
    }
    else {
      return { 'Content-Type': 'application/json' };
    }
  };

  url(method) {
    return location.origin + '/signaling/' + method;
  };

  async start() {
    const createResponse = await fetch(this.url(''), { method: 'PUT', headers: this.headers() });
    const session = await createResponse.json();
    this.sessionId = session.sessionId;

    this.loopGetOffer();
    this.loopGetAnswer();
    this.loopGetCandidate();
  }

  async loopGetOffer() {
    let lastTimeRequest = Date.now() - 30000;

    while (true) {
      const res = await this.getOffer(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const offers = data.offers;
      logger.log('get offers:', offers);

      offers.forEach(offer => {
        this.dispatchEvent(new CustomEvent('offer', { detail: offer }));
      });

      await this.sleep(this.interval);
    }
  }

  async loopGetAnswer() {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 30000;

    while (true) {
      const res = await this.getAnswer(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const answers = data.answers;
      Logger.log('get answers:', answers);

      answers.forEach(answer => {
        this.dispatchEvent(new CustomEvent('answer', { detail: answer }));
      });

      await this.sleep(this.interval);
    }
  }

  async loopGetCandidate() {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 30000;

    while (true) {
      const res = await this.getCandidate(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const candidates = data.candidates;
      Logger.log('get candidates:', candidates);

      if (candidates.length > 0) {
        for (let candidate of candidates[0].candidates) {
          this.dispatchEvent(new CustomEvent('candidate', { detail: candidate }));
        }
      }

      await this.sleep(this.interval);
    }
  }

  async stop() {
    await fetch(this.url(''), { method: 'DELETE', headers: this.headers() });
    this.sessionId = null;
  }

  async createConnection(connectionId) {
    const data = { 'connectionId': connectionId };
    const res = await fetch(this.url('connection'), { method: 'PUT', headers: this.headers(), body: JSON.stringify(data) });
    const json = await res.json();
    this.dispatchEvent(new CustomEvent('connect', { detail: json }));
    return json;
  };
  async deleteConnection(connectionId) {
    const data = { 'connectionId': connectionId };
    const res = await fetch(this.url('connection'), { method: 'DELETE', headers: this.headers(), body: JSON.stringify(data) });
    const json = await res.json();
    this.dispatchEvent(new CustomEvent('disconnect', { detail: json }));
    return json;
  };

  async sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    Logger.log('sendOffer:', data);
    await fetch(this.url('offer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    Logger.log('sendAnswer:', data);
    await fetch(this.url('answer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendCandidate(connectionId, candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': connectionId
    };
    Logger.log('sendCandidate:', data);
    await fetch(this.url('candidate'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async getOffer(fromTime = 0) {
    return await fetch(this.url(`offer?fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  };
  async getAnswer(fromTime = 0) {
    return await fetch(this.url(`answer?fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  };
  async getCandidate(fromTime = 0) {
    return await fetch(this.url(`candidate?fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  };
}

export class WebSocketSignaling extends EventTarget {

  constructor() {
    super();
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));

    if (location.protocol === "https:") {
      var websocketUrl = "wss://" + location.host;
    } else {
      var websocketUrl = "ws://" + location.host;
    }

    this.websocket = new WebSocket(websocketUrl);
    this.connectionId = null;

    this.websocket.onopen = () => {
      this.isWsOpen = true;
    }

    this.websocket.onclose = () => {
      this.isWsOpen = false;
    }

    this.websocket.onmessage = (event) => {
      const msg = JSON.parse(event.data);
      if (!msg || !this) {
        return;
      }

      Logger.log(msg);

      switch (msg.type) {
        case "connect":
          this.dispatchEvent(new CustomEvent('connect', { detail: msg }));
          break;
        case "disconnect":
          this.dispatchEvent(new CustomEvent('disconnect', { detail: msg }));
          break;
        case "offer":
          this.dispatchEvent(new CustomEvent('offer', { detail: { connectionId: msg.from, sdp: msg.data.sdp } }));
          break;
        case "answer":
          this.dispatchEvent(new CustomEvent('answer', { detail: { connectionId: msg.from, sdp: msg.data.sdp } }));
          break;
        case "candidate":
          this.dispatchEvent(new CustomEvent('candidate', { detail: { connectionId: msg.from, candidate: msg.data.candidate, sdpMLineIndex: msg.data.sdpMLineIndex, sdpMid: msg.data.sdpMid } }));
          break;
        default:
          break;
      }
    }
  }

  async start() {
    while (!this.isWsOpen) {
      await this.sleep(100);
    }
  }

  async stop() {
    this.websocket.close();
    while (this.isWsOpen) {
      await this.sleep(100);
    }
  }

  createConnection(connectionId) {
    const sendJson = JSON.stringify({ type: "connect", connectionId: connectionId });
    Logger.log(sendJson);
    this.websocket.send(sendJson);
  };

  deleteConnection(connectionId) {
    const sendJson = JSON.stringify({ type: "disconnect", connectionId: connectionId });
    Logger.log(sendJson);
    this.websocket.send(sendJson);
  };

  sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    const sendJson = JSON.stringify({ type: "offer", from: connectionId, data: data });
    Logger.log(sendJson);
    this.websocket.send(sendJson);
  }

  sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    const sendJson = JSON.stringify({ type: "answer", from: connectionId, data: data });
    Logger.log(sendJson);
    this.websocket.send(sendJson);
  }

  sendCandidate(connectionId, candidate, sdpMLineIndex, sdpMid) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': connectionId
    };
    const sendJson = JSON.stringify({ type: "candidate", from: connectionId, data: data });
    Logger.log(sendJson);
    this.websocket.send(sendJson);
  }
}