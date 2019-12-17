using UnityEngine;

namespace Unity.RenderStreaming.WebSocket
{
    static class DebugUtils
    {
        public static void DebugAssert(bool condition, string logMsg)
        {
            if (!condition)
            {
                Debug.Log(message: logMsg);
            }
        }
    }
}
