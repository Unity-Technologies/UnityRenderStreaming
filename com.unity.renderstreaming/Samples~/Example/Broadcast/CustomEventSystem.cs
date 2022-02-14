using UnityEngine.EventSystems;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;

namespace Unity.RenderStreaming.Samples
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    class CustomEventSystem : EventSystem
    {
#if !INPUTSYSTEM_1_1_OR_NEWER
        protected override void Awake()
        {
            base.Awake();
            unsafe
            {
                InputSystem.onDeviceCommand += InputSystemOnDeviceCommand;
            }
        }

        private static unsafe long? InputSystemOnDeviceCommand(InputDevice device, InputDeviceCommand* command)
        {
            if (command->type != QueryCanRunInBackground.Type)
            {
                // return null is skip this evaluation
                return null;
            }

            ((QueryCanRunInBackground*)command)->canRunInBackground = true;
            return InputDeviceCommand.GenericSuccess;
        }

        protected override void OnApplicationFocus(bool hasFocus)
        {
            //Do not change focus flag on eventsystem
        }
#endif
    }

}
