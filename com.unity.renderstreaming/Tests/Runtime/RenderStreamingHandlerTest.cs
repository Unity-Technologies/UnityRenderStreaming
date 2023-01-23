using System.Collections;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.RenderStreaming.Signaling;
using UnityEngine;
using UnityEngine.TestTools;
using Object = UnityEngine.Object;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderStreamingHandlerTest
    {
        RenderStreamingHandler component;

        [SetUp]
        public void SetUp()
        {
            GameObject obj = new GameObject();
            obj.SetActive(false);
            component = obj.AddComponent<RenderStreamingHandler>();
        }

        [TearDown]
        public void TearDown()
        {
            Object.DestroyImmediate(component.gameObject);
        }

        [Test]
        public void DoNothing()
        {
        }

        [Test]
        public void Run()
        {
            var handler = component.gameObject.AddComponent<SingleConnection>();
            var handlers = new SignalingHandlerBase[] { handler };
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(signaling: mock, handlers: handlers);
        }

        [Test, Ignore("Failed this test on macOS and Linux platform because of the signaling process.")]
        public void RunDefault()
        {
            var handler = component.gameObject.AddComponent<SingleConnection>();
            var handlers = new SignalingHandlerBase[] { handler };
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(handlers:handlers);
        }


        [Test]
        public void ThrowExceptionIfHandlerIsNullOrEmpty()
        {
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            Assert.That(() => component.Run(signaling: mock), Throws.InvalidOperationException);

            var handlers = new SignalingHandlerBase[] {};
            Assert.That(() => component.Run(signaling: mock, handlers:handlers),
                Throws.InvalidOperationException);
        }


        [UnityTest]
        public IEnumerator RunAgain()
        {
            var handler = component.gameObject.AddComponent<SingleConnection>();
            var handlers = new SignalingHandlerBase[] { handler };
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(signaling:mock, handlers:handlers);
            yield return 0;
            component.Stop();
            yield return 0;
            component.Run(signaling:mock, handlers:handlers);
        }

        [Test]
        public void AddAndRemoveSignalingSettings()
        {
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.SetSignalingSettings(new MockSignalingSettings());
            var handler = component.gameObject.AddComponent<SingleConnection>();
            component.AddSignalingHandler(handler);
            Assert.That(() => component.Run(), Throws.Nothing);
            component.RemoveSignalingHandler(handler);
        }

        [Test]
        public void ThrowExceptionSetSignalingOnRunning()
        {
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.SetSignalingSettings(new MockSignalingSettings());
            var handler = component.gameObject.AddComponent<SingleConnection>();
            component.AddSignalingHandler(handler);
            Assert.That(() => component.Run(), Throws.Nothing);
            Assert.That(component.Running, Is.True);

            Assert.That(() => component.SetSignalingSettings(new MockSignalingSettings()), Throws.InvalidOperationException);
        }
    }
}
