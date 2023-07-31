using System.Collections;
using NUnit.Framework;
using Unity.RenderStreaming.InputSystem;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.TestTools;
using Assert = NUnit.Framework.Assert;

namespace Unity.RenderStreaming.RuntimeTest
{
    using InputRemoting = InputSystem.InputRemoting;

    class MessageSerializerTest
    {
        [Test]
        public void Serialize()
        {
            InputRemoting.Message message1 = new InputRemoting.Message
            {
                participantId = 1,
                type = InputRemoting.MessageType.NewEvents,
                data = new byte[] { 1, 2, 3, 4, 5 },
            };

            var bytes = MessageSerializer.Serialize(ref message1);

            Assert.That(bytes, Is.Not.Null);
            Assert.That(bytes, Has.Length.GreaterThan(0));

            MessageSerializer.Deserialize(bytes, out var message2);
            Assert.That(message2.participantId, Is.EqualTo(message1.participantId));
            Assert.That(message2.type, Is.EqualTo(message1.type));
            Assert.That(message2.data, Is.EqualTo(message1.data));
        }
    }

    /// todo(kazuki):workaround
    class InputRemotingTest
    {
        class MyMonoBehaviourTest : MonoBehaviour, IMonoBehaviourTest
        {
            public bool IsTestFinished
            {
                get { return true; }
            }
        }

        private MonoBehaviourTest<MyMonoBehaviourTest> _test;
        private SignalingManagerInternal _target1, _target2;
        private RTCDataChannel _channel1, _channel2;
        private string connectionId = "12345";
        const float ResendOfferInterval = 3f;

        private RenderStreamingDependencies CreateDependencies()
        {
            return new RenderStreamingDependencies
            {
                signaling = new MockSignaling(),
                config = new RTCConfiguration
                {
                    iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
                },
                startCoroutine = _test.component.StartCoroutine,
                stopCoroutine = _test.component.StopCoroutine,
                resentOfferInterval = ResendOfferInterval,
            };
        }

        [UnitySetUp]
        public IEnumerator UnitySetUp()
        {
            MockSignaling.Reset(true);
            _test = new MonoBehaviourTest<MyMonoBehaviourTest>();

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            _target1 = new SignalingManagerInternal(ref dependencies1);
            _target2 = new SignalingManagerInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            _target1.onStart += () => { isStarted1 = true; };
            _target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);

            bool isCreatedConnection1 = false;
            bool isCreatedConnection2 = false;
            _target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            _target2.onCreatedConnection += _ => { isCreatedConnection2 = true; };

            // _target1 is Receiver in private mode
            _target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);

