using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using NUnit.Framework;
using UnityEditor;
using UnityEngine.TestTools;
using UnityEngine;
using UnityEngine.Rendering;

namespace Unity.RenderStreaming
{
    [UnityPlatform(exclude = new[]{RuntimePlatform.OSXEditor, RuntimePlatform.WindowsEditor})]
    public class RenderStreamingTest : IPrebuildSetup
    {
        private GameObject _camera;
        private System.Diagnostics.Process _process;
        private const string PathWebapp = "WebApp/bin~";

#if UNITY_EDITOR
        static void SetUpWebApp()
        {
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
        }

        private static readonly Dictionary<BuildTarget, GraphicsDeviceType[]> GraphicsDeviceTypeDictionary =
            new Dictionary<BuildTarget, GraphicsDeviceType[]>()
            {
                { BuildTarget.StandaloneWindows64, new[] { GraphicsDeviceType.Direct3D11 } },
                { BuildTarget.StandaloneLinux64, new[] { GraphicsDeviceType.Vulkan } },
                { BuildTarget.StandaloneOSX, new[] { GraphicsDeviceType.Metal } }
            };

        static void ChangeGraphicsApi()
        {

            var target = EditorUserBuildSettings.activeBuildTarget;
            PlayerSettings.SetGraphicsAPIs(target, GraphicsDeviceTypeDictionary[target]);
        }
#endif

        public void Setup()
        {
#if UNITY_EDITOR
            SetUpWebApp();

            ChangeGraphicsApi();
#endif
        }

        static GameObject CreateMainCamera()
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
#if UNITY_EDITOR_WIN
            _process.StartInfo.FileName = System.IO.Path.Combine(PathWebapp, "webserver.exe");
#else
            _process.StartInfo.FileName = System.IO.Path.Combine(PathWebapp, "webserver");
#endif
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
