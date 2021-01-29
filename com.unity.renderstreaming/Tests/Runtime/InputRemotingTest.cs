using System.Collections;
using NUnit.Framework;
using UnityEngine.InputSystem;
using System.Linq;
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
                participantId = 1, type = InputRemoting.MessageType.NewEvents, data = new byte[] {1, 2, 3, 4, 5},
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
            var device = InputSystem.devices.First();
            InputDeviceDescription empty;
            InputDeviceDescription origin = device.description;
            Assert.That(device.description, Is.Not.EqualTo(empty));
            device.SetDescription(empty);
            Assert.That(device.description, Is.EqualTo(empty));
            device.SetDescription(origin);
            Assert.That(device.description, Is.EqualTo(origin));
        }
        [Test]
        public void SetParticipantId()
        {
            var device = InputSystem.devices.First();
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
            var device = InputSystem.devices.First();
            int deviceFlags = 0;
            int origin = device.GetDeviceFlags();
            device.SetDeviceFlags(deviceFlags);
            Assert.That(device.GetDeviceFlags(), Is.EqualTo(deviceFlags));
            device.SetDeviceFlags(origin);
            Assert.That(device.GetDeviceFlags(), Is.EqualTo(origin));
        }
    }

    class InputRemotingTest
    {
        class MyMonoBehaviourTest : MonoBehaviour, IMonoBehaviourTest
        {
            public bool IsTestFinished
            {
                get { return true; }
            }
        }

        private MonoBehaviourTest<MyMonoBehaviourTest> test;
        private RenderStreamingInternal target1, target2;
        private RTCDataChannel channel1, channel2;
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
                startCoroutine = test.component.StartCoroutine
            };
        }

        [UnitySetUp]
        public IEnumerator UnitySetUp()
        {
            MockSignaling.Reset(true);
            test = new MonoBehaviourTest<MyMonoBehaviourTest>();

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            target1 = new RenderStreamingInternal(ref dependencies1);
            target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            target1.onStart += () => { isStarted1 = true; };
            target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);

            bool isCreatedConnection1 = false;
            bool isCreatedConnection2 = false;
            target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            target2.onFoundConnection += _ => { isCreatedConnection2 = true; };

            // target1 is Receiver in private mode
            target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);

            // target2 is sender in private mode
            target2.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection2);

            bool isAddChannel1 = false;
            bool isGotAnswer2 = false;
            target1.onAddChannel += (_, channel) => { isAddChannel1 = true; channel1 = channel; };
            target1.onGotOffer += (_, sdp) => { target1.SendAnswer(connectionId); };
            target2.onGotAnswer += (_, sdp) => { isGotAnswer2 = true; };

            // send offer automatically after creating channel
            channel2 = target2.CreateChannel(connectionId, "test");

            // send offer manually 
            target2.SendOffer(connectionId);

            yield return new WaitUntil(() => isAddChannel1 && isGotAnswer2);
        }

        [UnityTearDown]
        public IEnumerator UnityTearDown()
        {
            target1.DeleteConnection(connectionId);
            target2.DeleteConnection(connectionId);

            bool isDeletedConnection1 = false;
            bool isDeletedConnection2 = false;
            target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
            target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
            yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);

            channel1.Dispose();
            channel2.Dispose();

            test.component.StopAllCoroutines();
            UnityEngine.Object.Destroy(test.gameObject);
        }

        [Test]
        public void Sender()
        {
            var sender = new Sender();
            var senderInput = new InputRemoting(sender);
            var senderDisposer = senderInput.Subscribe(new Observer(channel1));
            senderInput.StartSending();
            senderInput.StopSending();
            senderDisposer.Dispose();
        }

        [Test]
        public void Receiver()
        {
            var receiver = new Receiver(channel1);
            var receiverInput = new InputRemoting(receiver);
            var receiverDisposer = receiverInput.Subscribe(receiverInput);
            receiverInput.StartSending();
            receiverInput.StopSending();
            receiverDisposer.Dispose();
        }

    }
}
