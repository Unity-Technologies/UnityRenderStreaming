using NUnit.Framework;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    public class RenderpipelineTest
    {
        [Test]
        public void Test()
        {
            GameObject obj = new GameObject();
            obj.AddComponent<RenderTextureBlitter>();
        }
    }
}
