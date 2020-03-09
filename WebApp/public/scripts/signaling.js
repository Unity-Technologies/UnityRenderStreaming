export default class Signaling {
  headers(sessionId) {
    if (sessionId !== undefined) {
      return { 'Content-Type': 'application/json', 'Session-Id': sessionId };
    }
    else {
      return { 'Content-Type': 'application/json' };
    }
  };

  url(method) {
    return location.protocol + '//' + location.host + location.pathname + 'signaling/' + method;
  };
  async createConnection(sessionId) {
    return await fetch(this.url('connection'), { method: 'PUT', headers: this.headers(sessionId) });
  };
  async deleteConnection(sessionId, connectionId) {
    const data = { 'connectionId': connectionId };
    return await fetch(this.url('connection'), { method: 'DELETE', headers: this.headers(sessionId), body: JSON.stringify(data) });
  };
  async sendOffer(sessionId, connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    return await fetch(this.url('offer'), { method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data) });
  };
  async sendAnswer(sessionId, connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    return await fetch(this.url('answer'), { method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data) });
  };
  async sendCandidate(sessionId, connectionId, candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': connectionId
    };
    return await fetch(this.url('candidate'), { method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data) });
  };
  async create() {
    return await fetch(this.url(''), { method: 'PUT', headers: this.headers() });
  };
  async delete(sessionId) {
    return await fetch(this.url(''), { method: 'DELETE', headers: this.headers(sessionId) });
  };
  async getOffer(sessionId, fromTime = 0) {
    return await fetch(this.url(`offer?fromtime=${fromTime}`), { method: 'GET', headers: this.headers(sessionId) });
  };
  async getAnswer(sessionId, fromTime = 0) {
    return await fetch(this.url(`answer?fromtime=${fromTime}`), { method: 'GET', headers: this.headers(sessionId) });
  };
  async getCandidate(sessionId, fromTime = 0) {
    return await fetch(this.url(`candidate?fromtime=${fromTime}`), { method: 'GET', headers: this.headers(sessionId) });
  };
}

export class WebSocketSignaling {

  constructor(element) {
    this.element = element;
    this.websocket = new WebSocket("ws://localhost:80");

    this.websocket.onopen = () => {
      this.websocket.send(JSON.stringify({ type: "connect" }));
    }

    this.websocket.onmessage = (event) => {
      const msg = JSON.parse(event.data);
      if (!msg || !this) {
        return;
      }

      console.log(msg);

      switch (msg.type) {
        case "connect":
          this.connectionId = msg.data.connectionId;
          break;
        case "disconnect":
          break;
        case "offer":
          element.dispatchEvent(new CustomEvent('offer', { detail: msg.data }));
          break;
        case "answer":
          element.dispatchEvent(new CustomEvent('answer', { detail: msg.data }));
          break;
        case "cadicate":
          element.dispatchEvent(new CustomEvent('cadicate', { detail: msg.data }));
          break;
        default:
          break;
      }
    }
  }

  start() {
    this.websocket.send(JSON.stringify({ type: "connect" }));
  }

  stop() {
    this.websocket.send(JSON.stringify({ type: "disconnect", from: this.connectionId }));
  }

  sendOffer(sdp) {
    const data = { 'sdp': sdp, 'connectionId': this.connectionId };
    this.websocket.send(JSON.stringify({ type: "offer", from: this.connectionId, data: data }));
  }

  sendAnswer(sdp) {
    const data = { 'sdp': sdp, 'connectionId': this.connectionId };
    this.websocket.send(JSON.stringify({ type: "answer", from: this.connectionId, data: data }));
  }

  sendCandidate(candidate, sdpMLineIndex, sdpMid) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': this.connectionId
    };
    this.websocket.send(JSON.stringify({ type: "candidate", from: this.connectionId, data: data }));
  }
}