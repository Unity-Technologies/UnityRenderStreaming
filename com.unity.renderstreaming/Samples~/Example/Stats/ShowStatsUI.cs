using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    internal class ShowStatsUI : MonoBehaviour
    {
        [SerializeField] private Button showStatsButton;
        [SerializeField] private Button hideStatsButton;
        [SerializeField] private GameObject scrollView;
        [SerializeField] private RectTransform displayParent;
        [SerializeField] private Text baseText;
        [SerializeField] private List<SignalingHandlerBase> signalingHandlerList;

        private Dictionary<string, HashSet<RTCRtpSender>> activeSenderList =
            new Dictionary<string, HashSet<RTCRtpSender>>();

        private Dictionary<StreamReceiverBase, HashSet<RTCRtpReceiver>> activeReceiverList =
            new Dictionary<StreamReceiverBase, HashSet<RTCRtpReceiver>>();

        private Dictionary<RTCRtpSender, StatsDisplay> lastSenderStats =
            new Dictionary<RTCRtpSender, StatsDisplay>();

        private Dictionary<RTCRtpReceiver, StatsDisplay> lastReceiverStats =
            new Dictionary<RTCRtpReceiver, StatsDisplay>();

        private HashSet<StreamSenderBase> alreadySetupSenderList = new HashSet<StreamSenderBase>();

        private void Awake()
        {
            showStatsButton.onClick.AddListener(ShowStats);
            hideStatsButton.onClick.AddListener(HideStats);
        }

        private void ShowStats()
        {
            scrollView.gameObject.SetActive(true);
            hideStatsButton.gameObject.SetActive(true);
            showStatsButton.gameObject.SetActive(false);
        }

        private void HideStats()
        {
            scrollView.gameObject.SetActive(false);
            showStatsButton.gameObject.SetActive(true);
            hideStatsButton.gameObject.SetActive(false);
        }

        private void Start()
        {
            StartCoroutine(CollectStats());
        }

        private void OnDestroy()
        {
            lastSenderStats.Clear();
            lastReceiverStats.Clear();
            activeSenderList.Clear();
            activeReceiverList.Clear();
            alreadySetupSenderList.Clear();
        }

        public void AddSignalingHandler(SignalingHandlerBase handlerBase)
        {
            if (signalingHandlerList.Contains(handlerBase))
            {
                return;
            }

            signalingHandlerList.Add(handlerBase);
        }

        class StatsDisplay
        {
            public Text display;
            public RTCStatsReport lastReport;
        }

        private IEnumerator CollectStats()
        {
            var waitSec = new WaitForSeconds(1);

            while (true)
            {
                yield return waitSec;

                foreach (var streamBase in signalingHandlerList.SelectMany(x => x.Streams))
                {
                    if (streamBase is StreamSenderBase senderBase)
                    {
                        SetUpSenderBase(senderBase);
                    }

                    if (streamBase is StreamReceiverBase receiverBase)
                    {
                        SetUpReceiverBase(receiverBase);
                    }
                }

                List<Coroutine> coroutines = new List<Coroutine>();

                foreach (var sender in activeSenderList.Values.SelectMany(x => x))
                {
                    var coroutine = StartCoroutine(UpdateStats(sender));
                    coroutines.Add(coroutine);
                }

                foreach (var receiver in activeReceiverList.Values.SelectMany(x => x))
                {
                    var coroutine = StartCoroutine(UpdateStats(receiver));
                    coroutines.Add(coroutine);
                }
                foreach (var coroutine in coroutines)
                {
                    yield return coroutine;
                }
                var noStatsData = !lastSenderStats.Any() && !lastReceiverStats.Any();
                baseText.gameObject.SetActive(noStatsData);
            }
        }

        IEnumerator UpdateStats(RTCRtpReceiver receiver)
        {
            var op = receiver.GetStats();
            yield return new WaitUntilWithTimeout(() => op.IsDone, 3f);

            if (op.IsError || !op.IsDone)
            {
                yield break;
            }

            var report = op.Value;
            if (report == null)
            {
                yield break;
            }

            if (lastReceiverStats.TryGetValue(receiver, out var statsDisplay))
            {
                var lastReport = statsDisplay.lastReport;
                statsDisplay.display.text = CreateDisplayString(report, lastReport);
                statsDisplay.lastReport = report;
                lastReport.Dispose();
            }
            else
            {
                var text = Instantiate(baseText, displayParent);
                text.text = "";
                text.gameObject.SetActive(true);
                lastReceiverStats[receiver] = new StatsDisplay { display = text, lastReport = report };
            }
        }

        IEnumerator UpdateStats(RTCRtpSender sender)
        {
            var op = sender.GetStats();
            yield return new WaitUntilWithTimeout(() => op.IsDone, 3f);

            if (op.IsError || !op.IsDone)
            {
                yield break;
            }

            var report = op.Value;
            if (report == null)
            {
                yield break;
            }

            if (lastSenderStats.TryGetValue(sender, out var statsDisplay))
            {
                var lastReport = statsDisplay.lastReport;
                statsDisplay.display.text = CreateDisplayString(report, lastReport);
                statsDisplay.lastReport = report;
                lastReport.Dispose();
            }
            else
            {
                var text = Instantiate(baseText, displayParent);
                text.text = "";
                text.gameObject.SetActive(true);
                lastSenderStats[sender] = new StatsDisplay { display = text, lastReport = report };
            }
        }

        private void SetUpSenderBase(StreamSenderBase senderBase)
        {
            if (alreadySetupSenderList.Contains(senderBase))
            {
                return;
            }

            senderBase.OnStartedStream += id =>
            {
                if (!activeSenderList.ContainsKey(id))
                {
                    activeSenderList[id] = new HashSet<RTCRtpSender>();
                }

                if (senderBase.Transceivers.TryGetValue(id, out var transceiver))
                {
                    activeSenderList[id].Add(transceiver.Sender);
                }
            };
            senderBase.OnStoppedStream += id =>
            {
                if (activeSenderList.TryGetValue(id, out var hashSet))
                {
                    foreach (var sender in hashSet)
                    {
                        if (lastSenderStats.TryGetValue(sender, out var statsDisplay))
                        {
                            DestroyImmediate(statsDisplay.display.gameObject);
                            lastSenderStats.Remove(sender);
                        }
                    }
                }
                activeSenderList.Remove(id);
            };

            foreach (var pair in senderBase.Transceivers)
            {
                if (!activeSenderList.ContainsKey(pair.Key))
                {
                    activeSenderList[pair.Key] = new HashSet<RTCRtpSender>();
                }

                activeSenderList[pair.Key].Add(pair.Value.Sender);
            }

            alreadySetupSenderList.Add(senderBase);
        }

        private void SetUpReceiverBase(StreamReceiverBase receiverBase)
        {
            if (activeReceiverList.ContainsKey(receiverBase))
            {
                return;
            }

            activeReceiverList[receiverBase] = new HashSet<RTCRtpReceiver>();

            receiverBase.OnStartedStream += id =>
            {
                if (activeReceiverList.TryGetValue(receiverBase, out var hashSet))
                {
                    hashSet.Add(receiverBase.Transceiver.Receiver);
                }
            };
            receiverBase.OnStoppedStream += id =>
            {
                if (activeReceiverList.TryGetValue(receiverBase, out var hashSet))
                {
                    foreach (var receiver in hashSet)
                    {
                        if (lastReceiverStats.TryGetValue(receiver, out var statsDisplay))
                        {
                            DestroyImmediate(statsDisplay.display.gameObject);
                            lastReceiverStats.Remove(receiver);
                        }
                    }
                }
                activeReceiverList.Remove(receiverBase);
            };

            var transceiver = receiverBase.Transceiver;
            if (transceiver != null && transceiver.Receiver != null)
            {
                activeReceiverList[receiverBase].Add(transceiver.Receiver);
            }
        }

        private static string CreateDisplayString(RTCStatsReport report, RTCStatsReport lastReport)
        {
            var builder = new StringBuilder();

            foreach (var stats in report.Stats.Values)
            {
                if (stats is RTCInboundRTPStreamStats inboundStats)
                {
                    builder.AppendLine($"{inboundStats.kind} receiving stream stats");
                    if (inboundStats.codecId != null && report.Get(inboundStats.codecId) is RTCCodecStats codecStats)
                    {
                        builder.AppendLine($"Codec: {codecStats.mimeType}");
                        if (!string.IsNullOrEmpty(codecStats.sdpFmtpLine))
                        {
                            foreach (var fmtp in codecStats.sdpFmtpLine.Split(';'))
                            {
                                builder.AppendLine($" - {fmtp}");
                            }
                        }

                        if (codecStats.payloadType > 0)
                        {
                            builder.AppendLine($" - {nameof(codecStats.payloadType)}={codecStats.payloadType}");
                        }

                        if (codecStats.clockRate > 0)
                        {
                            builder.AppendLine($" - {nameof(codecStats.clockRate)}={codecStats.clockRate}");
                        }

                        if (codecStats.channels > 0)
                        {
                            builder.AppendLine($" - {nameof(codecStats.channels)}={codecStats.channels}");
                        }
                    }

                    if (inboundStats.kind == "video")
                    {
                        builder.AppendLine($"Decoder: {inboundStats.decoderImplementation}");
                        builder.AppendLine($"Resolution: {inboundStats.frameWidth}x{inboundStats.frameHeight}");
                        builder.AppendLine($"Framerate: {inboundStats.framesPerSecond}");
                    }

                    if (lastReport.TryGetValue(inboundStats.Id, out var lastStats) &&
                        lastStats is RTCInboundRTPStreamStats lastInboundStats)
                    {
                        var duration = (double)(inboundStats.Timestamp - lastInboundStats.Timestamp) / 1000000;
                        var bitrate = (8 * (inboundStats.bytesReceived - lastInboundStats.bytesReceived) / duration) / 1000;
                        builder.AppendLine($"Bitrate: {bitrate:F2} kbit/sec");
                    }
                }
                else if (stats is RTCOutboundRTPStreamStats outboundStats)
                {
                    builder.AppendLine($"{outboundStats.kind} sending stream stats");
                    if (outboundStats.codecId != null && report.Get(outboundStats.codecId) is RTCCodecStats codecStats)
                    {
                        builder.AppendLine($"Codec: {codecStats.mimeType}");
                        if (!string.IsNullOrEmpty(codecStats.sdpFmtpLine))
                        {
                            foreach (var fmtp in codecStats.sdpFmtpLine.Split(';'))
                            {
                                builder.AppendLine($" - {fmtp}");
                            }
                        }

                        if (codecStats.payloadType > 0)
                        {
                            builder.AppendLine($" - {nameof(codecStats.payloadType)}={codecStats.payloadType}");
                        }

                        if (codecStats.clockRate > 0)
                        {
                            builder.AppendLine($" - {nameof(codecStats.clockRate)}={codecStats.clockRate}");
                        }

                        if (codecStats.channels > 0)
                        {
                            builder.AppendLine($" - {nameof(codecStats.channels)}={codecStats.channels}");
                        }
                    }

                    if (outboundStats.kind == "video")
                    {
                        builder.AppendLine($"Encoder: {outboundStats.encoderImplementation}");
                        builder.AppendLine($"Resolution: {outboundStats.frameWidth}x{outboundStats.frameHeight}");
                        builder.AppendLine($"Framerate: {outboundStats.framesPerSecond}");
                    }

                    if (lastReport.TryGetValue(outboundStats.Id, out var lastStats) &&
                        lastStats is RTCOutboundRTPStreamStats lastOutboundStats)
                    {
                        var duration = (double)(outboundStats.Timestamp - lastOutboundStats.Timestamp) / 1000000;
                        var bitrate = (8 * (outboundStats.bytesSent - lastOutboundStats.bytesSent) / duration) / 1000;
                        builder.AppendLine($"Bitrate: {bitrate:F2} kbit/sec");
                    }
                }
            }

            return builder.ToString();
        }
    }

    internal class WaitUntilWithTimeout : CustomYieldInstruction
    {
        public bool IsCompleted { get; private set; }

        private readonly float timeoutTime;

        private readonly System.Func<bool> predicate;

        public override bool keepWaiting
        {
            get
            {
                IsCompleted = predicate();
                if (IsCompleted)
                {
                    return false;
                }

                return !(Time.realtimeSinceStartup >= timeoutTime);
            }
        }

        public WaitUntilWithTimeout(System.Func<bool> predicate, float timeout)
        {
            this.timeoutTime = Time.realtimeSinceStartup + timeout;
            this.predicate = predicate;
        }
    }
}
