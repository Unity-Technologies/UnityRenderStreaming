using System;
using System.Collections;
using System.Linq;
using System.Reflection;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    class SingleConnectionBehaviourTest : SingleConnection, IMonoBehaviourTest
    {
        public bool IsTestFinished
        {
            get { return true; }
        }
    }

    class BroadcastBehaviourTest : Broadcast, IMonoBehaviourTest
    {
        public bool IsTestFinished
        {
            get { return true; }
        }
    }

    class VideoStreamSenderTester : VideoStreamSender
    {
        private Camera m_camera;

        internal override WaitForCreateTrack CreateTrack()
        {
            m_camera = gameObject.AddComponent<Camera>();
            var instruction = new WaitForCreateTrack();
            instruction.Done(m_camera.CaptureStreamTrack(256, 256));
            return instruction;
        }
    }

    class VideoStreamReceiverTester : VideoStreamReceiver
    {
    }

    class AudioStreamSenderTester : AudioStreamSender
    {
        private AudioSource m_audioSource;

        internal override WaitForCreateTrack CreateTrack()
        {
            m_audioSource = gameObject.AddComponent<AudioSource>();
            m_audioSource.clip = AudioClip.Create("test", 48000, 2, 48000, false);
            var instruction = new WaitForCreateTrack();
            instruction.Done(new AudioStreamTrack(m_audioSource));
            return instruction;
        }
    }

    class AudioStreamReceiverTester : AudioStreamReceiver
    {
    }

    class DataChannelTest : DataChannelBase
    {
        public Action<string> OnReceiveMessage;

        public void SetLabel(string label)
        {
            Type myClass = typeof(DataChannelBase);
            FieldInfo fieldLabel = myClass.GetField("label",
                BindingFlags.Public | BindingFlags.Instance | BindingFlags.NonPublic);
            fieldLabel.SetValue(this, label);
        }

        protected override void OnMessage(byte[] bytes)
        {
            OnReceiveMessage(System.Text.Encoding.UTF8.GetString(bytes));
        }
    }

    class TestContainer<T> : IDisposable where T : SignalingHandlerBase, IMonoBehaviourTest
    {
        const float ResendOfferInterval = 3.0f;

        public MonoBehaviourTest<T> test;
        public SignalingManagerInternal instance;
        public SignalingEventProvider provider;

        private static RenderStreamingDependencies CreateDependencies(MonoBehaviour behaviour)
        {
            return new RenderStreamingDependencies
            {
                signaling = new MockSignaling(),
                config = new RTCConfiguration
                {
                    iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
                },
                startCoroutine = behaviour.StartCoroutine,
                stopCoroutine = behaviour.StopCoroutine,
                resentOfferInterval = ResendOfferInterval,
            };
        }

        public static TestContainer<T> Create(string name)
        {
            var test = new MonoBehaviourTest<T>();
            var dependencies = CreateDependencies(test.component);
            var instance = new SignalingManagerInternal(ref dependencies);
            var provider = new SignalingEventProvider(instance);
            var container = new TestContainer<T> { test = test, instance = instance, provider = provider };
            test.component.SetHandler(instance);
            test.gameObject.name = name;
            provider.Subscribe(test.component);
            return container;
        }

        public void Dispose()
        {
            test.component.StopAllCoroutines();
            instance.Dispose();
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }
    }

    class BroadcastTest
    {
        [SetUp]
        public void SetUp()
        {
            MockSignaling.Reset(false);
        }

        //todo:: crash in dispose process on standalone linux
        [Test]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer })]
        public void AddStreamSource()
        {
            var container = TestContainer<BroadcastBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<VideoStreamSenderTester>();

            Assert.That(streamer.Track, Is.Null);
            Assert.That(streamer.Transceivers, Is.Empty);

            container.test.component.AddComponent(streamer);
            container.Dispose();
        }

        [Test]
        public void AddDataChannel()
        {
            var container = TestContainer<BroadcastBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<DataChannelTest>();
            channel.SetLabel("test");
            Assert.That(channel.Label, Is.EqualTo("test"));
            Assert.That(channel.IsConnected, Is.False);

            container.test.component.AddComponent(channel);
            container.Dispose();
        }

        //todo:: crash in dispose process on standalone linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer, RuntimePlatform.Android })]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<BroadcastBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<AudioStreamSenderTester>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            streamer.OnStartedStream += _ => isStartedStream1 = true;
            streamer.OnStoppedStream += _ => isStoppedStream1 = true;

            container1.test.component.AddComponent(streamer);

            var receiver = container2.test.gameObject.AddComponent<AudioStreamReceiverTester>();
            bool isStartedStream2 = false;
            bool isStoppedStream2 = false;

            receiver.OnStartedStream += _ => isStartedStream2 = true;
            receiver.OnStoppedStream += _ => isStoppedStream2 = true;

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));
            container2.test.component.SendOffer(connectionId);
            yield return new WaitUntil(() => isStartedStream2 && isStartedStream1);
            Assert.That(isStartedStream1, Is.True);
            Assert.That(isStartedStream2, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Transceiver, Is.Not.Null);

            yield return new WaitUntil(() => container1.test.component.IsConnected(connectionId));
            yield return new WaitUntil(() => container2.test.component.IsConnected(connectionId));

            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream1 && isStoppedStream2);
            Assert.That(isStoppedStream1, Is.True);
            Assert.That(isStoppedStream2, Is.True);

            Assert.That(container1.test.component.ExistConnection(connectionId), Is.False);
            Assert.That(container2.test.component.ExistConnection(connectionId), Is.False);


            container1.Dispose();
            container2.Dispose();
        }

        // todo:: Crash in dispose process on Linux standalone.
        // todo:: Timeout error on iPhonePlayer.
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[]
        {
            RuntimePlatform.WindowsEditor, RuntimePlatform.OSXEditor, RuntimePlatform.LinuxEditor,
            RuntimePlatform.LinuxPlayer, RuntimePlatform.IPhonePlayer, RuntimePlatform.Android
        })]
        public IEnumerator SetCodec()
        {
            string connectionId = "12345";
            var container1 = TestContainer<BroadcastBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<VideoStreamSenderTester>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            streamer.OnStartedStream += _ => isStartedStream1 = true;
            streamer.OnStoppedStream += _ => isStoppedStream1 = true;
            var codec = VideoStreamSender.GetAvailableCodecs().FirstOrDefault(x => x.mimeType.Contains("VP9"));
            Assert.That(codec, Is.Not.Null);
            streamer.SetCodec(codec);

            container1.test.component.AddComponent(streamer);

            var receiver = container2.test.gameObject.AddComponent<VideoStreamReceiverTester>();
            bool isStartedStream2 = false;
            bool isStoppedStream2 = false;

            receiver.OnStartedStream += _ => isStartedStream2 = true;
            receiver.OnStoppedStream += _ => isStoppedStream2 = true;

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));
            container2.test.component.SendOffer(connectionId);
            yield return new WaitUntil(() => isStartedStream2 && isStartedStream1);
            Assert.That(isStartedStream1, Is.True);
            Assert.That(isStartedStream2, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Transceiver, Is.Not.Null);

            yield return new WaitUntil(() => container1.test.component.IsConnected(connectionId));
            yield return new WaitUntil(() => container2.test.component.IsConnected(connectionId));

            RTCCodecStats senderCodecStats = null;
            while (senderCodecStats == null)
            {
                var statsOp = streamer.Transceivers[connectionId].Sender.GetStats();
                yield return statsOp;
                Assert.That(statsOp.IsError, Is.False);

                var outboundStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x =>
                        x is RTCOutboundRTPStreamStats stats && stats.kind == "video") as RTCOutboundRTPStreamStats;

                if (outboundStats == null || string.IsNullOrEmpty(outboundStats.codecId))
                {
                    yield return new WaitForSeconds(0.1f);
                    continue;
                }

                senderCodecStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x => x.Id == outboundStats.codecId) as RTCCodecStats;
            }
            Assert.That(senderCodecStats.mimeType, Is.EqualTo(codec.mimeType));
            Assert.That(senderCodecStats.sdpFmtpLine, Is.EqualTo(codec.sdpFmtpLine));

            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream1 && isStoppedStream2);
            Assert.That(isStoppedStream1, Is.True);
            Assert.That(isStoppedStream2, Is.True);

            Assert.That(container1.test.component.ExistConnection(connectionId), Is.False);
            Assert.That(container2.test.component.ExistConnection(connectionId), Is.False);

            container1.Dispose();
            container2.Dispose();
        }
    }

    class SingleConnectionTest
    {
        [SetUp]
        public void SetUp()
        {
            MockSignaling.Reset(true);
        }

        //todo:: crash in dispose process on standalone Linux and Android
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer, RuntimePlatform.Android })]
        public IEnumerator AddAudioStreamSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<AudioStreamSenderTester>();

            Assert.That(streamer.Track, Is.Null);
            Assert.That(streamer.Transceivers, Is.Empty);

            container.test.component.AddComponent(streamer);
            container.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.test.component.ExistConnection(connectionId));

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Transceivers, Is.Not.Empty);

            // SetCodec
            streamer.SetCodec(null);

            // SetBitrate
            var maxBitrate = streamer.maxBitrate;
            var minBitrate = streamer.maxBitrate;
            streamer.SetBitrate(minBitrate, maxBitrate);

            container.test.component.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.test.component.ExistConnection(connectionId));
            container.Dispose();
        }

        //todo:: crash in dispose process on standalone Linux and Android
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer, RuntimePlatform.Android })]
        public IEnumerator AddVideoStreamSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<VideoStreamSenderTester>();

            Assert.That(streamer.Track, Is.Null);
            Assert.That(streamer.Transceivers, Is.Empty);

            container.test.component.AddComponent(streamer);
            container.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.test.component.ExistConnection(connectionId));

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Transceivers, Is.Not.Empty);

            // SetCodec
            streamer.SetCodec(null);

            // SetFramerate
            var frameRate = streamer.frameRate;
            streamer.SetFrameRate(frameRate);

            // SetBitrate
            var maxBitrate = streamer.maxBitrate;
            var minBitrate = streamer.maxBitrate;
            streamer.SetBitrate(minBitrate, maxBitrate);

            // SetScaleResolutionDown
            var scaleFactor = streamer.scaleResolutionDown;
            streamer.SetScaleResolutionDown(scaleFactor);

            // SetTextureSize
            var width = streamer.width;
            var height = streamer.height;
            streamer.SetTextureSize(new Vector2Int((int)width, (int)height));

            container.test.component.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.test.component.ExistConnection(connectionId));
            container.Dispose();
        }

        [UnityTest, Timeout(10000)]
        public IEnumerator AddDataChannel()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var handler = container.test.component;
            var channel = container.test.gameObject.AddComponent<DataChannelTest>();
            channel.SetLabel("test");

            handler.AddComponent(channel);
            handler.CreateConnection(connectionId);

            yield return new WaitUntil(() => container.test.component.ExistConnection(connectionId));

            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.test.component.ExistConnection(connectionId));
            container.Dispose();
        }

        [UnityTest, Timeout(10000)]
        public IEnumerator AddSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<DataChannelTest>();

            channel.SetLabel("test");

            Assert.That(channel.IsConnected, Is.False);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.AddComponent(channel);
            container.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.test.component.ExistConnection(connectionId));

            Assert.That(channel.IsConnected, Is.False);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.test.component.ExistConnection(connectionId));
            container.Dispose();
        }


        // todo:: Crash in dispose process on Linux standalone
        // todo:: Timeout error on iPhonePlayer
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer, RuntimePlatform.IPhonePlayer, RuntimePlatform.Android })]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<VideoStreamSenderTester>();
            bool isStartedStream0 = false;
            bool isStoppedStream0 = false;
            streamer.OnStartedStream += _ => isStartedStream0 = true;
            streamer.OnStoppedStream += _ => isStoppedStream0 = true;

            container1.test.component.AddComponent(streamer);
            container1.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.test.component.ExistConnection(connectionId));
            yield return new WaitUntil(() => isStartedStream0);
            Assert.That(isStartedStream0, Is.True);

            var receiver = container2.test.gameObject.AddComponent<VideoStreamReceiverTester>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            receiver.OnStartedStream += _ => isStartedStream1 = true;
            receiver.OnStoppedStream += _ => isStoppedStream1 = true;

            Assert.That(receiver.Track, Is.Null);
            Assert.That(receiver.Transceiver, Is.Null);

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream1);
            Assert.That(isStartedStream1, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Transceiver, Is.Not.Null);

            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream0 && isStoppedStream1);
            Assert.That(isStoppedStream0, Is.True);
            Assert.That(isStoppedStream1, Is.True);

            container1.Dispose();
            container2.Dispose();
        }

        //todo(kazuki):: Unknown error is occurred on Android
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.Android })]
        public IEnumerator ReceiveDataChannel()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var channel1 = container1.test.gameObject.AddComponent<DataChannelTest>();
            bool isStartedChannel1 = false;
            bool isStoppedChannel1 = false;

            channel1.OnStartedChannel += _ => isStartedChannel1 = true;
            channel1.OnStoppedChannel += _ => isStoppedChannel1 = true;

            container1.test.component.AddComponent(channel1);
            container1.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.test.component.ExistConnection(connectionId));

            var channel2 = container2.test.gameObject.AddComponent<DataChannelTest>();
            bool isStartedChannel2 = false;
            bool isStoppedChannel2 = false;
            channel2.OnStartedChannel += _ => isStartedChannel2 = true;
            channel2.OnStoppedChannel += _ => isStoppedChannel2 = true;
            channel2.SetLabel("test");

            Assert.That(channel2.IsConnected, Is.False);
            Assert.That(channel2.Label, Is.EqualTo("test"));

            container2.test.component.AddComponent(channel2);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));
            yield return new WaitUntil(() => isStartedChannel1 && isStartedChannel2);
            Assert.That(isStartedChannel1, Is.True);
            Assert.That(isStartedChannel2, Is.True);

            Assert.That(channel1.Label, Is.EqualTo("test"));

            Assert.That(channel1.IsConnected, Is.True);
            Assert.That(channel2.IsConnected, Is.True);

            // send message from channel1 to channel2
            string sendMessage = "hello";
            string receivedMessage = null;
            channel2.OnReceiveMessage = message => { receivedMessage = message; };
            channel1.Send(sendMessage);
            yield return new WaitUntil(() => !string.IsNullOrEmpty(receivedMessage));
            Assert.That(receivedMessage, Is.EqualTo(sendMessage));

            // send message from channel2 to channel1
            receivedMessage = null;
            channel1.OnReceiveMessage = message => { receivedMessage = message; };
            channel2.Send(sendMessage);
            yield return new WaitUntil(() => !string.IsNullOrEmpty(receivedMessage));
            Assert.That(receivedMessage, Is.EqualTo(sendMessage));

            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedChannel1 && isStoppedChannel2);
            Assert.That(isStoppedChannel1, Is.True);
            Assert.That(isStoppedChannel2, Is.True);

            container1.Dispose();
            container2.Dispose();
        }

        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer, RuntimePlatform.Android })]
        public IEnumerator AssignTransceivers()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            // prepare caller
            var videoStreamer1 = container1.test.gameObject.AddComponent<VideoStreamSenderTester>();
            bool isStartedVideoSourceStream1 = false;
            bool isStoppedVideoSourceStream1 = false;
            videoStreamer1.OnStartedStream += _ => isStartedVideoSourceStream1 = true;
            videoStreamer1.OnStoppedStream += _ => isStoppedVideoSourceStream1 = true;
            var audioStreamer1 = container1.test.gameObject.AddComponent<AudioStreamSenderTester>();
            bool isStartedAudioSourceStream1 = false;
            bool isStoppedAudioSourceStream1 = false;
            audioStreamer1.OnStartedStream += _ => isStartedAudioSourceStream1 = true;
            audioStreamer1.OnStoppedStream += _ => isStoppedAudioSourceStream1 = true;
            var videoReceiver1 = container1.test.gameObject.AddComponent<VideoStreamReceiverTester>();
            bool isStartedVideoReceiveStream1 = false;
            bool isStoppedVideoReceiveStream1 = false;
            videoReceiver1.OnStartedStream += _ => isStartedVideoReceiveStream1 = true;
            videoReceiver1.OnStoppedStream += _ => isStoppedVideoReceiveStream1 = true;
            var audioReceiver1 = container1.test.gameObject.AddComponent<AudioStreamReceiverTester>();
            bool isStartedAudioReceiveStream1 = false;
            bool isStoppedAudioReceiveStream1 = false;
            audioReceiver1.OnStartedStream += _ => isStartedAudioReceiveStream1 = true;
            audioReceiver1.OnStoppedStream += _ => isStoppedAudioReceiveStream1 = true;

            container1.test.component.AddComponent(videoStreamer1);
            container1.test.component.AddComponent(videoReceiver1);
            container1.test.component.AddComponent(audioStreamer1);
            container1.test.component.AddComponent(audioReceiver1);

            // prepare callee
            var videoStreamer2 = container2.test.gameObject.AddComponent<VideoStreamSenderTester>();
            bool isStartedVideoSourceStream2 = false;
            bool isStoppedVideoSourceStream2 = false;
            videoStreamer2.OnStartedStream += _ => isStartedVideoSourceStream2 = true;
            videoStreamer2.OnStoppedStream += _ => isStoppedVideoSourceStream2 = true;
            var audioStreamer2 = container2.test.gameObject.AddComponent<AudioStreamSenderTester>();
            bool isStartedAudioSourceStream2 = false;
            bool isStoppedAudioSourceStream2 = false;
            audioStreamer2.OnStartedStream += _ => isStartedAudioSourceStream2 = true;
            audioStreamer2.OnStoppedStream += _ => isStoppedAudioSourceStream2 = true;
            var videoReceiver2 = container2.test.gameObject.AddComponent<VideoStreamReceiverTester>();
            bool isStartedVideoReceiveStream2 = false;
            bool isStoppedVideoReceiveStream2 = false;
            videoReceiver2.OnStartedStream += _ => isStartedVideoReceiveStream2 = true;
            videoReceiver2.OnStoppedStream += _ => isStoppedVideoReceiveStream2 = true;
            var audioReceiver2 = container2.test.gameObject.AddComponent<AudioStreamReceiverTester>();
            bool isStartedAudioReceiveStream2 = false;
            bool isStoppedAudioReceiveStream2 = false;
            audioReceiver2.OnStartedStream += _ => isStartedAudioReceiveStream2 = true;
            audioReceiver2.OnStoppedStream += _ => isStoppedAudioReceiveStream2 = true;

            container2.test.component.AddComponent(videoStreamer2);
            container2.test.component.AddComponent(videoReceiver2);
            container2.test.component.AddComponent(audioStreamer2);
            container2.test.component.AddComponent(audioReceiver2);

            // start signaling
            container1.test.component.CreateConnection(connectionId);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() =>
                container1.test.component.ExistConnection(connectionId) &&
                container2.test.component.ExistConnection(connectionId));
            yield return new WaitUntil(() => isStartedVideoSourceStream1 && isStartedAudioSourceStream1 && isStartedVideoSourceStream2 && isStartedAudioSourceStream2);
            yield return new WaitUntil(() => isStartedVideoReceiveStream1 && isStartedAudioReceiveStream1 && isStartedVideoReceiveStream2 && isStartedAudioReceiveStream2);
            yield return new WaitUntil(() =>
                container1.test.component.IsStable(connectionId) && container2.test.component.IsStable(connectionId));

            var transceivers1 = container1.instance.GetTransceivers(connectionId).ToList();
            var count1 = transceivers1.Count;
            Assert.That(count1, Is.EqualTo(4), $"{nameof(transceivers1)} count is {count1}");
            Assert.That(transceivers1.Select(x => x.Direction),
                Is.EquivalentTo(new[]
                {
                    RTCRtpTransceiverDirection.SendOnly, RTCRtpTransceiverDirection.SendOnly,
                    RTCRtpTransceiverDirection.RecvOnly, RTCRtpTransceiverDirection.RecvOnly,
                }));
            var transceivers2 = container2.instance.GetTransceivers(connectionId).ToList();
            var count2 = transceivers2.Count;
            Assert.That(count2, Is.EqualTo(4), $"{nameof(transceivers2)} count is {count2}");
            Assert.That(transceivers2.Select(x => x.Direction),
                Is.EquivalentTo(new[]
                {
                    RTCRtpTransceiverDirection.SendOnly, RTCRtpTransceiverDirection.SendOnly,
                    RTCRtpTransceiverDirection.RecvOnly, RTCRtpTransceiverDirection.RecvOnly,
                }));


            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedVideoSourceStream1 && isStoppedAudioSourceStream1 && isStoppedVideoSourceStream2 && isStoppedAudioSourceStream2);
            yield return new WaitUntil(() => isStoppedVideoReceiveStream1 && isStoppedAudioReceiveStream1 && isStoppedVideoReceiveStream2 && isStoppedAudioReceiveStream2);
            yield return new WaitUntil(() =>
                !container1.test.component.ExistConnection(connectionId) &&
                !container2.test.component.ExistConnection(connectionId));

            container1.Dispose();
            container2.Dispose();
        }

        //todo:: crash in dispose process on standalone linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[]
        {
            RuntimePlatform.WindowsEditor, RuntimePlatform.OSXEditor, RuntimePlatform.LinuxEditor,
            RuntimePlatform.LinuxPlayer, RuntimePlatform.Android
        })]
        public IEnumerator SetCodecOnSender()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<VideoStreamSenderTester>();
            bool isStartedStream0 = false;
            bool isStoppedStream0 = false;
            streamer.OnStartedStream += _ => isStartedStream0 = true;
            streamer.OnStoppedStream += _ => isStoppedStream0 = true;
            var codec = VideoStreamSender.GetAvailableCodecs().FirstOrDefault(x => x.mimeType.Contains("VP9"));
            Assert.That(codec, Is.Not.Null);
            streamer.SetCodec(codec);


            container1.test.component.AddComponent(streamer);
            container1.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream0);
            Assert.That(isStartedStream0, Is.True);

            var receiver = container2.test.gameObject.AddComponent<VideoStreamReceiverTester>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            receiver.OnStartedStream += _ => isStartedStream1 = true;
            receiver.OnStoppedStream += _ => isStoppedStream1 = true;

            Assert.That(receiver.Track, Is.Null);
            Assert.That(receiver.Transceiver, Is.Null);

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream1);
            Assert.That(isStartedStream1, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Transceiver, Is.Not.Null);

            RTCCodecStats senderCodecStats = null;
            while (senderCodecStats == null)
            {
                var statsOp = streamer.Transceivers[connectionId].Sender.GetStats();
                yield return statsOp;
                Assert.That(statsOp.IsError, Is.False);

                var outboundStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x =>
                        x is RTCOutboundRTPStreamStats stats && stats.kind == "video") as RTCOutboundRTPStreamStats;

                if (outboundStats == null || string.IsNullOrEmpty(outboundStats.codecId))
                {
                    yield return new WaitForSeconds(0.1f);
                    continue;
                }

                senderCodecStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x => x.Id == outboundStats.codecId) as RTCCodecStats;
            }
            Assert.That(senderCodecStats.mimeType, Is.EqualTo(codec.mimeType));
            Assert.That(senderCodecStats.sdpFmtpLine, Is.EqualTo(codec.sdpFmtpLine));

            RTCCodecStats receiverCodecStats = null;
            while (receiverCodecStats == null)
            {
                var statsOp = receiver.Transceiver.Receiver.GetStats();
                yield return statsOp;
                Assert.That(statsOp.IsError, Is.False);

                var inboundStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x =>
                        x is RTCInboundRTPStreamStats stats && stats.kind == "video") as RTCInboundRTPStreamStats;

                if (inboundStats == null || string.IsNullOrEmpty(inboundStats.codecId))
                {
                    yield return new WaitForSeconds(0.1f);
                    continue;
                }

                receiverCodecStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x => x.Id == inboundStats.codecId) as RTCCodecStats;
            }
            Assert.That(receiverCodecStats.mimeType, Is.EqualTo(codec.mimeType));
            Assert.That(receiverCodecStats.sdpFmtpLine, Is.EqualTo(codec.sdpFmtpLine));

            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream0 && isStoppedStream1);
            Assert.That(isStoppedStream0, Is.True);
            Assert.That(isStoppedStream1, Is.True);

            container1.Dispose();
            container2.Dispose();
        }

        //todo:: crash in dispose process on standalone linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[]
        {
            RuntimePlatform.WindowsEditor, RuntimePlatform.OSXEditor, RuntimePlatform.LinuxEditor,
            RuntimePlatform.LinuxPlayer, RuntimePlatform.Android
        })]
        public IEnumerator SetCodecOnReceiver()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<VideoStreamSenderTester>();
            bool isStartedStream0 = false;
            bool isStoppedStream0 = false;
            streamer.OnStartedStream += _ => isStartedStream0 = true;
            streamer.OnStoppedStream += _ => isStoppedStream0 = true;

            container1.test.component.AddComponent(streamer);
            container1.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream0);
            Assert.That(isStartedStream0, Is.True);

            var receiver = container2.test.gameObject.AddComponent<VideoStreamReceiverTester>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            receiver.OnStartedStream += _ => isStartedStream1 = true;
            receiver.OnStoppedStream += _ => isStoppedStream1 = true;
            var codec = VideoStreamSender.GetAvailableCodecs().FirstOrDefault(x => x.mimeType.Contains("VP9"));
            Assert.That(codec, Is.Not.Null);
            streamer.SetCodec(codec);

            Assert.That(receiver.Track, Is.Null);
            Assert.That(receiver.Transceiver, Is.Null);

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream1);
            Assert.That(isStartedStream1, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Transceiver, Is.Not.Null);

            RTCCodecStats senderCodecStats = null;
            while (senderCodecStats == null)
            {
                var statsOp = streamer.Transceivers[connectionId].Sender.GetStats();
                yield return statsOp;
                Assert.That(statsOp.IsError, Is.False);

                var outboundStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x =>
                        x is RTCOutboundRTPStreamStats stats && stats.kind == "video") as RTCOutboundRTPStreamStats;

                if (outboundStats == null || string.IsNullOrEmpty(outboundStats.codecId))
                {
                    yield return new WaitForSeconds(0.1f);
                    continue;
                }

                senderCodecStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x => x.Id == outboundStats.codecId) as RTCCodecStats;
            }
            Assert.That(senderCodecStats.mimeType, Is.EqualTo(codec.mimeType));
            Assert.That(senderCodecStats.sdpFmtpLine, Is.EqualTo(codec.sdpFmtpLine));

            RTCCodecStats receiverCodecStats = null;
            while (receiverCodecStats == null)
            {
                var statsOp = receiver.Transceiver.Receiver.GetStats();
                yield return statsOp;
                Assert.That(statsOp.IsError, Is.False);

                var inboundStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x =>
                        x is RTCInboundRTPStreamStats stats && stats.kind == "video") as RTCInboundRTPStreamStats;

                if (inboundStats == null || string.IsNullOrEmpty(inboundStats.codecId))
                {
                    yield return new WaitForSeconds(0.1f);
                    continue;
                }

                receiverCodecStats =
                    statsOp.Value.Stats.Values.FirstOrDefault(x => x.Id == inboundStats.codecId) as RTCCodecStats;
            }
            Assert.That(receiverCodecStats.mimeType, Is.EqualTo(codec.mimeType));
            Assert.That(receiverCodecStats.sdpFmtpLine, Is.EqualTo(codec.sdpFmtpLine));

            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream0 && isStoppedStream1);
            Assert.That(isStoppedStream0, Is.True);
            Assert.That(isStoppedStream1, Is.True);

            container1.Dispose();
            container2.Dispose();
        }
    }
}
