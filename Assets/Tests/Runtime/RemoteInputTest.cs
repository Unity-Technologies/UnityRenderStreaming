using System.Collections;
using NUnit.Framework;
using UnityEngine.TestTools;
using UnityEngine;

namespace Unity.RenderStreaming
{

    public class RemoteInputTest
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

        [Test]
        public void InitializeAndDestroy()
        {
           RemoteInput.Initialize();
           RemoteInput.Destroy();
        }
    }
}
