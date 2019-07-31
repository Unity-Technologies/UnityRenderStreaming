
using DEBUG = UnityEngine.Debug; 
using PM = UnityEditor.PackageManager;

using System.Diagnostics; //StackTrace
using System.IO; //Path
using System.Collections.Generic; //IEnumerable

public class RenderStreamingHDRPUtility
{
    static void ImportHDRPSample()
    {
        StackTrace st = new StackTrace(new StackFrame(true));
        StackFrame sf = st.GetFrame(0);

        string sampleDir = Path.GetDirectoryName(sf.GetFileName());
        UnityEditor.AssetDatabase.ImportPackage(sampleDir + "/RenderStreamingHDRPSimple.unitypackage", true);
    }

//---------------------------------------------------------------------------------------------------------------------

    [UnityEditor.InitializeOnLoadMethod]
    static void OnLoadedInEditor()
    {
        m_listRequest = UnityEditor.PackageManager.Client.List(false, true);
        UnityEditor.EditorApplication.update += WaitToInstallHDRP;
    }

//---------------------------------------------------------------------------------------------------------------------

    static void WaitToInstallHDRP() {
        if (null == m_listRequest) {
            UnityEditor.EditorApplication.update -= WaitToInstallHDRP;
            return;
        }

        if (m_listRequest.IsCompleted) {
            UnityEditor.EditorApplication.update -= WaitToInstallHDRP;

            if (m_listRequest.Status == PM.StatusCode.Success) {
                IEnumerable<PM.PackageInfo> packageInfoCollection = m_listRequest.Result as IEnumerable<PM.PackageInfo>;
                if (null == packageInfoCollection) {
                    DEBUG.LogError("ListRequest.Result is not an IEnumerable ?");
                    return;
                }

                bool found = false;
                var enumerator = packageInfoCollection.GetEnumerator();
                while (enumerator.MoveNext() && !found) {
                    PM.PackageInfo curInfo = enumerator.Current;
                    if (curInfo.name == "com.unity.render-pipelines.high-definition") {
                        found = true;
                    }
                }

                
                if (!found) {
                    //Add HDRP
                    m_addRequest = PM.Client.Add("com.unity.render-pipelines.high-definition");
                    UnityEditor.EditorApplication.update += WaitToImportHDRPSample;
                } else {
                    ImportHDRPSample();
                }
            }
        }
    }

//---------------------------------------------------------------------------------------------------------------------
    static void WaitToImportHDRPSample()
    {
        if (null == m_addRequest) {
            UnityEditor.EditorApplication.update -= WaitToImportHDRPSample;
            return;
        }

        if (m_addRequest.IsCompleted) {
            UnityEditor.EditorApplication.update -= WaitToImportHDRPSample;
            ImportHDRPSample();
        }
    }

//---------------------------------------------------------------------------------------------------------------------
    static UnityEditor.PackageManager.Requests.ListRequest m_listRequest = null;
    static UnityEditor.PackageManager.Requests.AddRequest m_addRequest = null;

}
