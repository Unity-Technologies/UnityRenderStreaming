using System;
using System.Collections;
using System.Reflection;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.RenderStreaming.Signaling;
using UnityEngine;
using UnityEngine.TestTools;
using Object = UnityEngine.Object;

namespace Unity.RenderStreaming.RuntimeTest
{
    static class RenderStreamingExtension
    {
        private static Type type;
        private static FieldInfo fieldRunOnAwake;

        static RenderStreamingExtension()
        {
            type = typeof(RenderStreaming);
            fieldRunOnAwake = type.GetField("runOnAwake",
                BindingFlags.NonPublic | BindingFlags.Instance);
        }

        //public static void SetRunOnAwake(this RenderStreaming obj, bool value)
        //{
        //    fieldRunOnAwake.SetValue(obj, value);
        //}
    }

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
        }

        [Test]
        public void DoNothing()
        {
        }

        [Test]
        public void Run()
        {
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(signaling:mock);
        }

        [UnityTest]
        public IEnumerator RunAgain()
        {
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(signaling: mock);
            yield return 0;
            component.Stop();
            yield return 0;
            component.Run(signaling: mock);
        }
    }
}
