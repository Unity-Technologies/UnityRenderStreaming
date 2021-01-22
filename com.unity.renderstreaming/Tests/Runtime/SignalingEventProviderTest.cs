using UnityEngine;
using Moq;
using NUnit.Framework;
using Unity.WebRTC;
using UnityEngine.EventSystems;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    public class CreatedConnectionHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, ICreatedConnectionHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }

        public void OnCreatedConnection(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class FoundConnectionHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IFoundConnectionHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }

        public void OnFoundConnection(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class DeletedConnectionHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IDeletedConnectionHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnDeletedConnection(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class ConnectHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IConnectHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnConnect(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class DisconnectHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IDisconnectHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnDisconnect(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class OfferHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IOfferHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnOffer(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class AnswerHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IAnswerHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnAnswer(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class AddReceiverHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IAddReceiverHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnAddReceiver(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class AddChannelHandlerTest : MonoBehaviour,
        IMonoBehaviourTest, IAddChannelHandler
    {
        public bool IsTestFinished { get; private set; }
        public SignalingEventData Data { get; private set; }
        public void OnAddChannel(SignalingEventData data)
        {
            IsTestFinished = true;
            this.Data = data;
        }
    }

    public class SignalingEventProviderTest
    {
        private EventSystem m_eventSystem;
        private SignalingEventProvider m_provider;
        private Mock<IRenderStreamingHandler> m_handler;

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            WebRTC.WebRTC.Initialize(EncoderType.Software);
            m_eventSystem = new GameObject("EventSystem").AddComponent<EventSystem>();
        }

        [OneTimeTearDown]
        public void OneTimeTearDown()
        {
            WebRTC.WebRTC.Dispose();
            UnityEngine.Object.DestroyImmediate(m_eventSystem);
        }

        [SetUp]
        public void SetUp()
        {
            m_handler = new Mock<IRenderStreamingHandler>();
            m_provider = new SignalingEventProvider(m_handler.Object);
        }

        [Test]
        public void OnCreatedConnection()
        {
            var connectionId = "12345";
            var test = new MonoBehaviourTest<CreatedConnectionHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onCreatedConnection += null, connectionId);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }

        [Test]
        public void OnFoundConnection()
        {
            var connectionId = "12345";
            var test = new MonoBehaviourTest<FoundConnectionHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onFoundConnection += null, connectionId);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }

        [Test]
        public void OnDeletedConnection()
        {
            var connectionId = "12345";
            var test = new MonoBehaviourTest<DeletedConnectionHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onDeletedConnection += null, connectionId);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }
        [Test]
        public void OnConnect()
        {
            var connectionId = "12345";
            var test = new MonoBehaviourTest<ConnectHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onConnect += null, connectionId);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }
        [Test]
        public void OnDisconnect()
        {
            var connectionId = "12345";
            var test = new MonoBehaviourTest<DisconnectHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onDisconnect += null, connectionId);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }

        [Test]
        public void OnOffer()
        {
            var connectionId = "12345";
            var sdp = "this is sdp";
            var test = new MonoBehaviourTest<OfferHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onGotOffer += null, connectionId, sdp);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            Assert.That(test.component.Data.sdp, Is.EqualTo(sdp));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }

        [Test]
        public void OnAnswer()
        {
            var connectionId = "12345";
            var sdp = "this is sdp";
            var test = new MonoBehaviourTest<AnswerHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onGotAnswer += null, connectionId, sdp);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            Assert.That(test.component.Data.sdp, Is.EqualTo(sdp));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }

        [Test]
        public void OnAddReceiver()
        {
            var connectionId = "12345";
            // todo:: create a receiver instance
            var test = new MonoBehaviourTest<AddReceiverHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onAddReceiver += null, connectionId, null);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            Assert.That(test.component.Data.receiver, Is.Null);
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }

        [Test]
        public void OnAddChannel()
        {
            var connectionId = "12345";
            var peer = new RTCPeerConnection();
            var channel = peer.CreateDataChannel("test");
            var test = new MonoBehaviourTest<AddChannelHandlerTest>();
            m_provider.Subscribe(test.component);
            m_handler.Raise(x => x.onAddChannel += null, connectionId, channel);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            Assert.That(test.component.Data.channel, Is.EqualTo(channel));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }
    }
}
