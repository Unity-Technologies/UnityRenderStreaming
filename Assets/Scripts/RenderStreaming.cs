using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    public class RenderStreaming : MonoBehaviour
    {
        [SerializeField]
        private string urlSignaling = "http://localhost";

        [SerializeField]
        private string urlSTUN = "stun:stun.l.google.com:19302";

        [SerializeField]
        private float interval = 5.0f;

        private Signaling signaling;
        private Dictionary<string, RTCPeerConnection> pcs = new Dictionary<string, RTCPeerConnection>();
        private Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>> mapChannels = new Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>>();
        private RTCConfiguration conf;
        private string sessionId;

        public void Awake()
        {
            WebRTC.WebRTC.Initialize();
        }

        public void OnDestroy()
        {
            WebRTC.WebRTC.Finalize();
        }
        public IEnumerator Start()
        {
            signaling = new Signaling(urlSignaling);
            var opCreate = signaling.Create();
            yield return opCreate;
            if (opCreate.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {opCreate.webRequest.error}");
                yield break;
            }
            var newResData = opCreate.webRequest.DownloadHandlerJson<NewResData>().GetObject();
            sessionId = newResData.sessionId;

            conf = default;
            conf.iceServers = new RTCIceServer[]
            {
            new RTCIceServer { urls = new string[] { urlSTUN } }
            };

            StartCoroutine(LoopPolling());
        }

        IEnumerator LoopPolling()
        {
            while (true)
            {
                yield return StartCoroutine(GetOffer());
                yield return StartCoroutine(GetCandidate());
                yield return new WaitForSeconds(interval);
            }
        }

        IEnumerator GetOffer()
        {
            var op = signaling.GetOffer(sessionId);
            yield return op;

            if (op.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {op.webRequest.error}");
                yield break;
            }
            var obj = op.webRequest.DownloadHandlerJson<OfferResDataList>().GetObject();
            foreach (var offer in obj.offers)
            {
                RTCSessionDescription _desc = default;
                _desc.type = RTCSdpType.Offer;
                _desc.sdp = offer.sdp;
                var connectionId = offer.connectionId;
                if (pcs.ContainsKey(connectionId))
                {
                    continue;
                }
                var pc = new RTCPeerConnection();
                pcs.Add(offer.connectionId, pc);

                pc.OnDataChannel = new DelegateOnDataChannel(channel => { OnDataChannel(pc, channel); });
                pc.SetConfiguration(ref conf);
                pc.OnIceCandidate = new DelegateOnIceCandidate(candidate => { StartCoroutine(OnIceCandidate(offer.connectionId, candidate)); });
                pc.SetRemoteDescription(ref _desc);

                StartCoroutine(Answer(connectionId));
            }
        }

        IEnumerator Answer(string connectionId)
        {
            RTCAnswerOptions options = default;
            var pc = pcs[connectionId];
            var op = pc.CreateAnswer(ref options);
            yield return op;
            if (op.isError)
            {
                Debug.LogError($"Network Error: {op.error}");
                yield break;
            }
            var opLocalDesc = pc.SetLocalDescription(ref op.desc);
            yield return opLocalDesc;
            if (opLocalDesc.isError)
            {
                Debug.LogError($"Network Error: {opLocalDesc.error}");
                yield break;
            }
            var op3 = signaling.PostAnswer(this.sessionId, connectionId, op.desc.sdp);
            yield return op3;
            if (op3.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {op3.webRequest.error}");
                yield break;
            }
        }

        IEnumerator GetCandidate()
        {
            var op = signaling.GetCandidate(sessionId);
            yield return op;

            if (op.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {op.webRequest.error}");
                yield break;
            }
            var obj = op.webRequest.DownloadHandlerJson<CandidateContainerResDataList>().GetObject();
            foreach (var candidateContainer in obj.candidates)
            {
                RTCPeerConnection pc;
                if (!pcs.TryGetValue(candidateContainer.connectionId, out pc))
                {
                    continue;
                }
                foreach (var candidate in candidateContainer.candidates)
                {
                    RTCIceCandidate _candidate = default;
                    _candidate.candidate = candidate.candidate;
                    _candidate.sdpMlineIndex = candidate.sdpMLineIndex;
                    _candidate.sdpMid = candidate.sdpMid;

                    pcs[candidateContainer.connectionId].AddIceCandidate(ref _candidate);
                }
            }
        }

        IEnumerator OnIceCandidate(string connectionId, RTCIceCandidate candidate)
        {
            var opCandidate = signaling.PostCandidate(sessionId, connectionId, candidate.candidate, candidate.sdpMid, candidate.sdpMlineIndex);
            yield return opCandidate;
            if (opCandidate.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {opCandidate.webRequest.error}");
                yield break;
            }
        }

        void OnDataChannel(RTCPeerConnection pc, RTCDataChannel channel)
        {
            Dictionary<int, RTCDataChannel> channels;
            if (!mapChannels.TryGetValue(pc, out channels))
            {
                channels = new Dictionary<int, RTCDataChannel>();
                mapChannels.Add(pc, channels);
            }
            channels.Add(channel.Id, channel);

            channel.OnMessage = new DelegateOnMessage(bytes => { RemoteInput.ProcessInput(bytes); });
        }
    }
}
