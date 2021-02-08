using NUnit.Framework;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderpipelineTest
    {
        [Test]
        public void Test()
        {
            GameObject obj = new GameObject();
            obj.AddComponent<Camera>();
            obj.AddComponent<RenderTextureBlitter>();
            Object.DestroyImmediate(obj);
        }
    }
}
