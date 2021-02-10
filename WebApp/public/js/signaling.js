import uuid4 from 'https://cdn.jsdelivr.net/gh/tracker1/node-uuid4/browser.mjs';

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

    const id = uuid4();
    const connection = await this.createConnection(id);

    this.loopGetOffer();
    this.loopGetAnswer();
    this.loopGetCandidate();

    this.connectionId = connection.connectionId;
    return this.connectionId;
  }

  async loopGetOffer() {
    let lastTimeRequest = Date.now() - 30000;

    while (true) {
      const res = await this.getOffer(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const offers = data.offers;

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
      const candidates = data.candidates.filter(v => v.connectionId = this.connectionId);

      if (candidates.length > 0) {
        for (let candidate of candidates[0].candidates) {
          this.dispatchEvent(new CustomEvent('candidate', { detail: candidate }));
        }
      }

      await this.sleep(this.interval);
    }
  }

  async stop() {
    await this.deleteConnection();
    this.connectionId = null;
    await fetch(this.url(''), { method: 'DELETE', headers: this.headers() });
    this.sessionId = null;
  }

  async createConnection(connectionId) {
    const data = { 'connectionId': connectionId };
    const res = await fetch(this.url('connection'), { method: 'PUT', headers: this.headers(), body: JSON.stringify(data) });
    const json = await res.json();
    this.dispatchEvent(new CustomEvent('connect', {detail: json}));
    return json;
  };
  async deleteConnection(connectionId) {
    const data = { 'connectionId': connectionId };
    const res = await fetch(this.url('connection'), { method: 'DELETE', headers: this.headers(), body: JSON.stringify(data) });
    const json = await res.json();
    this.dispatchEvent(new CustomEvent('disconnect', {detail: json}));
    return json;
  };

  async sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    await fetch(this.url('offer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    await fetch(this.url('answer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendCandidate(connectionId, candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': connectionId
    };
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

    if (location.protocol === "https:") {
      var websocketUrl = "wss://" + location.host;
    } else {
      var websocketUrl = "ws://" + location.host;
    }

    this.websocket = new WebSocket(websocketUrl);
    this.connectionId = null;

    this.websocket.onopen = () => {
      const id = uuid4();
      this.websocket.send(JSON.stringify({ type: "connect", connectionId: id }));
    }

    this.websocket.onmessage = (event) => {
      const msg = JSON.parse(event.data);
      if (!msg || !this) {
        return;
      }

      console.log(msg);

      switch (msg.type) {
        case "connect":
          this.connectionId = msg.connectionId;
          this.dispatchEvent(new CustomEvent('connect', { detail: msg }));
          break;
        case "disconnect":
          this.dispatchEvent(new CustomEvent('disconnect', { detail: msg }));
          break;
        case "offer":
          this.dispatchEvent(new CustomEvent('offer', { detail: msg.data }));
          break;
        case "answer":
          this.dispatchEvent(new CustomEvent('answer', { detail: msg.data }));
          break;
        case "candidate":
          this.dispatchEvent(new CustomEvent('candidate', { detail: msg.data }));
          break;
        default:
          break;
      }
    }
  }

  async start() {
    const sleep = msec => new Promise(resolve => setTimeout(resolve, msec));
    while(this.connectionId == null){
      await sleep(100);
    }
    return this.connectionId;
  }

  stop() {
    this.websocket.send(JSON.stringify({ type: "disconnect", from: this.connectionId }));
  }

  createConnection(connectionId) {
    const sendJson = JSON.stringify({ type: "connect", connectionId: connectionId });
    console.log(sendJson);
    this.websocket.send(sendJson);
  };

  deleteConnection(connectionId) {
    const sendJson = JSON.stringify({ type: "disconnect", connectionId: connectionId });
    console.log(sendJson);
    this.websocket.send(sendJson);
  };

  sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    const sendJson = JSON.stringify({ type: "offer", from: connectionId, data: data });
    console.log(sendJson);
    this.websocket.send(sendJson);
  }

  sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    const sendJson = JSON.stringify({ type: "answer", from: connectionId, data: data });
    console.log(sendJson);
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
    console.log(sendJson);
    this.websocket.send(sendJson);
  }
}