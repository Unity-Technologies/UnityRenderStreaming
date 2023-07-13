using System;
using NUnit.Framework;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    class CreatedConnectionHandlerTest : MonoBehaviour,
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

    class DeletedConnectionHandlerTest : MonoBehaviour,
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

    class ConnectHandlerTest : MonoBehaviour,
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

    class DisconnectHandlerTest : MonoBehaviour,
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

    class OfferHandlerTest : MonoBehaviour,
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

    class AnswerHandlerTest : MonoBehaviour,
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

    class AddReceiverHandlerTest : MonoBehaviour,
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

    class AddChannelHandlerTest : MonoBehaviour,
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

    /// <summary>
    /// note:: Moq is not supported IL2CPP platform,
    /// this class should be replaced Moq `Raise` method.
    /// </summary>
    class MockDelegate : IRenderStreamingDelegate
    {
        public event Action onStart;
        public event Action<string> onCreatedConnection;
        public event Action<string> onDeletedConnection;
        public event Action<string, string> onGotOffer;
        public event Action<string, string> onGotAnswer;
        public event Action<string> onConnect;
        public event Action<string> onDisconnect;
        public event Action<string, RTCRtpTransceiver> onAddTransceiver;
        public event Action<string, RTCDataChannel> onAddChannel;

        public void RaiseOnStart()
        {
            onStart?.Invoke();
        }

        public void RaiseOnCreatedConnection(string connectionId)
        {
            onCreatedConnection?.Invoke(connectionId);
        }
        public void RaiseOnDeletedConnection(string connectionId)
        {
            onDeletedConnection?.Invoke(connectionId);
        }
        public void RaiseOnGotOffer(string connectionId, string sdp)
        {
            onGotOffer?.Invoke(connectionId, sdp);
        }
        public void RaiseOnGotAnswer(string connectionId, string sdp)
        {
            onGotAnswer?.Invoke(connectionId, sdp);
        }
        public void RaiseOnConnect(string connectionId)
        {
            onConnect?.Invoke(connectionId);
        }
        public void RaiseOnDisconnect(string connectionId)
        {
            onDisconnect?.Invoke(connectionId);
        }
        public void RaiseOnAddTransceiver(string connectionId, RTCRtpTransceiver transceiver)
        {
            onAddTransceiver?.Invoke(connectionId, transceiver);
        }
        public void RaiseOnAddChannel(string connectionId, RTCDataChannel channel)
        {
            onAddChannel?.Invoke(connectionId, channel);
        }
    }

    [UnityPlatform(exclude = new[] { RuntimePlatform.OSXEditor, RuntimePlatform.OSXPlayer, RuntimePlatform.LinuxEditor, RuntimePlatform.LinuxPlayer })]
    class SignalingEventProviderTest
    {
        private EventSystem m_eventSystem;
        private SignalingEventProvider m_provider;
        private MockDelegate _mDelegate;

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            m_eventSystem = new GameObject("EventSystem").AddComponent<EventSystem>();
        }

        [OneTimeTearDown]
        public void OneTimeTearDown()
        {
            UnityEngine.Object.DestroyImmediate(m_eventSystem);
        }

        [SetUp]
        public void SetUp()
        {
            _mDelegate = new MockDelegate();
            m_provider = new SignalingEventProvider(_mDelegate);
        }

        [Test]
        public void Subscribe()
        {
            var test = new MonoBehaviourTest<CreatedConnectionHandlerTest>();
            Assert.That(m_provider.Subscribe(test.component), Is.True);

            // return false if it is already registered.
            Assert.That(m_provider.Subscribe(test.component), Is.False);

            Assert.That(m_provider.Unsubscribe(test.component), Is.True);

            // return false if it is not found.
            Assert.That(m_provider.Unsubscribe(test.component), Is.False);
        }

        [Test]
        public void OnCreatedConnection()
        {
            var connectionId = "12345";
            var test = new MonoBehaviourTest<CreatedConnectionHandlerTest>();
            m_provider.Subscribe(test.component);
            _mDelegate.RaiseOnCreatedConnection(connectionId);
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
            _mDelegate.RaiseOnDeletedConnection(connectionId);
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
            _mDelegate.RaiseOnConnect(connectionId);
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
            _mDelegate.RaiseOnDisconnect(connectionId);
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
            _mDelegate.RaiseOnGotOffer(connectionId, sdp);
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
            _mDelegate.RaiseOnGotAnswer(connectionId, sdp);
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
            _mDelegate.RaiseOnAddTransceiver(connectionId, null);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            Assert.That(test.component.Data.transceiver, Is.Null);
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
            _mDelegate.RaiseOnAddChannel(connectionId, channel);
            Assert.That(test.component.IsTestFinished, Is.True);
            Assert.That(test.component.Data.connectionId, Is.EqualTo(connectionId));
            Assert.That(test.component.Data.channel, Is.EqualTo(channel));
            UnityEngine.Object.DestroyImmediate(test.gameObject);
        }
    }
}
