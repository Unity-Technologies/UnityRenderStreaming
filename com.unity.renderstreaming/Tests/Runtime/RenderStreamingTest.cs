using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    public class RenderStreamingTest
    {
        [Test]
        [Ignore("This test occurs a crash")]
        public void Test()
        {
            GameObject obj = new GameObject();
            obj.AddComponent<RenderStreaming>();
        }
    }
}
