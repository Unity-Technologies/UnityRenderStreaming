using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    public class ShowStatsUI : MonoBehaviour
    {
        [SerializeField] private Toggle toggle;
        [SerializeField] private GameObject scrollView;
        [SerializeField] private RectTransform displayParent;
        [SerializeField] private Text baseText;

        [SerializeField] private List<StreamSenderBase> senderBaseList;
        [SerializeField] private List<StreamReceiverBase> receiverBaseList;

        private void Awake()
        {
            toggle.onValueChanged.AddListener(SwitchToggle);
        }

        private void SwitchToggle(bool isOn)
        {
            scrollView.SetActive(isOn);
        }

        private void Start()
        {
            StartCoroutine(CollectStats());
        }

        private void OnDestroy()
        {
            lastSenderStats.Clear();
            lastReceiverStats.Clear();
        }

        private Dictionary<RTCRtpSender, StatsDisplay> lastSenderStats = new Dictionary<RTCRtpSender, StatsDisplay>();
        private Dictionary<RTCRtpReceiver, StatsDisplay> lastReceiverStats = new Dictionary<RTCRtpReceiver, StatsDisplay>();

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

                foreach (var transceiver in senderBaseList.SelectMany(x => x.Transceivers.Values)
                             .Where(x => x != null && x.Sender != null))
                {
                    var op = transceiver.Sender.GetStats();
                    yield return op;
                    if (op.IsError)
                    {
                        continue;
                    }

                    var report = op.Value;
                    if (lastSenderStats.TryGetValue(transceiver.Sender, out var statsDisplay))
                    {
                        var displayString = CreateDisplayString(report, statsDisplay.lastReport);
                        statsDisplay.display.text = displayString;
                        statsDisplay.lastReport = report;
                    }
                    else
                    {
                        var text = Instantiate(baseText, displayParent);
                        text.text = "";
                        text.gameObject.SetActive(true);
                        lastSenderStats[transceiver.Sender] = new StatsDisplay {display = text, lastReport = report};
                    }
                }

                foreach (var transceiver in receiverBaseList.Select(x => x.Transceiver)
                             .Where(x => x != null && x.Receiver != null))
                {
                    var op = transceiver.Receiver.GetStats();
                    yield return op;

                    if (op.IsError)
                    {
                        continue;
                    }

                    var report = op.Value;
                    if (lastReceiverStats.TryGetValue(transceiver.Receiver, out var statsDisplay))
                    {
                        var displayString = CreateDisplayString(report, statsDisplay.lastReport);
                        statsDisplay.display.text = displayString;
                        statsDisplay.lastReport = report;
                    }
                    else
                    {
                        var text = Instantiate(baseText, displayParent);
                        text.text = "";
                        text.gameObject.SetActive(true);
                        lastReceiverStats[transceiver.Receiver] = new StatsDisplay {display = text, lastReport = report};
                    }
                }

                if (lastSenderStats.Any() || lastReceiverStats.Any())
                {
                    baseText.gameObject.SetActive(false);
                    // Correct scroll view contents height
                    var size = displayParent.sizeDelta;
                    size.y = baseText.GetComponent<RectTransform>().sizeDelta.y * (displayParent.childCount - 1) + 100;
                    displayParent.sizeDelta = size;
                }
            }
        }

        private static string CreateDisplayString(RTCStatsReport report, RTCStatsReport lastReport)
        {
            var builder = new StringBuilder();

            foreach (var stats in report.Stats.Values)
            {
                if (stats is RTCInboundRTPStreamStats inboundStats)
                {
                    builder.AppendLine(inboundStats.Id);
                    if (inboundStats.codecId != null && report.Get(inboundStats.codecId) is RTCCodecStats codecStats)
                    {
                        builder.AppendLine(
                            $"Codec: {codecStats.mimeType} {codecStats.sdpFmtpLine}, payloadType={codecStats.payloadType}.");
                    }

                    if (inboundStats.kind == "video")
                    {
                        builder.AppendLine($"Decoder: {inboundStats.decoderImplementation}.");
                        builder.AppendLine($"Resolution: {inboundStats.frameWidth}x{inboundStats.frameHeight}");
                        builder.AppendLine($"Framerate: {inboundStats.framesPerSecond}");
                    }

                    if (lastReport.TryGetValue(inboundStats.Id, out var lastStats) && lastStats is RTCInboundRTPStreamStats lastInboundStats)
                    {
                        var duration = (double)(inboundStats.Timestamp - lastInboundStats.Timestamp) / 1000000;
                        var bitrate = (ulong)(8 * (inboundStats.bytesReceived - lastInboundStats.bytesReceived) / duration);
                        builder.AppendLine($"Bitrate: {bitrate}");
                    }
                }
                else if (stats is RTCOutboundRTPStreamStats outboundStats)
                {
                    builder.AppendLine(outboundStats.Id);
                    if (outboundStats.codecId != null && report.Get(outboundStats.codecId) is RTCCodecStats codecStats)
                    {
                        builder.AppendLine(
                            $"Codec: {codecStats.mimeType} {codecStats.sdpFmtpLine}, payloadType={codecStats.payloadType}.");
                    }

                    if (outboundStats.kind == "video")
                    {
                        builder.AppendLine($"Encoder: {outboundStats.encoderImplementation}.");
                        builder.AppendLine($"Resolution: {outboundStats.frameWidth}x{outboundStats.frameHeight}");
                        builder.AppendLine($"Framerate: {outboundStats.framesPerSecond}");
                    }

                    if (lastReport.TryGetValue(outboundStats.Id, out var lastStats) && lastStats is RTCOutboundRTPStreamStats lastOutboundStats)
                    {
                        var duration = (double)(outboundStats.Timestamp - lastOutboundStats.Timestamp) / 1000000;
                        var bitrate = (ulong)(8 * (outboundStats.bytesSent - lastOutboundStats.bytesSent) / duration);
                        builder.AppendLine($"Bitrate: {bitrate}");
                    }
                }
            }

            return builder.ToString();
        }

        public void AddSenderBase(StreamSenderBase sender)
        {
            if (senderBaseList.Contains(sender))
            {
                return;
            }

            senderBaseList.Add(sender);
        }

        public void AddReceiverBase(StreamReceiverBase receiver)
        {
            if (receiverBaseList.Contains(receiver))
            {
                return;
            }

            receiverBaseList.Add(receiver);
        }
    }
}
