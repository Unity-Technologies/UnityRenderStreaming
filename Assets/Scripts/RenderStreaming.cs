using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.WebRTC;
using System.Text.RegularExpressions;
using UnityEditor;

namespace Unity.RenderStreaming
{
    [Serializable]
    public class ButtonClickEvent : UnityEngine.Events.UnityEvent<int> { }

    [Serializable]
    public class ButtonClickElement
    {
        [Tooltip("Specifies the ID on the HTML")]
        public int elementId;
        public ButtonClickEvent click;
    }

    public class RenderStreaming : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField, Tooltip("Address for signaling server")]
        private string urlSignaling = "ws://localhost";

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers")]
        private RTCIceServer[] iceServers = new RTCIceServer[]
        {
            new RTCIceServer()
            {
                urls = new string[] { "stun:stun.l.google.com:19302" }
            }
        };

        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        private Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Camera to capture video stream")]
        private Camera captureCamera;

        [SerializeField, Tooltip("Array to set your own click event")]
        private ButtonClickElement[] arrayButtonClickEvent;
#pragma warning restore 0649

        private Signaling signaling;
        private Dictionary<string, RTCPeerConnection> pcs = new Dictionary<string, RTCPeerConnection>();
        private Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>> mapChannels = new Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>>();
        private RTCConfiguration conf;

        private string sessionId = Guid.NewGuid().ToString();

        private MediaStream videoStream;
        private MediaStream audioStream;

        public void Awake()
        {
            WebRTC.WebRTC.Initialize();
            RemoteInput.Initialize();
            RemoteInput.ActionButtonClick = OnButtonClick;
        }

        public void OnDestroy()
        {
            WebRTC.WebRTC.Finalize();
            RemoteInput.Destroy();
            Unity.WebRTC.Audio.Stop();
        }
        public IEnumerator Start()
        {
            if (!WebRTC.WebRTC.HWEncoderSupport)
            {
                yield break;
            }
            videoStream = captureCamera.CaptureStream(streamingSize.x, streamingSize.y);
            audioStream = Unity.WebRTC.Audio.CaptureStream();

            conf = default;
            conf.iceServers = iceServers;
            StartCoroutine(WebRTC.WebRTC.Update());
        }

        private Coroutine _loopPollingCoroutine;

        void OnEnable()
        {
            if (this.signaling == null)
            {
                this.signaling = new Signaling(urlSignaling, sessionId);
            }

            if (_loopPollingCoroutine == null)
            {
                _loopPollingCoroutine = StartCoroutine(LoopPolling());
            }
        }

        void OnDisable()
        {
            if (_loopPollingCoroutine != null)
            {
                StopCoroutine(_loopPollingCoroutine);
                _loopPollingCoroutine = null;
            }

            if (this.signaling != null)
            {
                this.signaling.Stop();
                this.signaling = null;
            }
        }

        long lastTimeGetOfferRequest = 0;
        long lastTimeGetCandidateRequest = 0;

        IEnumerator LoopPolling()
        {
            while (true)
            {
                yield return null;

                var messages = this.signaling.GetMessages();
                if (messages != null)
                {
                    foreach (var message in messages)
                    {
                        if (message.type == "offer")
                        {
                            HandleOfferMessage(message);
                        }

                        if (!string.IsNullOrEmpty(message.candidate))
                        {
                            HandleCandidateMessage(message);
                        }
                    }
                }
            }
        }

        void HandleOfferMessage(SignalingMessage offer)
        {
            RTCSessionDescription _desc = default;
            _desc.type = RTCSdpType.Offer;
            _desc.sdp = offer.sdp;
            var connectionId = offer.connectionId;

            if (!pcs.TryGetValue(connectionId, out var pc))
            {
                pc = new RTCPeerConnection();
                pcs.Add(offer.connectionId, pc);

                pc.OnDataChannel = new DelegateOnDataChannel(channel => { OnDataChannel(pc, channel); });
                pc.SetConfiguration(ref conf);
                pc.OnIceCandidate = new DelegateOnIceCandidate(candidate => { OnIceCandidate(offer.connectionId, candidate); });
                pc.OnIceConnectionChange = new DelegateOnIceConnectionChange(state =>
                {
                    if(state == RTCIceConnectionState.Disconnected)
                    {
                        pc.Close();
                        pcs.Remove(connectionId);
                    }
                });
            }

            //make video bit rate starts at 16000kbits, and 160000kbits at max.
            string pattern = @"(a=fmtp:\d+ .*level-asymmetry-allowed=.*)\r\n";
            _desc.sdp = Regex.Replace(_desc.sdp, pattern, "$1;x-google-start-bitrate=16000;x-google-max-bitrate=160000\r\n");
            pc.SetRemoteDescription(ref _desc);
            foreach (var track in videoStream.GetTracks())
            {
                pc.AddTrack(track);
            }
            /*foreach(var track in audioStream.GetTracks())
            {
                pc.AddTrack(track);
            }*/
            StartCoroutine(Answer(connectionId));
        }

        void HandleCandidateMessage(SignalingMessage candidate)
        {
            RTCPeerConnection pc;
            if (!pcs.TryGetValue(candidate.connectionId, out pc))
            {
                return;
            }

            RTCIceCandidate _candidate = default;
            _candidate.candidate = candidate.candidate;
            _candidate.sdpMLineIndex = candidate.sdpMLineIndex;
            _candidate.sdpMid = candidate.sdpMid;

            pc.AddIceCandidate(ref _candidate);
        }

        IEnumerator GetOffer()
        {
            var op = signaling.GetOffer(sessionId, lastTimeGetOfferRequest);
            yield return op;
            if (op.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {op.webRequest.error}");
                yield break;
            }
            var date = DateTimeExtension.ParseHttpDate(op.webRequest.GetResponseHeader("Date"));
            lastTimeGetOfferRequest = date.ToJsMilliseconds();

            var obj = op.webRequest.DownloadHandlerJson<OfferResDataList>().GetObject();
            if (obj == null)
            {
                yield break;
            }
            foreach (var offer in obj.offers)
            {

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

            signaling.SendMessage(new SignalingMessage
            {
                connectionId = connectionId,
                sdp = op.desc.sdp,
                type = "answer",
            });
        }

        IEnumerator GetCandidate()
        {
            var op = signaling.GetCandidate(sessionId, lastTimeGetCandidateRequest);
            yield return op;

            if (op.webRequest.isNetworkError)
            {
                Debug.LogError($"Network Error: {op.webRequest.error}");
                yield break;
            }
            var date = DateTimeExtension.ParseHttpDate(op.webRequest.GetResponseHeader("Date"));
            lastTimeGetCandidateRequest = date.ToJsMilliseconds();

            var obj = op.webRequest.DownloadHandlerJson<CandidateContainerResDataList>().GetObject();
            if (obj == null)
            {
                yield break;
            }
            foreach (var candidateContainer in obj.candidates)
            {

            }
        }

        void OnIceCandidate(string connectionId, RTCIceCandidate candidate)
        {
            signaling.SendMessage(
                new SignalingMessage
                {
                    connectionId = connectionId,
                    type = "candidate",
                    candidate = candidate.candidate,
                    sdpMid = candidate.sdpMid,
                    sdpMLineIndex = candidate.sdpMLineIndex,
                });
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

            if(channel.Label == "data")
            {
                channel.OnMessage = new DelegateOnMessage(bytes => { RemoteInput.ProcessInput(bytes); });
                channel.OnClose = new DelegateOnClose(() => { RemoteInput.Reset(); });
            }
        }

        void OnButtonClick(int elementId)
        {
            foreach (var element in arrayButtonClickEvent)
            {
                if (element.elementId == elementId)
                {
                    element.click.Invoke(elementId);
                }
            }
        }
    }
}
