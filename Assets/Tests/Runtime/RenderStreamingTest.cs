using System.Collections;
using System.IO;
using System.Threading;
using NUnit.Framework;
using UnityEngine.TestTools;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [UnityPlatform(exclude = new[]{RuntimePlatform.OSXEditor})]
    public class RenderStreamingTest : IPrebuildSetup
    {
        private GameObject _camera;
        private System.Diagnostics.Process _process;
        private const string PathWebapp = "WebApp/bin~";

        public void Setup()
        {
#if UNITY_EDITOR
            if (Directory.Exists(PathWebapp))
            {
                return;
            }
            Unity.RenderStreaming.Editor.WebAppDownloader.DownloadCurrentVersionWebApp(PathWebapp);
            int time = 0;
            int timeout = 180000;
            while (true)
            {
                Thread.Sleep(1);
                if (Directory.Exists(PathWebapp))
                {
                    break;
                }

                if (time > timeout)
                {
                    Assert.Fail("Downloading webapp is timeout");
                }
                time++;
            }
#endif
        }

        GameObject CreateMainCamera()
        {
            var obj = new GameObject("Camera");
            var camera = obj.AddComponent<Camera>();
            camera.tag = "MainCamera";
            return obj;
        }

        [UnitySetUp]
        public IEnumerator OneTimeSetUp()
        {
            _process = new System.Diagnostics.Process();
            _process.StartInfo.FileName = PathWebapp + "/webserver";
            _process.ErrorDataReceived += (sender, ev) =>
            {
                Debug.Log(ev.Data);
            };
            _process.OutputDataReceived += (sender, ev) =>
            {
                Debug.Log(ev.Data);
            };
            var ret = _process.Start();
            Assert.True(ret);

            yield return new WaitForSeconds(3.0f);
        }

        [UnityTearDown]
        public IEnumerator OneTimeTearDown()
        {
            _process.Kill();
            yield return null;
        }

        [SetUp]
        public void Initialize()
        {
            _camera = CreateMainCamera();
        }

        [TearDown]
        public void Clean()
        {
            Object.DestroyImmediate(_camera);
        }

        [UnityTest]
        public IEnumerator InstantiateAndDestroy()
        {
            var obj = new GameObject();
            obj.AddComponent<RenderStreaming>();
            yield return new WaitForSeconds(3);
            Object.DestroyImmediate(obj);
        }
    }
}
