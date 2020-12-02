export default class Answer {
  sdp: string;
  datetime: number;
  constructor(sdp: string, datetime: number) {
    this.sdp = sdp;
    this.datetime = datetime;
  }
}