using System.Linq;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;

namespace Unity.RenderStreaming.RuntimeTest
{

    class InputPositionCorrectorTest
    {
        void OnEvent(InputEventPtr ptr, InputDevice device)
        {
        }

        [Test]
        public void Invoke()
        {
            System.Action<InputEventPtr, InputDevice> onEvent = OnEvent;
            var corrector = new InputPositionCorrector(onEvent);
            Assert.That(corrector.inputRegion, Is.EqualTo(Rect.zero));
            Assert.That(corrector.outputRegion, Is.EqualTo(Rect.zero));

            var device = UnityEngine.InputSystem.InputSystem.devices.First(_ => _ is Pointer);
            var inputEvent = new InputEvent();
            unsafe
            {
                var ptr = InputEventPtr.From(&inputEvent);
                corrector.Invoke(ptr, device);
            }
        }
    }
}
