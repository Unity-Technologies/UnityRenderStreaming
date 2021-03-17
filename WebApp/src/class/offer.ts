export default class Offer {
  sdp: string;
  datetime: number;
  constructor(sdp: string, datetime: number) {
    this.sdp = sdp;
    this.datetime = datetime;
  }
}
