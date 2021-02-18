using System.Collections;
using NUnit.Framework;
using UnityEngine.InputSystem;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.TestTools;
using Assert = NUnit.Framework.Assert;

namespace Unity.RenderStreaming.RuntimeTest
{
    class MessageSerializerTest
    {
        [Test]
        public void Serialize()
        {
            InputRemoting.Message message1 = new InputRemoting.Message
            {
                participantId = 1,
                type = InputRemoting.MessageType.NewEvents,
                data = new byte[] {1, 2, 3, 4, 5},
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

    class InputDeviceExtensionTest
    {
        [Test]
        public void SetDescription()
        {
            var device = InputSystem.AddDevice<Mouse>();
            InputDeviceDescription empty;
            InputDeviceDescription origin = device.description;
            device.SetDescription(empty);
            Assert.That(device.description, Is.EqualTo(empty));
            device.SetDescription(origin);
            Assert.That(device.description, Is.EqualTo(origin));
        }
        [Test]
        public void SetParticipantId()
        {
            var device = InputSystem.AddDevice<Mouse>();
            int participantId = 0;
            int origin = device.GetParticipantId();
            device.SetParticipantId(participantId);
            Assert.That(device.GetParticipantId(), Is.EqualTo(participantId));
            device.SetParticipantId(origin);
            Assert.That(device.GetParticipantId(), Is.EqualTo(origin));
        }
        [Test]
        public void SetDeviceFlags()
        {
            var device = InputSystem.AddDevice<Mouse>();
            int deviceFlags = 0;
            int origin = device.GetDeviceFlags();
            device.SetDeviceFlags(deviceFlags);
            Assert.That(device.GetDeviceFlags(), Is.EqualTo(deviceFlags));
            device.SetDeviceFlags(origin);
            Assert.That(device.GetDeviceFlags(), Is.EqualTo(origin));
        }
    }

    /// todo(kazuki):workaround
    [Ignore("TODO::This test-case is failed when there is no input devices.")]
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
        private RenderStreamingInternal _target1, _target2;
        private RTCDataChannel _channel1, _channel2;
        private string connectionId = "12345";

        private RenderStreamingDependencies CreateDependencies()
        {
            return new RenderStreamingDependencies
            {
                signaling = new MockSignaling(),
                config = new RTCConfiguration
                {
                    iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
                },
                encoderType = EncoderType.Software,
                startCoroutine = _test.component.StartCoroutine
            };
        }

        [UnitySetUp]
        public IEnumerator UnitySetUp()
        {
            MockSignaling.Reset(true);
            _test = new MonoBehaviourTest<MyMonoBehaviourTest>();

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            _target1 = new RenderStreamingInternal(ref dependencies1);
            _target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            _target1.onStart += () => { isStarted1 = true; };
            _target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);

            bool isCreatedConnection1 = false;
            bool isCreatedConnection2 = false;
            _target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            _target2.onFoundConnection += _ => { isCreatedConnection2 = true; };

            // _target1 is Receiver in private mode
            _target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);

            // _target2 is sender in private mode
            _target2.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection2);

            _target1.onAddChannel += (_, channel) => { _channel1 = channel; };
            _target1.onGotOffer += (_, sdp) => { _target1.SendAnswer(connectionId); };

            // send offer automatically after creating channel
            _channel2 = _target2.CreateChannel(connectionId, "_test");

            // send offer manually 
            _target2.SendOffer(connectionId);

            yield return new WaitUntil(() => _target1.IsConnected(connectionId));
            yield return new WaitUntil(() => _target2.IsConnected(connectionId));
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

            _test.component.StopAllCoroutines();
            _target1.Dispose();
            _target2.Dispose();
            UnityEngine.Object.DestroyImmediate(_test.gameObject);
        }

        [Test]
        public void Sender()
        {
            var sender = new Sender();
            var senderInput = new InputRemoting(sender);
            var senderDisposer = senderInput.Subscribe(new Observer(_channel1));
            senderInput.StartSending();
            senderInput.StopSending();
            senderDisposer.Dispose();
        }

        [Test]
        public void Receiver()
        {
            var receiver = new Receiver(_channel1);
            var receiverInput = new InputRemoting(receiver);
            var receiverDisposer = receiverInput.Subscribe(receiverInput);
            receiverInput.StartSending();
            receiverInput.StopSending();
            receiverDisposer.Dispose();
        }

        [UnityTest, Timeout(3000)]
        public IEnumerator AddDevice()
        {
            var sender = new Sender();
            var senderInput = new InputRemoting(sender);
            var senderDisposer = senderInput.Subscribe(new Observer(_channel1));

            var receiver = new Receiver(_channel2);
            var receiverInput = new InputRemoting(receiver);
            var receiverDisposer = receiverInput.Subscribe(receiverInput);

            InputDevice device = null;
            InputDeviceChange change = default;
            receiver.onDeviceChange += (_device, _change) => {
                device = _device;
                change = _change;
            };

            receiverInput.StartSending();
            senderInput.StartSending();

            yield return new WaitUntil(() => device != null);

            Assert.That(device, Is.Not.Null);
            Assert.That(change, Is.EqualTo(InputDeviceChange.Added));
            Assert.That(receiver.layouts, Is.Empty);
            Assert.That(receiver.remoteDevices, Is.Not.Empty);
            Assert.That(receiver.remoteDevices, Has.All.Matches<InputDevice>(d => d.remote));

            senderInput.StopSending();
            receiverInput.StopSending();

            receiver.RemoveAllDevices();

            Assert.That(receiver.layouts, Is.Empty);
            Assert.That(receiver.remoteDevices, Is.Empty);

            senderDisposer.Dispose();
            receiverDisposer.Dispose();
        }
    }
}
