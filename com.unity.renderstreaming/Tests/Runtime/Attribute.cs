using System;
using NUnit.Framework;

namespace Unity.RenderStreaming.RuntimeTest
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
    internal class LongRunningAttribute : CategoryAttribute
    {
    }
}
