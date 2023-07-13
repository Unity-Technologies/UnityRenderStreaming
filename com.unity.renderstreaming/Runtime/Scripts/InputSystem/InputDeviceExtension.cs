using System;
using System.Reflection;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;

namespace Unity.RenderStreaming.InputSystem
{
    // todo(kazuki)::Avoid to use reflection
    static class InputDeviceExtension
    {
        private static Type typeInputDevice;
        private static FieldInfo fieldInfoParticipantId;
        private static FieldInfo fieldInfoDescription;
        private static FieldInfo fieldInfoDeviceFlags;

        static InputDeviceExtension()
        {
            typeInputDevice = typeof(InputDevice);
            fieldInfoParticipantId = typeInputDevice.GetField("m_ParticipantId",
                BindingFlags.NonPublic | BindingFlags.Instance);
            fieldInfoDescription = typeInputDevice.GetField("m_Description",
                BindingFlags.NonPublic | BindingFlags.Instance);
            fieldInfoDeviceFlags = typeInputDevice.GetField("m_DeviceFlags",
                BindingFlags.NonPublic | BindingFlags.Instance);
        }

        public static void SetParticipantId(this InputDevice device, int value)
        {
            fieldInfoParticipantId.SetValue(device, value);
        }

        public static int GetParticipantId(this InputDevice device)
        {
            return (int)fieldInfoParticipantId.GetValue(device);
        }

        public static void SetDescription(this InputDevice device, InputDeviceDescription value)
        {
            fieldInfoDescription.SetValue(device, value);
        }

        public static void SetDeviceFlags(this InputDevice device, int value)
        {
            fieldInfoDeviceFlags.SetValue(device, value);
        }
        public static int GetDeviceFlags(this InputDevice device)
        {
            return (int)fieldInfoDeviceFlags.GetValue(device);
        }
    }
}
// #endif
