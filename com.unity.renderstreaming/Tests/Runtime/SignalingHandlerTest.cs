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

    class InputChannelTest : DataChannelBase
    {
        public void SetLocal(bool isLocal)
        {
            Type myClass = typeof(InputChannelTest);
            FieldInfo fieldLocal = myClass.GetField("local",
                BindingFlags.Public | BindingFlags.Instance | BindingFlags.NonPublic);
            fieldLocal.SetValue(this, true);
        }


        public void SetLabel(string label)
        {
            Type myClass = typeof(InputChannelTest);
            FieldInfo fieldLabel = myClass.GetField("label",
                BindingFlags.Public | BindingFlags.Instance | BindingFlags.NonPublic);
            fieldLabel.SetValue(this, label);
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

    class StreamReceiverTest : StreamReceiverBase
    {
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
            instance.Dispose();
            test.component.StopAllCoroutines();
            UnityEngine.Object.Destroy(test.gameObject);
        }
    }

    [UnityPlatform(exclude = new[] {
        RuntimePlatform.OSXEditor,
        RuntimePlatform.OSXPlayer,
        RuntimePlatform.LinuxEditor,
        RuntimePlatform.LinuxPlayer
    })]
    [ConditionalIgnore(ConditionalIgnore.IL2CPP, "Process.Start does not implement in IL2CPP.")]
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
        public void AddInputChannel()
        {
            var container = TestContainer<BroadcastBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<InputChannelTest>();
            channel.SetLabel("test");
            channel.SetLocal(true);

            Assert.That(channel.IsLocal, Is.True);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.AddComponent(channel);
            container.Dispose();
        }

        [UnityTest, Timeout(1000)]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<BroadcastBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<StreamSourceTest>();
            bool isStartedStream1 = false;
            streamer.OnStartedStream += _ => isStartedStream1 = true;

            container1.test.component.AddComponent(streamer);

            var receiver = container2.test.gameObject.AddComponent<StreamReceiverTest>();
            bool isStartedStream2 = false;
            receiver.OnStartedStream += _ => isStartedStream2 = true;
            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);

            yield return new WaitUntil(() => isStartedStream2 && isStartedStream1);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Receiver, Is.Not.Null);

            container1.Dispose();
            container2.Dispose();
        }
    }

    [UnityPlatform(exclude = new[] {
        RuntimePlatform.OSXEditor,
        RuntimePlatform.OSXPlayer,
        RuntimePlatform.LinuxEditor,
        RuntimePlatform.LinuxPlayer
    })]
    [ConditionalIgnore(ConditionalIgnore.IL2CPP, "Process.Start does not implement in IL2CPP.")]
    class SingleConnectionTest
    {
        [SetUp]
        public void SetUp()
        {
            MockSignaling.Reset(true);
        }

        [UnityTest, Timeout(1000)]
        public IEnumerator AddStreamSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<StreamSourceTest>();

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Empty);

            container.test.component.AddComponent(streamer);
            container.test.component.CreateConnection(connectionId);

            yield return new WaitUntil(() => streamer.Senders.Count == 1);

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Not.Empty);

            container.Dispose();
        }

        [UnityTest, Timeout(1000)]
        public IEnumerator AddInputChannel()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<InputChannelTest>();

            channel.SetLocal(true);
            channel.SetLabel("test");

            Assert.That(channel.Channel, Is.Null);
            Assert.That(channel.IsLocal, Is.True);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.test.component.AddComponent(channel);
            container.test.component.CreateConnection(connectionId);

            yield return new WaitUntil(() => channel.Channel != null);

            Assert.That(channel.Channel, Is.Not.Null);
            Assert.That(channel.Channel.Label, Is.EqualTo("test"));

            container.Dispose();
        }

        [UnityTest, Timeout(1000)]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<StreamSourceTest>();
            bool isStartedStream0 = false;
            streamer.OnStartedStream += _ => isStartedStream0 = true;

            container1.test.component.AddComponent(streamer);
            container1.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => isStartedStream0);

            var receiver = container2.test.gameObject.AddComponent<StreamReceiverTest>();
            bool isStartedStream1 = false;
            receiver.OnStartedStream += _ => isStartedStream1 = true;

            Assert.That(receiver.Track, Is.Null);
            Assert.That(receiver.Receiver, Is.Null);

            container2.test.component.AddComponent(receiver);
            container2.test.component.CreateConnection(connectionId);

            yield return new WaitUntil(() => isStartedStream1);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Receiver, Is.Not.Null);

            container1.Dispose();
            container2.Dispose();
        }

        [UnityTest, Timeout(1000)]
        public IEnumerator ReceiveInputChannel()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var channel1 = container1.test.gameObject.AddComponent<InputChannelTest>();
            bool isStartedChannel1 = false;
            channel1.OnStartedChannel += _ => isStartedChannel1 = true;
            container1.test.component.AddComponent(channel1);
            container1.test.component.CreateConnection(connectionId);

            var channel2 = container2.test.gameObject.AddComponent<InputChannelTest>();
            bool isStartedChannel2 = false;
            channel2.OnStartedChannel += _ => isStartedChannel2 = true;

            channel2.SetLocal(true);
            channel2.SetLabel("test");

            Assert.That(channel2.Channel, Is.Null);
            Assert.That(channel2.IsLocal, Is.True);
            Assert.That(channel2.Label, Is.EqualTo("test"));

            container2.test.component.AddComponent(channel2);
            container2.test.component.CreateConnection(connectionId);
            yield return new WaitUntil(() => isStartedChannel1 && isStartedChannel2);

            Assert.That(channel1.Channel, Is.Not.Null);
            Assert.That(channel1.IsLocal, Is.False);
            Assert.That(channel1.Label, Is.EqualTo("test"));

            container1.Dispose();
            container2.Dispose();
        }
    }
}
