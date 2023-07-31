using NUnit.Framework;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

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
