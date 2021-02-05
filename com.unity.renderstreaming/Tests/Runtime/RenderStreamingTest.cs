using NUnit.Framework;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderStreamingTest
    {
        [Test]
        [Ignore("This test occurs a crash")]
        public void Test()
        {
            GameObject obj = new GameObject();
            obj.AddComponent<RenderStreaming>();
            Object.DestroyImmediate(obj);
        }
    }
}
