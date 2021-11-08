using System;
using System.IO;
using System.Reflection;

namespace Unity.RenderStreaming.InputSystem
{
#if UNITY_EDITOR && !INPUTSYSTEM_1_1_OR_NEWER
    // todo(kazuki)::Avoid to use reflection
    static class InputEditorUserSettings
    {
        private static Type type;
        private static PropertyInfo propertyLockInputToGameView;
        private static MethodInfo methodLoad;
        private static FieldInfo fieldFilePath;

        static InputEditorUserSettings()
        {
            type = Type.GetType("UnityEngine.InputSystem.Editor.InputEditorUserSettings, Unity.InputSystem");
            propertyLockInputToGameView = type.GetProperty("lockInputToGameView");
            methodLoad = type.GetMethod("Load",
                BindingFlags.NonPublic | BindingFlags.Static);
            fieldFilePath = type.GetField("kSavePath",
                BindingFlags.NonPublic | BindingFlags.Static);
        }

        public static bool lockInputToGameView
        {
            get { return (bool)propertyLockInputToGameView.GetValue(null); }
            set { propertyLockInputToGameView.SetValue(null, value); }
        }

        public static void Load()
        {
            methodLoad.Invoke(null, null);
        }

        public static void Delete()
        {
            string filePath = (string)fieldFilePath.GetValue(null);
            if(File.Exists(filePath))
                File.Delete(filePath);
        }
    }
#endif
}
