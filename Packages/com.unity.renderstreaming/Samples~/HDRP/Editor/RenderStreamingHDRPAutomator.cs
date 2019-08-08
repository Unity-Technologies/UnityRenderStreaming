using UnityEditor.PackageManager.Requests;  //Request
using UnityEditor.PackageManager;           //PackageCollection
using Unity.RenderStreaming.Editor;         //RequestJobManager
using System.IO;                            //Path
using UnityEngine;                          //ScriptableObject
using System.Diagnostics;                   //StackTrace

public class RenderStreamingHDRPAutomator 
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
        RenderStreamingSettings settings = LoadSettings();

        if (null!=settings && !string.IsNullOrEmpty(settings.Version)) {
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


        //update json
        RenderStreamingSettings settings = LoadSettings();
        if (null!=settings) {
            PackageInfo renderStreamingPackageInfo = req.FindPackage("com.unity.renderstreaming");
            if (null != renderStreamingPackageInfo) {
                settings.Version = renderStreamingPackageInfo.version;
                SaveSettings(settings);
            }
        }

        //Change the C# file to trigger recompilation next time "Import in project" is pushed again
        File.AppendAllText(m_codePath, System.Environment.NewLine + "//Automatically Modified to trigger recompilation");
        UnityEditor.AssetDatabase.ImportAsset(m_codePath);
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnHDRPPackageAdded(Request<PackageInfo> req) {
        ImportHDRPSample();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void ImportHDRPSample() {
        UnityEditor.EditorUtility.DisplayProgressBar(PROGRESS_BAR_TITLE, PROGRESS_BAR_INFO, 0.8f );
        UnityEditor.AssetDatabase.ImportPackage(m_unityPackageSamplePath, true);
        UnityEditor.EditorUtility.ClearProgressBar();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void Init() {

        StackTrace st = new StackTrace(new StackFrame(true));
        StackFrame sf = st.GetFrame(0);

        string path = sf.GetFileName().Replace(@"\", "/" );

        //Change absolute path to relative path
        if (path.StartsWith(Application.dataPath)) {
            path=  "Assets" + path.Substring(Application.dataPath.Length);
        }

        m_codePath = path;

        string dir = Path.GetDirectoryName(path).Replace(@"\", "/" );
        m_settingsPath = dir + "/RenderStreamingHDRPSettings.json";
        m_unityPackageSamplePath =  dir + "/RenderStreamingHDRPSimple.unitypackage";
    }

//---------------------------------------------------------------------------------------------------------------------

    static RenderStreamingSettings LoadSettings() {
        if (!File.Exists(m_settingsPath))
            return null;
        
        return JsonUtility.FromJson<RenderStreamingSettings>(File.ReadAllText(m_settingsPath));
    }


//---------------------------------------------------------------------------------------------------------------------

    static void SaveSettings(RenderStreamingSettings settings) {
        File.WriteAllText(m_settingsPath,JsonUtility.ToJson(settings));
    }

//---------------------------------------------------------------------------------------------------------------------


    static string m_settingsPath;
    static string m_unityPackageSamplePath;
    static string m_codePath;

    readonly static string PROGRESS_BAR_TITLE = "RenderStreaming";
    readonly static string PROGRESS_BAR_INFO  = "Installing HDRP Sample";

}
