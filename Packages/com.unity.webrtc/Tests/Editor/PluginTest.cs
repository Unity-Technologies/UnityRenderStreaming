using System.Diagnostics;   //Process
using NUnit.Framework;      //Assert

namespace Unity.WebRTC.Editor {

public class PluginTest {
    [Test]
    public static void IsPluginLoaded() {
        // Get the current process.
        Process currentProcess = Process.GetCurrentProcess();
        bool found = false;
        var enumerator = currentProcess.Modules.GetEnumerator();
        while (enumerator.MoveNext()) {
            ProcessModule module = enumerator.Current as ProcessModule;
            if (null != module && module.ModuleName=="webrtc.dll")
                found = true;

        }
        Assert.True(found);
    }
}

} //namespace Unity.WebRTC.Editor
