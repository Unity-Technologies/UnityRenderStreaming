using System.Collections;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.RenderStreaming.Signaling;
using UnityEngine;
using UnityEngine.TestTools;
using Object = UnityEngine.Object;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderStreamingTest
    {
        RenderStreaming component;

        [SetUp]
        public void SetUp()
        {
            GameObject obj = new GameObject();
            obj.SetActive(false);
            component = obj.AddComponent<RenderStreaming>();
        }

        [TearDown]
        public void TearDown()
        {
            Object.DestroyImmediate(component.gameObject);

            RenderStreaming.Logger = Debug.unityLogger;
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

        [Test]
        public void ThrowExceptionIfHandlerIsNullOrEmpty()
        {
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            Assert.That(() => component.Run(signaling: mock),
                Throws.InvalidOperationException);

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
        public void Logger()
        {
            Assert.NotNull(RenderStreaming.Logger);
            Assert.AreEqual(RenderStreaming.Logger, Debug.unityLogger);

            Assert.That(() => RenderStreaming.Logger = null, Throws.ArgumentNullException);

            MockLogger logger = new MockLogger();
            Assert.That(() => RenderStreaming.Logger = logger, Throws.Nothing);
            Assert.AreEqual(logger, RenderStreaming.Logger);

            Assert.That(() => RenderStreaming.Logger = Debug.unityLogger, Throws.Nothing);
        }
    }
}
