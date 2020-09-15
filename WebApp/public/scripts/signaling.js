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
    return location.protocol + '//' + location.host + location.pathname + 'signaling/' + method;
  };

  async start() {
    const createResponse = await fetch(this.url(''), { method: 'PUT', headers: this.headers() });
    const session = await createResponse.json();
    this.sessionId = session.sessionId;

    const res = await this.createConnection();
    const connection = await res.json();
    this.connectionId = connection.connectionId;

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

  async createConnection() {
    return await fetch(this.url('connection'), { method: 'PUT', headers: this.headers() });
  };
  async deleteConnection() {
    const data = { 'connectionId': this.connectionId };
    return await fetch(this.url('connection'), { method: 'DELETE', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendOffer(sdp) {
    const data = { 'sdp': sdp, 'connectionId': this.connectionId };
    await fetch(this.url('offer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendAnswer(sdp) {
    const data = { 'sdp': sdp, 'connectionId': this.connectionId };
    await fetch(this.url('answer'), { method: 'POST', headers: this.headers(), body: JSON.stringify(data) });
  };

  async sendCandidate(candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': this.connectionId
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
          this.connectionId = msg.connectionId;
          break;
        case "disconnect":
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
  }

  stop() {
    this.websocket.send(JSON.stringify({ type: "disconnect", from: this.connectionId }));
  }

  sendOffer(sdp) {
    const data = { 'sdp': sdp, 'connectionId': this.connectionId };
    const sendJson = JSON.stringify({ type: "offer", from: this.connectionId, data: data });
    console.log(sendJson);
    this.websocket.send(sendJson);
  }

  sendAnswer(sdp) {
    const data = { 'sdp': sdp, 'connectionId': this.connectionId };
    const sendJson = JSON.stringify({ type: "answer", from: this.connectionId, data: data });
    console.log(sendJson);
    this.websocket.send(sendJson);
  }

  sendCandidate(candidate, sdpMLineIndex, sdpMid) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': this.connectionId
    };
    const sendJson = JSON.stringify({ type: "candidate", from: this.connectionId, data: data });
    console.log(sendJson);
    this.websocket.send(sendJson);
  }
}