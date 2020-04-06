using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.WebRTC;
using System.Text.RegularExpressions;

namespace Unity.RenderStreaming
{
    [Serializable]
    public class ButtonClickEvent : UnityEngine.Events.UnityEvent<int>
    {
    }

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
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers")]
        private RTCIceServer[] iceServers = new RTCIceServer[]
        {
            new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
        };

        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        private Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Tooltip("Streaming bit rate")]
        private int bitRate = 1000000;

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Camera to capture video stream")]
        private Camera captureCamera;

        [SerializeField, Tooltip("Enable or disable hardware encoder")]
        private bool hardwareEncoderSupport = true;

        [SerializeField, Tooltip("Array to set your own click event")]
        private ButtonClickElement[] arrayButtonClickEvent;

#pragma warning restore 0649

        private Signaling signaling;
        private Dictionary<string, RTCPeerConnection> pcs = new Dictionary<string, RTCPeerConnection>();
        private Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>> mapChannels = new Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>>();
        private RTCConfiguration conf;
        private MediaStream videoStream;
        private MediaStream audioStream;

        void Awake()
        {
            var encoderType = hardwareEncoderSupport ? EncoderType.Hardware : EncoderType.Software;
            WebRTC.WebRTC.Initialize(encoderType);
            RemoteInput.Initialize();
            RemoteInput.ActionButtonClick = OnButtonClick;
        }

        void OnDestroy()
        {
            WebRTC.WebRTC.Dispose();
            RemoteInput.Destroy();
            Unity.WebRTC.Audio.Stop();
        }

        void Start()
        {
            videoStream = captureCamera.CaptureStream(streamingSize.x, streamingSize.y, bitRate);
            audioStream = Unity.WebRTC.Audio.CaptureStream();

            conf = default;
            conf.iceServers = iceServers;


            StartCoroutine(WebRTC.WebRTC.Update());
        }

        void OnEnable()
        {
            if (this.signaling == null)
            {
                this.signaling = new Signaling(urlSignaling, interval);
                this.signaling.OnOffer += OnOffer;
                this.signaling.OnIceCandidate += OnIceCandidate;
            }

            this.signaling.Start();
        }

        void OnDisable()
        {
            if (this.signaling != null)
            {
                this.signaling.Stop();
                this.signaling = null;
            }
        }

        public Vector2Int GetStreamingSize() { return streamingSize; }

        void OnOffer(Signaling signaling, DescData data)
        {
            if (!pcs.TryGetValue(data.connectionId, out var pc))
            {
                pc = new RTCPeerConnection();
                pcs.Add(data.connectionId, pc);

                pc.SetConfiguration(ref conf);

                pc.OnDataChannel = new DelegateOnDataChannel(channel =>
                {
                    OnDataChannel(pc, channel);
                });

                pc.OnIceCandidate = new DelegateOnIceCandidate(candidate =>
                {
                    signaling.SendCandidate(data.connectionId, candidate);
                });

                pc.OnIceConnectionChange = new DelegateOnIceConnectionChange(state =>
                {
                    if (state == RTCIceConnectionState.Disconnected)
                    {
                        pc.Close();
                        pcs.Remove(data.connectionId);
                    }
                });

                foreach (var track in videoStream.GetTracks())
                {
                    pc.AddTrack(track);
                }

                foreach (var track in audioStream.GetTracks())
                {
                    pc.AddTrack(track);
                }
            }

            //make video bit rate starts at 16000kbits, and 160000kbits at max.
            string pattern = @"(a=fmtp:\d+ .*level-asymmetry-allowed=.*)\r\n";
            data.sdp = Regex.Replace(data.sdp, pattern,
                "$1;x-google-start-bitrate=16000;x-google-max-bitrate=160000\r\n");

            RTCSessionDescription _desc = default;
            _desc.type = RTCSdpType.Offer;
            _desc.sdp = data.sdp;


            RTCAnswerOptions options = default;

            var opSetRemoteDescription = pc.SetRemoteDescription(ref _desc);
            while (opSetRemoteDescription.MoveNext())
            {
            }

            if (opSetRemoteDescription.IsError)
            {
                Debug.LogError($"SetRemoteDescription Error: {opSetRemoteDescription.Error}");
                return;
            }

            var opCreateAnswer = pc.CreateAnswer(ref options);
            while (opCreateAnswer.MoveNext())
            {
            }

            if (opCreateAnswer.IsError)
            {
                Debug.LogError($"CreateAnswer Error: {opCreateAnswer.Error}");
                return;
            }

            RTCSessionDescription desc = opCreateAnswer.Desc;
            var opSetLocalDescription = pc.SetLocalDescription(ref desc);
            while (opSetLocalDescription.MoveNext())
            {
            }

            if (opSetLocalDescription.IsError)
            {
                Debug.LogError($"SetLocalDescription Error: {opSetLocalDescription.Error}");
                return;
            }

            signaling.SendAnswer(data.connectionId, opCreateAnswer.Desc);
        }

        void OnIceCandidate(Signaling signaling, CandidateData data)
        {
            RTCPeerConnection pc;
            if (pcs.TryGetValue(data.connectionId, out pc))
            {
                RTCIceCandidate candidate = default;
                candidate.candidate = data.candidate;
                candidate.sdpMLineIndex = data.sdpMLineIndex;
                candidate.sdpMid = data.sdpMid;
                pc.AddIceCandidate(ref candidate);
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

            if (channel.Label == "data")
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