            // _target2 is sender in private mode
            _target2.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection2);

            _target1.onAddChannel += (_, channel) => { _channel1 = channel; };

            // send offer automatically after creating channel
            _channel2 = _target2.CreateChannel(connectionId, "_test");

            bool isGotOffer1 = false;
            bool isGotAnswer2 = false;
            _target1.onGotOffer += (_, sdp) => { isGotOffer1 = true; };
            _target2.onGotAnswer += (_, sdp) => { isGotAnswer2 = true; };

            // each peer are not stable, signaling process not complete.
            yield return new WaitUntil(() => isGotOffer1);

            _target1.SendAnswer(connectionId);
            yield return new WaitUntil(() => isGotAnswer2);
            Assert.That(isGotAnswer2, Is.True);

            // If target1 processes resent　Offer from target2, target1 is not stable.
            Assert.That(_target2.IsStable(connectionId), Is.True);

            yield return new WaitUntil(() => _channel1 != null);
            Assert.That(_channel1.ReadyState, Is.EqualTo(RTCDataChannelState.Open));
        }

        [UnityTearDown]
        public IEnumerator UnityTearDown()
        {
            _target1.DeleteConnection(connectionId);
            _target2.DeleteConnection(connectionId);

            bool isDeletedConnection1 = false;
            bool isDeletedConnection2 = false;
            _target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
            _target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
            yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);

            _channel1.Dispose();
            _channel2.Dispose();
            _channel1 = null;
            _channel2 = null;
            _test.component.StopAllCoroutines();
            _target1.Dispose();
            _target2.Dispose();
            UnityEngine.Object.DestroyImmediate(_test.gameObject);
        }

        [Test]
        public void DoNothing()
        {
        }


        [Test]
        public void Sender()
        {
            var sender = new Sender();
            Assert.That(sender.layouts, Is.Not.Empty);
            Assert.That(sender.devices, Is.Not.Empty);
            Assert.That(sender.GetDeviceById(0), Is.Null);
            var senderInput = new InputRemoting(sender);
            var senderDisposer = senderInput.Subscribe(new Observer(_channel1));
            senderInput.StartSending();
            senderInput.StopSending();
            senderDisposer.Dispose();
            sender.Dispose();
        }

        [Test]
        public void Receiver()
        {
            var receiver = new Receiver(_channel1);
            Assert.That(receiver.remoteDevices, Is.Empty);
            Assert.That(receiver.remoteLayouts, Is.Empty);
            Assert.That(receiver.GetDeviceById(0), Is.Null);
            var receiverInput = new InputRemoting(receiver);
            var receiverDisposer = receiverInput.Subscribe(receiverInput);
            receiverInput.StartSending();
            receiverInput.StopSending();
            receiverDisposer.Dispose();
        }

        /// todo(kazuki): This test is failed for timeout on macOS.
        /// todo(kazuki): This test is failed for timeout on iPhonePlayer.
        [UnityTest, Timeout(3000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.OSXPlayer, RuntimePlatform.IPhonePlayer })]
        public IEnumerator AddDevice()
        {
            var sender = new Sender();
            var senderInput = new InputRemoting(sender);
            var senderSubscriberDisposer = senderInput.Subscribe(new Observer(_channel1));

            var receiver = new Receiver(_channel2);
            var receiverInput = new InputRemoting(receiver);
            var receiverSubscriberDisposer = receiverInput.Subscribe(receiverInput);

            InputDevice device = null;
            InputDeviceChange change = default;
            receiver.onDeviceChange += (_device, _change) =>
            {
                device = _device;
                change = _change;
            };
            string layoutName = null;
            InputControlLayoutChange layoutChange = default;
            receiver.onLayoutChange += (_name, _change) =>
            {
                layoutName = _name;
                layoutChange = _change;
            };

            receiverInput.StartSending();
            senderInput.StartSending();

            yield return new WaitUntil(() => device != null);
            yield return new WaitUntil(() => layoutName != null);

            Assert.That(device, Is.Not.Null);
            Assert.That(change, Is.EqualTo(InputDeviceChange.Added));
            Assert.That(layoutName, Is.Not.Null);
            Assert.That(layoutChange, Is.EqualTo(InputControlLayoutChange.Added));

            Assert.That(receiver.remoteLayouts, Is.Not.Empty);
            Assert.That(receiver.remoteDevices, Is.Not.Empty);
            Assert.That(receiver.remoteDevices, Has.All.Matches<InputDevice>(d => d.remote));

            senderInput.StopSending();
            receiverInput.StopSending();

            senderSubscriberDisposer.Dispose();
            receiverSubscriberDisposer.Dispose();
            sender.Dispose();
            receiver.Dispose();
        }

        [Test]
        public void RemoveRemoteDevices()
        {
            var sender = new Sender();
            var senderInput = new InputRemoting(sender);
            senderInput.RemoveRemoteDevices(0);
            sender.Dispose();
        }

        [Test]
        public void SaveState()
        {
            var sender = new Sender();
            var inputRemoting = new InputRemoting(sender);
            var state = inputRemoting.SaveState();
            inputRemoting.RestoreState(state, sender);
            sender.Dispose();
        }
    }
}
