using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    [UnityPlatform(exclude = new[] { RuntimePlatform.OSXPlayer, RuntimePlatform.LinuxPlayer, RuntimePlatform.WindowsPlayer })]
    public class RenderStreamingTest
    {
        [Test]
        public void Test()
        {
            GameObject obj = new GameObject();
            obj.AddComponent<RenderStreaming>();
        }
    }
}
