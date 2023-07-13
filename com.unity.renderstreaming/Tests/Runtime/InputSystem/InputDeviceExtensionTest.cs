using NUnit.Framework;
using Unity.RenderStreaming.InputSystem;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using Assert = NUnit.Framework.Assert;

namespace Unity.RenderStreaming.RuntimeTest
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

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
}
