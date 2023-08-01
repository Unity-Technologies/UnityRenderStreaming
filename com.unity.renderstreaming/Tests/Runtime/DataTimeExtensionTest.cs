using System;
using NUnit.Framework;

namespace Unity.RenderStreaming.RuntimeTest
{

    public class DataTimeExtensionTest
    {
        [Test]
        public void ToJsMilliseconds()
        {
            Assert.That(DateTime.Now.ToJsMilliseconds(), Is.GreaterThan(0));
        }
    }
}
