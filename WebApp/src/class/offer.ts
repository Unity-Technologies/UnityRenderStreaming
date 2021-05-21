export default class Offer {
  sdp: string;
  datetime: number;
  readyOtherPeer: boolean;
  polite: boolean;
  constructor(sdp: string, datetime: number, readyOtherPeer: boolean, polite: boolean) {
    this.sdp = sdp;
    this.datetime = datetime;
    this.readyOtherPeer = readyOtherPeer;
    this.polite = polite;
  }
}
