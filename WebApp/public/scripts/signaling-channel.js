export default class SignalingChannel {
  headers(sessionId) {
    if(sessionId != undefined)
    {
      return {'Content-Type': 'application/json', 'Session-Id': sessionId};
    }
    else {
      return {'Content-Type': 'application/json'};
    }
  };

  url(method) {
    return location.protocol + '//' + location.host + '/signaling/' + method;
  };
  async createConnection(sessionId) {
    const response = await fetch(this.url('connection'), {method: 'PUT', headers: this.headers(sessionId)});
    return await response.json();
  };
  async deleteConnection(sessionId, connectionId) {
    const data = {'connectionId' : connectionId };
    const response = await fetch(this.url('connection'), {method: 'DELETE', headers: this.headers(sessionId), body: JSON.stringify(data)});
    return await response.json();
  };
  async sendOffer(sessionId, connectionId, sdp) {
    const data = {'sdp' : sdp, 'connectionId' : connectionId };
    await fetch(this.url('offer'), {method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data)});
    return;
  };
  async sendAnswer(sessionId, connectionId, sdp) {
    const data = {'sdp' : sdp, 'connectionId' : connectionId };
    await fetch(this.url('answer'), {method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data)});
    return;
  };
  async sendCandidate(sessionId, connectionId, candidate, sdpMid, sdpMLineIndex) {
    const data = {
      'candidate' : candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId' : connectionId
    };
    await fetch(this.url('candidate'), {method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data)});
    return;
  };
  async create() {
    const response = await fetch(this.url(''), {method: 'PUT', headers: this.headers()});
    return await response.json();
  };
  async delete(sessionId) {
    await fetch(this.url(''), {method: 'DELETE', headers: this.headers(sessionId)});
    return;
  };
  async getOffer(sessionId) {
    const response = await fetch(this.url('offer'), {method: 'GET', headers: this.headers(sessionId)});
    return await response.json();
  };
  async getAnswer(sessionId) {
    const response = await fetch(this.url('answer'), {method: 'GET', headers: this.headers(sessionId)});
    return await response.json();
  };
  async getCandidate(sessionId) {
    const response = await fetch(this.url('candidate'), {method: 'GET', headers: this.headers(sessionId)});
    return await response.json();
  };
}
