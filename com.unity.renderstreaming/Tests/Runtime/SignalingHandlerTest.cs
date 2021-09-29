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

    class StreamSender : StreamSenderBase
    {
        private Camera m_camera;

        protected override MediaStreamTrack CreateTrack()
        {
            m_camera = gameObject.AddComponent<Camera>();
            return m_camera.CaptureStreamTrack(256, 256, 0);
        }
    }

    class StreamReceiver : StreamReceiverBase
    {
        public override TrackKind Kind { get { return TrackKind.Video; } }
    }

    class DataChannel : DataChannelBase
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
                startCoroutine = behaviour.StartCoroutine,
                resentOfferInterval = 1.0f,
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

        public T handler
        {
            get { return test.component; }
        }
    }

    class StreamSenderTest
    {
        [Test]
        public void CreateAndDestroyStreamSender()
        {
            var obj = new GameObject("test");
            var streamer = obj.AddComponent<StreamSender>();

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Empty);

            UnityEngine.Object.DestroyImmediate(streamer);
            UnityEngine.Object.DestroyImmediate(obj);
        }
    }

    class DataChannelTest
    {
        [Test]
        public void CreateAndDestroyDataChannel()
        {
            var obj = new GameObject("test");
            var channel = obj.AddComponent<DataChannel>();
            channel.SetLabel("test");
            channel.SetLocal(true);

            Assert.That(channel.IsLocal, Is.True);
            Assert.That(channel.Label, Is.EqualTo("test"));

            UnityEngine.Object.DestroyImmediate(channel);
            UnityEngine.Object.DestroyImmediate(obj);
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
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer})]
        public void AddComponent()
        {
            var container = TestContainer<BroadcastBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<StreamSender>();
            var channel = container.test.gameObject.AddComponent<DataChannel>();

            container.handler.AddComponent(streamer);
            container.handler.AddComponent(channel);
            container.Dispose();
        }

        private IEnumerator WaitUntilConnect(
            SingleConnection handler1, Broadcast handler2, string connectionId)
        {
            handler1.CreateConnection(connectionId);
            yield return new WaitUntil(() => handler1.IsConnected(connectionId));
            yield return new WaitUntil(() => handler2.IsConnected(connectionId));
        }

        private IEnumerator WaitUntilDisconnect(
            SingleConnection handler1, Broadcast handler2, string connectionId)
        {
            handler1.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !handler1.IsConnected(connectionId));
            yield return new WaitUntil(() => !handler2.IsConnected(connectionId));
        }

        [UnityTest, Timeout(3000)]
        public IEnumerator DataChannel()
        {
            string connectionId = "12345";
            var container1 = TestContainer<BroadcastBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var channel1 = container1.test.gameObject.AddComponent<DataChannel>();
            channel1.SetLocal(true);
            var channel2 = container2.test.gameObject.AddComponent<DataChannel>();
            channel2.SetLocal(false);
            container1.handler.AddComponent(channel1);
            container2.handler.AddComponent(channel2);

            yield return WaitUntilConnect(container2.handler, container1.handler, connectionId);
            yield return WaitUntilDisconnect(container2.handler, container1.handler, connectionId);

            container1.Dispose();
            container2.Dispose();
        }


        //todo:: crash in dispose process on standalone linux
        [UnityTest, Timeout(3000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer})]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<BroadcastBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var sender = container1.test.gameObject.AddComponent<StreamSender>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            sender.OnStartedStream += _ => isStartedStream1 = true;
            sender.OnStoppedStream += _ => isStoppedStream1 = true;
            container1.handler.AddComponent(sender);

            var receiver = container2.test.gameObject.AddComponent<StreamReceiver>();
            bool isStartedStream2 = false;
            bool isStoppedStream2 = false;

            receiver.OnStartedStream += _ => isStartedStream2 = true;
            receiver.OnStoppedStream += _ => isStoppedStream2 = true;
            container2.handler.AddComponent(receiver);

            yield return WaitUntilConnect(
                container2.handler, container1.handler, connectionId);

            yield return new WaitUntil(() => isStartedStream2 && isStartedStream1);
            Assert.That(isStartedStream1, Is.True);
            Assert.That(isStartedStream2, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Receiver, Is.Not.Null);

            yield return WaitUntilDisconnect(
                container2.handler, container1.handler, connectionId);

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

        //todo:: crash in dispose process on standalone linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer})]
        public IEnumerator AddStreamSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var streamer = container.test.gameObject.AddComponent<StreamSender>();

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Empty);

            container.handler.AddComponent(streamer);
            container.handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.handler.ExistConnection(connectionId));

            Assert.That(streamer.Track, Is.Not.Null);
            Assert.That(streamer.Senders, Is.Not.Empty);

            container.handler.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.handler.ExistConnection(connectionId));
            container.Dispose();
        }

        [UnityTest, Timeout(10000)]
        public IEnumerator AddDataChannel()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var handler = container.handler;
            var channel = container.test.gameObject.AddComponent<DataChannel>();
            channel.SetLocal(true);
            channel.SetLabel("test");

            handler.AddComponent(channel);
            handler.CreateConnection(connectionId);

            yield return new WaitUntil(() => container.handler.ExistConnection(connectionId));

            Assert.That(channel.Channel, Is.Not.Null);
            Assert.That(channel.Channel.Label, Is.EqualTo("test"));

            container.handler.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.handler.ExistConnection(connectionId));
            container.Dispose();
        }

        [UnityTest, Timeout(10000)]
        public IEnumerator AddSource()
        {
            string connectionId = "12345";
            var container = TestContainer<SingleConnectionBehaviourTest>.Create("test");
            var channel = container.test.gameObject.AddComponent<DataChannel>();

            channel.SetLocal(true);
            channel.SetLabel("test");

            Assert.That(channel.Channel, Is.Null);
            Assert.That(channel.IsLocal, Is.True);
            Assert.That(channel.Label, Is.EqualTo("test"));

            container.handler.AddComponent(channel);
            container.handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => container.handler.ExistConnection(connectionId));

            Assert.That(channel.Channel, Is.Not.Null);
            Assert.That(channel.Channel.Label, Is.EqualTo("test"));

            container.handler.DeleteConnection(connectionId);
            yield return new WaitUntil(() => !container.handler.ExistConnection(connectionId));
            container.Dispose();
        }


        //todo:: crash in dispose process on standalone linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxPlayer})]
        public IEnumerator ReceiveStream()
        {
            string connectionId = "12345";
            var container1 = TestContainer<SingleConnectionBehaviourTest>.Create("test1");
            var container2 = TestContainer<SingleConnectionBehaviourTest>.Create("test2");

            var streamer = container1.test.gameObject.AddComponent<StreamSender>();
            bool isStartedStream0 = false;
            bool isStoppedStream0 = false;
            streamer.OnStartedStream += _ => isStartedStream0 = true;
            streamer.OnStoppedStream += _ => isStoppedStream0 = true;

            container1.handler.AddComponent(streamer);
            container1.handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.handler.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream0);
            Assert.That(isStartedStream0, Is.True);

            var receiver = container2.test.gameObject.AddComponent<StreamReceiver>();
            bool isStartedStream1 = false;
            bool isStoppedStream1 = false;
            receiver.OnStartedStream += _ => isStartedStream1 = true;
            receiver.OnStoppedStream += _ => isStoppedStream1 = true;

            Assert.That(receiver.Track, Is.Null);
            Assert.That(receiver.Receiver, Is.Null);

            container2.handler.AddComponent(receiver);
            container2.handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.handler.ExistConnection(connectionId));

            yield return new WaitUntil(() => isStartedStream1);
            Assert.That(isStartedStream1, Is.True);

            Assert.That(receiver.Track, Is.Not.Null);
            Assert.That(receiver.Receiver, Is.Not.Null);

            container1.handler.DeleteConnection(connectionId);
            container2.handler.DeleteConnection(connectionId);

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

            var channel1 = container1.test.gameObject.AddComponent<DataChannel>();
            bool isStartedChannel1 = false;
            bool isStoppedChannel1 = false;

            channel1.OnStartedChannel += _ => isStartedChannel1 = true;
            channel1.OnStoppedChannel += _ => isStoppedChannel1 = true;

            container1.handler.AddComponent(channel1);
            container1.handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => container1.handler.ExistConnection(connectionId));

            var channel2 = container2.test.gameObject.AddComponent<DataChannel>();
            bool isStartedChannel2 = false;
            bool isStoppedChannel2 = false;
            channel2.OnStartedChannel += _ => isStartedChannel2 = true;
            channel2.OnStoppedChannel += _ => isStoppedChannel2 = true;

            channel2.SetLocal(true);
            channel2.SetLabel("test");

            Assert.That(channel2.Channel, Is.Null);
            Assert.That(channel2.IsLocal, Is.True);
            Assert.That(channel2.Label, Is.EqualTo("test"));

            container2.handler.AddComponent(channel2);
            container2.handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => container2.handler.ExistConnection(connectionId));
            yield return new WaitUntil(() => isStartedChannel1 && isStartedChannel2);
            Assert.That(isStartedChannel1, Is.True);
            Assert.That(isStartedChannel2, Is.True);

            Assert.That(channel1.Channel, Is.Not.Null);
            Assert.That(channel1.IsLocal, Is.False);
            Assert.That(channel1.Label, Is.EqualTo("test"));

            container1.handler.DeleteConnection(connectionId);
            container2.handler.DeleteConnection(connectionId);

            yield return new WaitUntil(() => isStoppedChannel1 && isStoppedChannel2);
            Assert.That(isStoppedChannel1, Is.True);
            Assert.That(isStoppedChannel2, Is.True);

            container1.Dispose();
            container2.Dispose();
        }
    }
}
