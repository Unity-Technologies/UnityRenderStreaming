using NUnit.Framework;

namespace Unity.RenderStreaming
{

    public class RemoteInputTest
    {
        [Test]
        public void InitializeAndDestroy()
        {
           RemoteInput.Initialize();
           RemoteInput.Destroy();
        }
    }
}
