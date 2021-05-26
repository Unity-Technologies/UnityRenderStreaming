export default class Offer {
  sdp: string;
  datetime: number;
  polite: boolean;
  constructor(sdp: string, datetime: number, polite: boolean) {
    this.sdp = sdp;
    this.datetime = datetime;
    this.polite = polite;
  }
}
