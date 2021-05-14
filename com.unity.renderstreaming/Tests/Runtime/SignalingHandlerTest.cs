using System;
using System.Collections;
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

    class StreamSourceTest : StreamSourceBase
    {
        private Camera m_camera;

        protected override MediaStreamTrack CreateTrack()
        {
            m_camera = gameObject.AddComponent<Camera>();
            return m_camera.CaptureStreamTrack(256, 256, 0);
        }
    }

    class VideoStreamReceiverTest : StreamReceiverBase
    {
        public override TrackKind Kind { get { return TrackKind.Video; } }
    }

    class DataChannelTest : DataChannelBase
    {
        public void SetLocal(bool isLocal)
        {
            Type myClass = typeof(DataChannelBase);
            FieldInfo fieldLocal = myClass.GetField("local",
                BindingFlags.Public | BindingFlags.Instance | BindingFlags.NonPublic);
            fieldLocal.SetValue(this, true);
        }

        public void SetLabel(string label)
        {
            Type myClass = typeof(DataChannelBase);
            FieldInfo fieldLabel = myClass.GetField("label",
                BindingFlags.Public | BindingFlags.Instance | BindingFlags.NonPublic);
            fieldLabel.SetValue(this, label);
        }
    }

    class TestContainer<T> : IDisposable where T : SignalingHandlerBase, IMonoBehaviourTest
    {
        public MonoBehaviourTest<T> test;
        public RenderStreamingInternal instance;
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
                encoderType = EncoderType.Software,
                startCoroutine = behaviour.StartCoroutine
            };
        }

        public static TestContainer<T> Create(string name)
        {
            var test = new MonoBehaviourTest<T>();
            var dependencies = CreateDependencies(test.component);
            var instance = new RenderStreamingInternal(ref dependencies);
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

        [Test]
        public void AddStreamSource()
        {
            var container = TestContainer<BroadcastBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<StreamSourceTest>();

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Empty);

            container.test.component.AddComponent(streamer);
            container.Dispose();
        }

        [Test]
        public void AddDataChannel()
        {
            var container = TestContainer<BroadcastBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<DataChannelTest>();
            channel.SetLabel("test");
            channel.SetLocal(true);

            Assert.That(channel.IsLocal, Is.True);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.AddComponent(channel);
            container.Dispose();
        }

        [UnityTest, Timeout(3000)]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<BroadcastBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<StreamSourceTest>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            streamer.OnStartedStream += _ => isStartedStream1 = true;
            streamer.OnStoppedStream += _ => isStoppedStream1 = true;

            container1.test.component.AddComponent(streamer);

            var receiver = container2.test.gameObject.AddComponent<VideoStreamReceiverTest>();
            bool isStartedStream2 = false;
            bool isStoppedStream2 = false;

            receiver.OnStartedStream += _ => isStartedStream2 = true;
            receiver.OnStoppedStream += _ => isStoppedStream2 = true;
            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId, true);

            yield return new WaitUntil(() => isStartedStream2 && isStartedStream1);
            Assert.That(isStartedStream1, Is.True);
            Assert.That(isStartedStream2, Is.True);
            
            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Receiver, Is.Not.Null);

            yield return new WaitUntil(() => container1.test.component.IsConnected(connectionId));
            yield return new WaitUntil(() => container2.test.component.IsConnected(connectionId));

            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream1 && isStoppedStream2);
            Assert.That(isStoppedStream1, Is.True);
            Assert.That(isStoppedStream2, Is.True);

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

        [UnityTest, Timeout(3000)]
        public IEnumerator AddStreamSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<StreamSourceTest>();

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Empty);

            container.test.component.AddComponent(streamer);
            container.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.test.component.ExistConnection(connectionId));

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Not.Empty);

            container.test.component.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.test.component.ExistConnection(connectionId));
            container.Dispose();
        }

        [UnityTest, Timeout(3000)]
        public IEnumerator AddDataChannel()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<DataChannelTest>();

            channel.SetLocal(true);
            channel.SetLabel("test");

            Assert.That(channel.Channel, Is.Null);
            Assert.That(channel.IsLocal, Is.True);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.AddComponent(channel);
            container.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.test.component.ExistConnection(connectionId));

            Assert.That(channel.Channel, Is.Not.Null);
            Assert.That(channel.Channel.Label, Is.EqualTo("test"));

            container.test.component.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.test.component.ExistConnection(connectionId));
            container.Dispose();
        }

        [UnityTest, Timeout(3000)]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<StreamSourceTest>();
            bool isStartedStream0 = false;
            bool isStoppedStream0 = false;
            streamer.OnStartedStream += _ => isStartedStream0 = true;
            streamer.OnStoppedStream += _ => isStoppedStream0 = true;

            container1.test.component.AddComponent(streamer);
            container1.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream0);
            Assert.That(isStartedStream0, Is.True);

            var receiver = container2.test.gameObject.AddComponent<VideoStreamReceiverTest>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            receiver.OnStartedStream += _ => isStartedStream1 = true;
            receiver.OnStoppedStream += _ => isStoppedStream1 = true;

            Assert.That(receiver.Track, Is.Null);
            Assert.That(receiver.Receiver, Is.Null);

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream1);
            Assert.That(isStartedStream1, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Receiver, Is.Not.Null);

            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedStream0 && isStoppedStream1);
            Assert.That(isStoppedStream0, Is.True);
            Assert.That(isStoppedStream1, Is.True);

            container1.Dispose();
            container2.Dispose();
        }

        //todo(kazuki):: Unknown error is occurred on Android
        [UnityTest, Timeout(3000)]
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

            channel2.SetLocal(true);
            channel2.SetLabel("test");

            Assert.That(channel2.Channel, Is.Null);
            Assert.That(channel2.IsLocal, Is.True);
            Assert.That(channel2.Label, Is.EqualTo("test"));

            container2.test.component.AddComponent(channel2);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.test.component.ExistConnection(connectionId));
            yield return new WaitUntil(() => isStartedChannel1 && isStartedChannel2);
            Assert.That(isStartedChannel1, Is.True);
            Assert.That(isStartedChannel2, Is.True);

            Assert.That(channel1.Channel, Is.Not.Null);
            Assert.That(channel1.IsLocal, Is.False);
            Assert.That(channel1.Label, Is.EqualTo("test"));

            container1.test.component.DeleteConnection(connectionId);
            container2.test.component.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedChannel1 && isStoppedChannel2);
            Assert.That(isStoppedChannel1, Is.True);
            Assert.That(isStoppedChannel2, Is.True);

            container1.Dispose();
            container2.Dispose();
        }
    }
}
