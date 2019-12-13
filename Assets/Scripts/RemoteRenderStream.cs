using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class RemoteRenderStream : MonoBehaviour
    {
        [SerializeField, Tooltip("Address for signaling server")]
        private string _urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers")]
        private RTCIceServer[] _iceServers = new RTCIceServer[]
        {
            new RTCIceServer()
            {
                urls = new string[] {"stun:stun.l.google.com:19302"}
            }
        };

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float _interval = 5.0f;

        private Signaling _signaling;
        private RTCPeerConnection _pc;
        private RTCConfiguration _conf;
        private RTCOfferOptions _offer;
        private RTCDataChannel _dataChannel;
        private string _sessionId;
        private string _connectionId;
        private List<RTCRtpSender> _pcSenders;

        public IEnumerator Start()
        {
            _conf = default;
            _conf.iceServers = _iceServers;

            // Create Local Peer Connection
            _pc = new RTCPeerConnection(ref _conf);
            _pc.OnIceCandidate = new DelegateOnIceCandidate(candidate => { StartCoroutine(OnIceCandidate(candidate)); });
            _pc.OnIceConnectionChange = new DelegateOnIceConnectionChange(state =>
            {
                if (state == RTCIceConnectionState.Disconnected)
                {
                    _pc.Close();
                }
            });
            _pc.OnTrack = new DelegateOnTrack(track => { OnTrack(track); });

            // Create Data Channel
            var conf = new RTCDataChannelInit(true);
            _dataChannel = _pc.CreateDataChannel("data", ref conf);
            _dataChannel.OnOpen = () => { Debug.Log("Remote DataChannel Connected"); };
            _dataChannel.OnClose = () => { Debug.Log("Remote DataChannel Closed"); };
            _dataChannel.OnMessage = bytes => { Debug.Log("Remote DataChannel Message"); };

            // Create Signaling
            _signaling = new Signaling(_urlSignaling);
            var opCreate = _signaling.Create();
            yield return opCreate;

            if (opCreate.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error - Signaling: {opCreate.webRequest.error}.  Check to make sure the signaling server is running at '{_urlSignaling}'.");
                yield break;
            }

            var newResData = opCreate.webRequest.DownloadHandlerJson<NewResData>().GetObject();
            _sessionId = newResData.sessionId;

            // Create Offer
            _offer = default;
            _offer.iceRestart = false;
            _offer.offerToReceiveVideo = true;
            _offer.offerToReceiveAudio = true;

            var opOffer = _pc.CreateOffer(ref _offer);
            yield return opOffer;

            if (opOffer.isError)
            {
                Debug.LogError($"Network Error - Create Offer: {opOffer.error}");
                yield break;
            }

            // Create Connection
            var opConnection = _signaling.CreateConnection(_sessionId);
            yield return opConnection;

            if (opConnection.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error - Create Connection: {opConnection.webRequest.error}");
                yield break;
            }

            var newConnectionData = opConnection.webRequest.DownloadHandlerJson<CreateConnectionResData>().GetObject();
            _connectionId = newConnectionData.connectionId;
            Debug.Log($"Remote Connection ID: {_connectionId}");

            Debug.Log($"Remote Offer: {opOffer.desc.sdp}");
            var opLocal = _pc.SetLocalDescription(ref opOffer.desc);
            yield return opLocal;

            if (opLocal.isError)
            {
                Debug.LogError($"Network Error - SetLocalDescription: {opLocal.error}");
                yield break;
            }

            var opPostOffer = _signaling.PostOffer(_sessionId, _connectionId, opOffer.desc.sdp);
            yield return opPostOffer;

            StartCoroutine(LoopPolling());
        }

        long lastTimeGetCandidateRequest = 0;
        long lastTimeGetAnswerRequest = 0;

        IEnumerator LoopPolling()
        {
            // ignore messages arrived before 30 secs ago
            lastTimeGetCandidateRequest = DateTime.UtcNow.ToJsMilliseconds() - 30000;
            lastTimeGetAnswerRequest = DateTime.UtcNow.ToJsMilliseconds() - 30000;

            while (true)
            {
                yield return StartCoroutine(GetCandidate());
                yield return StartCoroutine(GetAnswer());
                yield return new WaitForSeconds(_interval);
            }
        }

        IEnumerator GetAnswer()
        {
            var op = _signaling.GetAnswer(_sessionId, lastTimeGetAnswerRequest);
            yield return op;

            if (op.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error - Get Answer: {op.webRequest.error}");
                yield break;
            }
            var date = DateTimeExtension.ParseHttpDate(op.webRequest.GetResponseHeader("Date"));
            lastTimeGetAnswerRequest = date.ToJsMilliseconds();

            var obj = op.webRequest.DownloadHandlerJson<AnswerContainerResDataList>().GetObject();
            if (obj == null)
            {
                yield break;
            }

            foreach (var answer in obj.answers)
            {
                RTCSessionDescription desc = default;
                desc.type = RTCSdpType.Answer;
                desc.sdp = answer.sdp;

                Debug.Log($"Remote answer: {answer.sdp}");

                _pc.SetRemoteDescription(ref desc);
            }
        }

        IEnumerator GetCandidate()
        {
            var op = _signaling.GetCandidate(_sessionId, lastTimeGetCandidateRequest);
            yield return op;

            if (op.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error - Get Candidate: {op.webRequest.error}");
                yield break;
            }

            var date = DateTimeExtension.ParseHttpDate(op.webRequest.GetResponseHeader("Date"));
            lastTimeGetCandidateRequest = date.ToJsMilliseconds();

            var obj = op.webRequest.DownloadHandlerJson<CandidateContainerResDataList>().GetObject();
            if (obj == null)
            {
                yield break;
            }

            var candidates = obj.candidates.Where(c => c.connectionId == _connectionId);
            foreach (var candidateContainer in candidates)
            {
                foreach (var candidate in candidateContainer.candidates)
                {
                    RTCIceCandidate _candidate = default;
                    _candidate.candidate = candidate.candidate;
                    _candidate.sdpMLineIndex = candidate.sdpMLineIndex;
                    _candidate.sdpMid = candidate.sdpMid;

                    _pc.AddIceCandidate(ref _candidate);
                }
            }
        }

        IEnumerator OnIceCandidate(RTCIceCandidate candidate)
        {
            var opCandidate = _signaling.PostCandidate(_sessionId, _connectionId, candidate.candidate, candidate.sdpMid, candidate.sdpMLineIndex);
            yield return opCandidate;

            if (opCandidate.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error - Post Candidate: {opCandidate.webRequest.error}");
                yield break;
            }
        }

        void OnTrack(RTCTrackEvent e)
        {
            Debug.Log($"!!!!  Received remote track:\r\nTrack kind: {e.Track.Kind}\r\nTrack id: {e.Track.Id}\r\n");
        }
    }
}
