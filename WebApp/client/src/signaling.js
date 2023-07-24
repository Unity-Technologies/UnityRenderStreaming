import * as Logger from "./logger.js";

export class Signaling extends EventTarget {

  constructor(interval = 1000) {
    super();
    this.running = false;
    this.interval = interval;
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
      const data = await res.json();
      lastTimeRequest = data.datetime ? data.datetime : Date.now();

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

  async getAll(fromTime = 0) {
    return await fetch(this.url(``, `fromtime=${fromTime}`), { method: 'GET', headers: this.headers() });
  }
}

export class WebSocketSignaling extends EventTarget {

  constructor(interval = 1000) {
    super();
    this.interval = interval;
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
