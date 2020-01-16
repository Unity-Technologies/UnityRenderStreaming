using System.Collections;
using NUnit.Framework;
using UnityEngine.TestTools;
using UnityEngine;

namespace Unity.RenderStreaming
{

    public class RenderStreamingTest
    {
        GameObject CreateMainCamera()
        {
            var obj = new GameObject("Camera");
            var camera = obj.AddComponent<Camera>();
            camera.tag = "MainCamera";
            return obj;
        }

        [SetUp]
        public void SetUp()
        {
            CreateMainCamera();
        }

        [UnityTest]
        public IEnumerator InstantiateAndDestroy()
        {
            var obj = new GameObject();

            // TODO:: launch signaling server
            // obj.AddComponent<RenderStreaming>();
            yield return new WaitForSeconds(3);
            Object.DestroyImmediate(obj);
        }
    }
}
