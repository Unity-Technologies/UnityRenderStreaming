using UnityEditor.PackageManager.Requests;  //Request
using UnityEditor.PackageManager;           //PackageCollection
using Unity.RenderStreaming.Editor;         //RequestJobManager
using System.IO;                            //Path
using UnityEngine;                          //ScriptableObject
using System.Diagnostics;                   //StackTrace

public class RenderStreamingHDRPAutomator : ScriptableObject 
{
    [UnityEditor.InitializeOnLoadMethod]
    static public void OnLoad() {
        //Some steps are necessary to "hack" so that Unity will execute this file everytime we click "Import in project"
        //In the package manager UI.
        //1. Import the ScriptableObject to make sure that we are dealing with the new copied asset, and not the asset
        //   modified later in this module
        //2. Change the C# code (this file) a bit to trigger C# compilation, even though the file content is the same
        //3. One additional requirement that the asset must be in the same path as this C# file

        Init();
        UnityEditor.AssetDatabase.ImportAsset(m_automatorAssetPath);

        if (!string.IsNullOrEmpty(m_automatorAsset.m_version)) {
            return;
        }
        
        RequestJobManager.CreateListRequest(false, true, OnPackageListRequestSuccess, null);
        UnityEditor.EditorUtility.DisplayProgressBar(PROGRESS_BAR_TITLE, PROGRESS_BAR_INFO, 0.1f );
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnPackageListRequestSuccess(Request<PackageCollection> req) {

        //Check if HDRP hasn't been installed and add it if that's the case
        const string HDRP_PACKAGE_NAME = "com.unity.render-pipelines.high-definition";
        PackageInfo packageInfo = req.FindPackage(HDRP_PACKAGE_NAME);
        if (null == packageInfo) {
            RequestJobManager.CreateAddRequest(HDRP_PACKAGE_NAME, OnHDRPPackageAdded, null);
        } else {
            ImportHDRPSample();
        }

        //update ScriptableObject asset
        PackageInfo renderStreamingPackageInfo = req.FindPackage("com.unity.renderstreaming");
        if (null != renderStreamingPackageInfo) {
            m_automatorAsset.m_version = renderStreamingPackageInfo.version;
        }

        //Force saving the ScriptableObject
        string[] assetPaths = { m_automatorAssetPath };
        UnityEditor.AssetDatabase.ForceReserializeAssets(assetPaths, UnityEditor.ForceReserializeAssetsOptions.ReserializeAssets);

        //Change the C# file to trigger recompilation next time "Import in project" is pushed again
        string cScript = Path.ChangeExtension(m_automatorAssetPath, ".cs");
        File.AppendAllText(cScript, System.Environment.NewLine + "//Automatically Modified to trigger recompilation");
        UnityEditor.AssetDatabase.ImportAsset(cScript);
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnHDRPPackageAdded(Request<PackageInfo> req) {
        ImportHDRPSample();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void ImportHDRPSample() {
        UnityEditor.EditorUtility.DisplayProgressBar(PROGRESS_BAR_TITLE, PROGRESS_BAR_INFO, 0.8f );

        string path = UnityEditor.AssetDatabase.GetAssetPath(m_automatorAsset.m_samplePackage);
        UnityEditor.AssetDatabase.ImportPackage(path, true);

        UnityEditor.EditorUtility.ClearProgressBar();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void Init() {

        StackTrace st = new StackTrace(new StackFrame(true));
        StackFrame sf = st.GetFrame(0);

        string path = Path.ChangeExtension(sf.GetFileName(),".asset").Replace(@"\", "/" );

        //Change absolute path to relative path
        if (path.StartsWith(Application.dataPath)) {
            path=  "Assets" + path.Substring(Application.dataPath.Length);
        }
        m_automatorAssetPath = path;
        m_automatorAsset = UnityEditor.AssetDatabase.LoadAssetAtPath<RenderStreamingHDRPAutomator>(m_automatorAssetPath);
    }

//---------------------------------------------------------------------------------------------------------------------

    [SerializeField] private string m_version = null;
    [SerializeField] Object m_samplePackage = null;

    static string m_automatorAssetPath;
    static RenderStreamingHDRPAutomator m_automatorAsset;

    readonly static string PROGRESS_BAR_TITLE = "RenderStreaming";
    readonly static string PROGRESS_BAR_INFO  = "Installing HDRP Sample";

}

