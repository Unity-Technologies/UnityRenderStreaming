
using UnityEditor.PackageManager.Requests;
using UnityEditor.PackageManager;           //PackageCollection
using Unity.RenderStreaming.Editor; //RequestJobManager
using System.Diagnostics; //StackTrace
using System.IO; //Path

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
    static void OnLoad() {
        RequestJobManager.CreateListRequest(false, true, OnPackageListRequestSuccess, null);
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnPackageListRequestSuccess(Request<PackageCollection> req) {

        const string HDRP_PACKAGE_NAME = "com.unity.render-pipelines.high-definition";
        req.FindPackage(HDRP_PACKAGE_NAME);
        if (null == req) {
            //Add HDRP
            RequestJobManager.CreateAddRequest(HDRP_PACKAGE_NAME, OnHDRPPackageAdded, null);
        } else {
            ImportHDRPSample();            
        }
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnHDRPPackageAdded(Request<PackageInfo> req) {
        ImportHDRPSample();            
    }

//---------------------------------------------------------------------------------------------------------------------


}
