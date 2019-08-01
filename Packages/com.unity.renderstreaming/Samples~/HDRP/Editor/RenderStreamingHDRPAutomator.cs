using UnityEditor.PackageManager.Requests;  //Request
using UnityEditor.PackageManager;           //PackageCollection
using Unity.RenderStreaming.Editor;         //RequestJobManager
using System.IO;                            //Path
using UnityEngine;                          //ScriptableObject

[UnityEditor.InitializeOnLoad]
public class RenderStreamingHDRPAutomator : ScriptableObject 
{
    void ImportHDRPSample() {
        string path = UnityEditor.AssetDatabase.GetAssetPath(m_samplePackage);
        UnityEditor.AssetDatabase.ImportPackage(path, true);
    }

//---------------------------------------------------------------------------------------------------------------------

    public void OnEnable() {

        //Two steps are necessary to "hack" so that Unity will execute this file everytime we click "Import in project"
        //In the package manager UI.
        //1. Import the ScriptableObject to make sure that we are dealing with the new copied asset, and not the asset
        //   modified later in this module
        //2. Change the C# code (this file) a bit to trigger C# compilation, even though the file content is the same

        string path = UnityEditor.AssetDatabase.GetAssetPath(this);
        UnityEditor.AssetDatabase.ImportAsset(path);

        if (!string.IsNullOrEmpty(m_version)) {
            return;
        }
        
        RequestJobManager.CreateListRequest(false, true, OnPackageListRequestSuccess, null);       
    }

//---------------------------------------------------------------------------------------------------------------------

    void OnPackageListRequestSuccess(Request<PackageCollection> req) {

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
        if (null!=renderStreamingPackageInfo) {
            m_version = renderStreamingPackageInfo.version;
        }

        //Force saving the ScriptableObject
        string path = UnityEditor.AssetDatabase.GetAssetPath(this);
        string[] assetPaths = { path };
        UnityEditor.AssetDatabase.ForceReserializeAssets(assetPaths, UnityEditor.ForceReserializeAssetsOptions.ReserializeAssets);

        //Change the C# file to trigger recompilation.
        string cScript = Path.ChangeExtension(path,".cs");
        File.AppendAllText(cScript, System.Environment.NewLine + "//Automatically Modified to trigger recompilation");
        UnityEditor.AssetDatabase.ImportAsset(cScript);
    }

//---------------------------------------------------------------------------------------------------------------------

    void OnHDRPPackageAdded(Request<PackageInfo> req) {
        ImportHDRPSample();
    }

//---------------------------------------------------------------------------------------------------------------------

    [SerializeField] private string m_version = null;
    [SerializeField] Object m_samplePackage = null;

}

