import * as Logger from "./logger.js";

export class Signaling extends EventTarget {

  constructor() {
    super();
    this.running = false;
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));
  }

  headers() {
    if (this.sessionId !== undefined) {
      return { 'Content-Type': 'application/json', 'Session-Id': this.sessionId };
    }
    else {
      return { 'Content-Type': 'application/json' };
    }
  }

  get interval() {
    return 1000;
  }

  url(method, parameter='') {
    let ret = location.origin + '/signaling';
    if(method)
      ret += '/' + method;
    if(parameter)
      ret += '?' + parameter;
    return ret;
  }

  async start() {
    if(this.running) {
      return;
    }

    this.running = true;
    while (!this.sessionId) {
      const createResponse = await fetch(this.url(''), { method: 'PUT', headers: this.headers() });
      const session = await createResponse.json();
      this.sessionId = session.sessionId;

      if (!this.sessionId) {
        await this.sleep(this.interval);
      }
    }

    this.loopGetAll();
  }

  async loopGetAll() {
    let lastTimeRequest = Date.now() - 30000;
    while (this.running) {
      const res = await this.getAll(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const messages = data.messages;

      for(const msg of messages) {
        switch (msg.type) {
          case "connect":
            break;
          case "disconnect":
            this.dispatchEvent(new CustomEvent('disconnect', { detail: msg }));
            break;
          case "offer":
            this.dispatchEvent(new CustomEvent('offer', { detail: msg } ));
            break;
          case "answer":
            this.dispatchEvent(new CustomEvent('answer', { detail: msg } ));
            break;
          case "candidate":
            this.dispatchEvent(new CustomEvent('candidate', { detail: msg }));
            break;
          default:
            break;
        }
      }
      await this.sleep(this.interval);
    }
  }

  async loopGetConnection() {
    let currentConnections = new Set();
    while (this.running) {
      const res = await this.getConnection();
      const data = await res.json();
      const connections = data.connections;
      Logger.log('get connections:', connections);

      const newSet = new Set();
      connections.forEach(e => newSet.add(e.connectionId));
      const deleteConnection = new Set([...currentConnections].filter(e => (!newSet.has(e))));

      deleteConnection.forEach(connection => {
        this.dispatchEvent(new CustomEvent('disconnect', { detail: { connectionId: connection } }));
        currentConnections.delete(connection);
      });

      newSet.forEach(e => currentConnections.add(e));

      await this.sleep(this.interval);
    }
  }

  async loopGetOffer() {
    let lastTimeRequest = Date.now() - 30000;

    while (this.running) {
      const res = await this.getOffer(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const offers = data.offers;
      Logger.log('get offers:', offers);

      offers.forEach(offer => {
        this.dispatchEvent(new CustomEvent('offer', { detail: offer }));
      });

      await this.sleep(this.interval);
    }
  }

  async loopGetAnswer() {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 30000;

    while (this.running) {
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

    while (this.running) {
      const res = await this.getCandidate(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const candidates = data.candidates;
      Logger.log('get candidates:', candidates);

      if (candidates.length > 0) {
        const connectionId = candidates[0].connectionId;
        for (let candidate of candidates[0].candidates) {
          const dispatch = { connectionId: connectionId, candidate: candidate.candidate, sdpMLineIndex: candidate.sdpMLineIndex, sdpMid: candidate.sdpMid };
          this.dispatchEvent(new CustomEvent('candidate', { detail: dispatch }));
        }
      }

      await this.sleep(this.interval);
    }
  }

  async stop() {
    this.running = false;
    await fetch(this.url(''), { method: 'DELETE', headers: this.headers() });
    this.sessionId = null;
  }

  async createConnection(connectionId) {
    const data = { 'connectionId': connectionId };
    const res = await fetch(this.url('connection'), { method: 'PUT', headers: this.headers(), body: JSON.stringify(data) });
    const json = await res.json();
    Logger.log(`Signaling: HTTP create connection, connectionId: ${json.connectionId}, polite:${json.polite}`);

    this.dispatchEvent(new CustomEvent('connect', { detail: json }));
    return json;
  }

  async deleteConnection(connectionId) {
    const data = { 'connectionId': connectionId };
    const res = await fetch(this.url('connection'), { method: 'DELETE', headers: this.headers(), body: JSON.stringify(data) });
    const json = await res.json();
    this.dispatchEvent(new CustomEvent('disconnect', { detail: json }));
    return json;
  }

  async sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    Logger.log('sendOffer:' + data);
    await fetch(this.url('offer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  }

  async sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    Logger.log('sendAnswer:' + data);
    await fetch(this.url('answer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  }

  async sendCandidate(connectionId, candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': connectionId
    };
    Logger.log('sendCandidate:' + data);
    await fetch(this.url('candidate'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  }

  async getConnection() {
    return await fetch(this.url(`connection`), { method: 'GET', headers: this.headers() });
  }

  async getOffer(fromTime = 0) {
    return await fetch(this.url(`offer`, `fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  }

  async getAnswer(fromTime = 0) {
    return await fetch(this.url(`answer`, `fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  }

  async getCandidate(fromTime = 0) {
    return await fetch(this.url(`candidate`, `fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  }

  async getAll(fromTime = 0) {
    return await fetch(this.url(``, `fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  }
}

export class WebSocketSignaling extends EventTarget {

  constructor() {
    super();
    this.sleep = msec => new Promise(resolve => setTimeout(resolve, msec));

    let websocketUrl;
    if (location.protocol === "https:") {
      websocketUrl = "wss://" + location.host;
    } else {
      websocketUrl = "ws://" + location.host;
    }

    this.websocket = new WebSocket(websocketUrl);
    this.connectionId = null;

    this.websocket.onopen = () => {
      this.isWsOpen = true;
    };

    this.websocket.onclose = () => {
      this.isWsOpen = false;
    };

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
          this.dispatchEvent(new CustomEvent('offer', { detail: { connectionId: msg.from, sdp: msg.data.sdp, polite: msg.data.polite } }));
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
    };
  }

  get interval() {
    return 100;
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
  }

  deleteConnection(connectionId) {
    const sendJson = JSON.stringify({ type: "disconnect", connectionId: connectionId });
    Logger.log(sendJson);
    this.websocket.send(sendJson);
  }

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
