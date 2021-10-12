import { sleep } from "./testutils";

let manager;

export function reset(isPrivate) {
  manager = isPrivate ? new MockPrivateSignalingManager() : new MockPublicSignalingManager();
}

export class MockSignaling extends EventTarget {

  get interval() {
    return 100;
  }

  async start() {
    await manager.add(this);
  }

  async stop() {
    await manager.remove(this);
  }

  async createConnection(connectionId) {
    await manager.openConnection(this, connectionId);
  }

  async deleteConnection(connectionId) {
    await manager.closeConnection(this, connectionId);
  }

  async sendOffer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    await manager.offer(this, data);
  }

  async sendAnswer(connectionId, sdp) {
    const data = { 'sdp': sdp, 'connectionId': connectionId };
    await manager.answer(this, data);
  }

  async sendCandidate(connectionId, candidate, sdpMLineIndex, sdpMid) {
    const data = {
      'candidate': candidate,
      'sdpMLineIndex': sdpMLineIndex,
      'sdpMid': sdpMid,
      'connectionId': connectionId
    };
    await manager.candidate(this, data);
  }
}

class MockPublicSignalingManager {
  constructor() {
    this.list = new Set();
    this.delay = async () => await sleep(10);
  }

  async add(signaling) {
    await this.delay();
    this.list.add(signaling);
    signaling.dispatchEvent(new Event("start"));
  }

  async remove(signaling) {
    await this.delay();
    this.list.delete(signaling);
    signaling.dispatchEvent(new Event("end"));
  }

  async openConnection(signaling, connectionId) {
    await this.delay();
    const data = { connectionId: connectionId, polite: true };
    signaling.dispatchEvent(new CustomEvent("connect", { detail: data }));
  }

  async closeConnection(signaling, connectionId) {
    await this.delay();
    const data = { connectionId: connectionId };
    for (const element of this.list) {
      element.dispatchEvent(new CustomEvent("disconnect", { detail: data }));
    }
  }

  async offer(owner, data) {
    await this.delay();
    data.polite = false;
    for (const signaling of this.list) {
      if (signaling != owner) {
        signaling.dispatchEvent(new CustomEvent("offer", { detail: data }));
      }
    }
  }

  async answer(owner, data) {
    await this.delay();
    for (const signaling of this.list) {
      if (signaling != owner) {
        signaling.dispatchEvent(new CustomEvent("answer", { detail: data }));
      }
    }
  }

  async candidate(owner, data) {
    await this.delay();
    for (const signaling of this.list) {
      if (signaling != owner) {
        signaling.dispatchEvent(new CustomEvent("candidate", { detail: data }));
      }
    }
  }
}

class MockPrivateSignalingManager {
  constructor() {
    // structure Map<string:connectionId, Set<MockSignaling>> connectionIds
    this.connectionIds = new Map();
    this.delay = () => new Promise(resolve => setTimeout(resolve, 10));
  }

  async add(signaling) {
    await this.delay();
    signaling.dispatchEvent(new Event("start"));
  }

  async remove(signaling) {
    await this.delay();
    signaling.dispatchEvent(new Event("end"));
  }

  async openConnection(signaling, connectionId) {
    await this.delay();
    const peerExists = this.connectionIds.has(connectionId);
    if (!peerExists) {
      this.connectionIds.set(connectionId, new Set());
    }

    const list = this.connectionIds.get(connectionId);
    list.add(signaling);

    const data = { connectionId: connectionId, polite: peerExists };
    signaling.dispatchEvent(new CustomEvent("connect", { detail: data }));
  }

  async closeConnection(signaling, connectionId) {
    await this.delay();
    const peerExists = this.connectionIds.has(connectionId);
    const list = this.connectionIds.get(connectionId);
    if (!peerExists || !list.has(signaling)) {
      console.error(`${connectionId} This connection id is not used.`);
    }

    const data = { connectionId: connectionId };
    for (const element of list) {
      element.dispatchEvent(new CustomEvent("disconnect", { detail: data }));
    }

    list.delete(signaling);
    if (list.size == 0) {
      this.connectionIds.delete(connectionId);
    }
  }

  findList(owner, connectionId) {
    if (!this.connectionIds.has(connectionId)) {
      return null;
    }

    const list = new Set(this.connectionIds.get(connectionId));
    list.delete(owner);
    if (list.Count == 0) {
      return null;
    }

    return list;
  }

  async offer(owner, data) {
    await this.delay();
    const list = this.findList(owner, data.connectionId);
    if (list == null) {
      console.warn(`${data.connectionId} This connection id is not ready other session.`);
      return;
    }

    data.polite = true;
    for (const signaling of list) {
      if (signaling != owner) {
        signaling.dispatchEvent(new CustomEvent("offer", { detail: data }));
      }
    }
  }

  async answer(owner, data) {
    await this.delay();
    const list = this.findList(owner, data.connectionId);
    if (list == null) {
      console.warn(`${data.connectionId} This connection id is not ready other session.`);
      return;
    }

    for (const signaling of list) {
      if (signaling != owner) {
        signaling.dispatchEvent(new CustomEvent("answer", { detail: data }));
      }
    }
  }

  async candidate(owner, data) {
    await this.delay();
    const list = this.findList(owner, data.connectionId);
    if (list == null) {
      console.warn(`${data.connectionId} This connection id is not ready other session.`);
      return;
    }

    for (const signaling of list) {
      if (signaling != owner) {
        signaling.dispatchEvent(new CustomEvent("candidate", { detail: data }));
      }
    }
  }
}
