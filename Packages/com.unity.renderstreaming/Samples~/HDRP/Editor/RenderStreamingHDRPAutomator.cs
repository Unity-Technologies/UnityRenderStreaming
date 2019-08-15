using System;                               //AssemblyLoadEventArgs 
using System.Diagnostics;                   //StackTrace
using System.IO;                            //Path
using Unity.RenderStreaming.Editor;         //RequestJobManager
using UnityEditor.PackageManager;           //PackageCollection
using UnityEditor.PackageManager.Requests;  //Request
using UnityEngine;                          //ScriptableObject

public class RenderStreamingHDRPAutomator 
{
   
//---------------------------------------------------------------------------------------------------------------------
    public static bool IsSampleImported() {
        return m_sampleImported;
    }

//---------------------------------------------------------------------------------------------------------------------

    public static void TryAddHDRPPackageAndImportSample() {
        m_sampleImported = false;

        //Some steps are necessary to "hack" so that Unity will execute this file everytime we click "Import in project"
        //In the package manager UI.
        //1. Import the json to make sure that we are dealing with the new settings
        //2. Change the C# code (this file) a bit to trigger C# compilation, even though the file content is the same
        //3. One additional requirement that the asset must be in the same path as this C# file

        Init();
        RenderStreamingSettings settings = LoadSettings();

        if (null!=settings && !string.IsNullOrEmpty(settings.Version)) {
            m_sampleImported = true;
            return;
        }

        RequestJobManager.CreateListRequest(false, true, OnPackageListRequestSuccess, null);
        UnityEditor.EditorUtility.DisplayProgressBar(PROGRESS_BAR_TITLE, PROGRESS_BAR_INFO, 0.1f );
    }

//---------------------------------------------------------------------------------------------------------------------

    [UnityEditor.InitializeOnLoadMethod]
    static void OnLoad() {
        if (Application.isBatchMode)
            return;

        TryAddHDRPPackageAndImportSample();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnPackageListRequestSuccess(Request<PackageCollection> req) {

        //Check if HDRP hasn't been installed and add it if that's the case
        const string HDRP_PACKAGE_NAME = "com.unity.render-pipelines.high-definition";
        PackageInfo packageInfo = req.FindPackage(HDRP_PACKAGE_NAME);
        if (null == packageInfo) {
            UnityEditor.EditorApplication.LockReloadAssemblies();
            RequestJobManager.CreateAddRequest(HDRP_PACKAGE_NAME, OnHDRPPackageAdded, OnHDRPPackageAddFailed);
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
        UnityEditor.EditorApplication.UnlockReloadAssemblies();      
        m_sampleImported = true;
    }
   
//---------------------------------------------------------------------------------------------------------------------
    static void OnHDRPPackageAddFailed(Request<PackageInfo> req) {
        UnityEditor.EditorApplication.UnlockReloadAssemblies();
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

    static bool m_sampleImported = false;

    readonly static string PROGRESS_BAR_TITLE = "RenderStreaming";
    readonly static string PROGRESS_BAR_INFO  = "Installing HDRP Sample";

}

