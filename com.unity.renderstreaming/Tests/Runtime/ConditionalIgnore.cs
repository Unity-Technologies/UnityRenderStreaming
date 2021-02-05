using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    internal class ConditionalIgnore
    {
        public const string IL2CPP = "IgnoreIL2CPP";

        [RuntimeInitializeOnLoadMethod]
        static void OnLoad()
        {
#if ENABLE_IL2CPP
            ConditionalIgnoreAttribute.AddConditionalIgnoreMapping(IL2CPP, true);
#else
            ConditionalIgnoreAttribute.AddConditionalIgnoreMapping(IL2CPP, false);
#endif

        }
    }
}
