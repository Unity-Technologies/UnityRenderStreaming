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
    this.dispatchEvent(new CustomEvent('connect', { detail: this.connectionId }));

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
        const offerData = {
          'connectionId': offer.connectionId,
          'sdp': offer.sdp
        };
        this.dispatchEvent(new CustomEvent('offer', { detail: offerData }));
      });

      await this.sleep(this.interval);
    }
  }

  async loopGetAnswer() {
    // receive answer message from 30secs ago
    let lastTimeRequest = Date.now() - 20000;

    while (true) {
      const res = await this.getAnswer(lastTimeRequest);
      lastTimeRequest = Date.parse(res.headers.get('Date'));

      const data = await res.json();
      const answers = data.answers;

      answers.forEach(answer => {
        const answerData = {
          'connectionId': answer.connectionId,
          'sdp': answer.sdp
        };
        this.dispatchEvent(new CustomEvent('answer', { detail: answerData }));
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
      const candidatesList = data.candidates.filter(v => v.connectionId = this.connectionId);

      for (let candidateContainer of candidatesList.candidates) {
        for (let candidate of candidateContainer.candidates) {
          const candidateData = {
            'connectionId': candidateContainer.connectionId,
            'candidate': candidate.candidate,
            'sdpMid': candidate.sdpMid,
            'sdpMLineIndex': candidate.sdpMLineIndex
          };
          this.dispatchEvent(new CustomEvent('candidate', { detail: candidateData }));
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
      this.websocketUrl = "wss://" + location.host;
    } else {
      this.websocketUrl = "ws://" + location.host;
    }
  }

  async start() {
    this.websocket = new WebSocket(this.websocketUrl);
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
          this.dispatchEvent(new CustomEvent('connect', { detail: msg.connectionId }));
          break;
        case "offer":
          const offerData = {
            'connectionId': msg.from,
            'sdp': msg.data.sdp,
            'type': msg.data.type
          };
          this.dispatchEvent(new CustomEvent('offer', { detail: offerData }));
          break;
        case "answer":
          const answerData = {
            'connectionId': msg.from,
            'sdp': msg.data.sdp,
            'type': msg.data.type
          };
          this.dispatchEvent(new CustomEvent('answer', { detail: answerData }));
          break;
        case "candidate":
          const candidateData = {
            'connectionId': msg.from,
            'candidate': msg.data.candidate,
            'sdpMid': msg.data.sdpMid,
            'sdpMLineIndex': msg.data.sdpMLineIndex
          };
          this.dispatchEvent(new CustomEvent('candidate', { detail: candidateData }));
          break;
        default:
          break;
      }
    }

    const sleep = msec => new Promise(resolve => setTimeout(resolve, msec));
    while(this.connectionId == null){
      await sleep(100);
    }
  }

  stop() {
    this.websocket.close();
    this.websocket = null;
  }

  sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    const sendJson = JSON.stringify({ type: "offer", from: this.connectionId, data: data });
    console.log(sendJson);
    this.websocket.send(sendJson);
  }

  sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    const sendJson = JSON.stringify({ type: "answer", from: this.connectionId, data: data });
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
    const sendJson = JSON.stringify({ type: "candidate", from: this.connectionId, data: data });
    console.log(sendJson);
    this.websocket.send(sendJson);
  }
}