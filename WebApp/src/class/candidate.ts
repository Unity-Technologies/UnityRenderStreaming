export default class Candidate {
  candidate: string;
  sdpMLineIndex: number;
  sdpMid: string;
  datetime: number;
  constructor(candidate: string, sdpMLineIndex: number, sdpMid: string, datetime: number) {
    this.candidate = candidate;
    this.sdpMLineIndex = sdpMLineIndex;
    this.sdpMid = sdpMid;
    this.datetime = datetime;
  }
}