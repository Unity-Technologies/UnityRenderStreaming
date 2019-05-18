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

  async send(sessionId, data) {
    let method = undefined;
    if ('type' in data) {
      switch (data.type) {
        case 'offer':
          method = 'offer';
          break;
        case 'answer':
          method = 'answer';
          break;
        default:
          return;
      }
    } else {
      method = 'candidate';
    }
    const response = await fetch(this.url(method), {method: 'POST', headers: this.headers(sessionId), body: JSON.stringify(data)});
    return await response.json();
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
