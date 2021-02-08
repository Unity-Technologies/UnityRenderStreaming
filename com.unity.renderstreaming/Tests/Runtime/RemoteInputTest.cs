using System;
using NUnit.Framework;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RemoteInputTest
    {
        [Test]
        public void RemoteInputReceiverCreate()
        {
            RemoteInput input = RemoteInputReceiver.Create();
            Assert.NotNull(input);
        }

        [Test]
        public void RemoteInputReceiverAll()
        {
            Assert.AreEqual(RemoteInputReceiver.All().Count, 0);
            RemoteInput input = RemoteInputReceiver.Create();
            Assert.AreEqual(RemoteInputReceiver.All().Count, 1);
            input.Dispose();
            Assert.AreEqual(RemoteInputReceiver.All().Count, 0);
        }

        [Test]
        public void RemoteInputProperties()
        {
            RemoteInput input = RemoteInputReceiver.Create();
            Assert.NotNull(input.RemoteMouse);
            Assert.NotNull(input.RemoteGamepad);
            Assert.NotNull(input.RemoteKeyboard);
            Assert.NotNull(input.RemoteTouchscreen);
            input.Dispose();
        }

        [Test]
        public void RemoteInputProcessInput()
        {
            RemoteInput input = RemoteInputReceiver.Create();
            Assert.Throws<ArgumentException>(() => input.ProcessInput(new byte[0]));
            input.Dispose();
        }
    }
}
