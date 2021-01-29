using NUnit.Framework;
using Unity.WebRTC;
using UnityEngine.InputSystem;
using System.Linq;
using UnityEngine.InputSystem.Layouts;
using Assert = NUnit.Framework.Assert;

namespace Unity.RenderStreaming.RuntimeTest
{
    class InputRemotingMessageTest
    {
        [Test]
        public void Serialize()
        {
            InputRemoting.Message message1 = new InputRemoting.Message
            {
                participantId = 1, type = InputRemoting.MessageType.NewEvents, data = new byte[] {1, 2, 3, 4, 5},
            };
            var bytes = message1.Serialize();

            Assert.That(bytes, Is.Not.Null);
            Assert.That(bytes, Has.Length.GreaterThan(0));

            InputRemoting.Message.Deserialize(bytes, out var message2);
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
}
