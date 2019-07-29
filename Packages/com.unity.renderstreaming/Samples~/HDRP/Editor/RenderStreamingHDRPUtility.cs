using UnityEditor;
using UnityEngine;
using UnityEditor.PackageManager;
using System.Diagnostics; //StackTrace
using System.IO; //Path

public class RenderStreamingUtility
{
    // Add a menu item named "Do Something" to MyMenu in the menu bar.
    [MenuItem("Assets/Render Streaming/Import Simple HDRP Sample")]
    static void ImportHDRPSample()
    {
        UnityEditor.PackageManager.Client.Add("com.unity.render-pipelines.high-definition");

        StackTrace st = new StackTrace(new StackFrame(true));
        StackFrame sf = st.GetFrame(0);

        string sampleDir = Path.GetDirectoryName(sf.GetFileName());
        AssetDatabase.ImportPackage(sampleDir + "/RenderStreamingHDRPSimple.unitypackage",true);
    }
}
