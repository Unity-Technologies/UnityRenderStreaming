using System;                               //AssemblyLoadEventArgs
using System.Diagnostics;                   //StackTrace
using System.IO;                            //Path
using Unity.RenderStreaming.Editor;         //RequestJobManager
using UnityEditor.PackageManager;           //PackageCollection
using UnityEditor.PackageManager.Requests;  //Request
using UnityEngine;                          //ScriptableObject

public class RenderStreamingLegacyRPAutomator
{

//---------------------------------------------------------------------------------------------------------------------
    public static bool IsSampleImported() {
        return m_sampleImported;
    }

//---------------------------------------------------------------------------------------------------------------------

    public static void TryAddLegacyRPPackageAndImportSample() {
        m_sampleImported = false;

        //Some steps are necessary to "hack" so that Unity will execute this file every time we click
        //"Import in project" in the package manager UI.
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

        TryAddLegacyRPPackageAndImportSample();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void OnPackageListRequestSuccess(Request<PackageCollection> req) {

        ImportLegacyRPSample();

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

    static void OnLegacyRPPackageAdded(Request<PackageInfo> req) {
        ImportLegacyRPSample();
        UnityEditor.EditorApplication.UnlockReloadAssemblies();
        m_sampleImported = true;
    }

//---------------------------------------------------------------------------------------------------------------------
    static void OnLegacyRPPackageAddFailed(Request<PackageInfo> req) {
        UnityEditor.EditorApplication.UnlockReloadAssemblies();
    }


//---------------------------------------------------------------------------------------------------------------------

    static void ImportLegacyRPSample() {
        UnityEditor.EditorUtility.DisplayProgressBar(PROGRESS_BAR_TITLE, PROGRESS_BAR_INFO, 0.8f );
        UnityEditor.AssetDatabase.ImportPackage(m_unityPackageSamplePath, true);
        UnityEditor.EditorUtility.ClearProgressBar();
    }

//---------------------------------------------------------------------------------------------------------------------

    static void Init() {

        StackTrace st = new StackTrace(new StackFrame(true));
        StackFrame sf = st.GetFrame(0);

        string fileName = sf.GetFileName();
        if (String.IsNullOrEmpty(fileName))
            return;

        string path = fileName.Replace(@"\", "/" );


        //Change absolute path to relative path
        if (path.StartsWith(Application.dataPath)) {
            path=  "Assets" + path.Substring(Application.dataPath.Length);
        }

        m_codePath = path;

        string dir = Path.GetDirectoryName(path).Replace(@"\", "/" );
        m_settingsPath = dir + "/RenderStreamingLegacyRPSettings.json";
        m_unityPackageSamplePath =  dir + "/RenderStreamingLegacyRPSimple.unitypackage";
    }

//---------------------------------------------------------------------------------------------------------------------

    static RenderStreamingSettings LoadSettings() {
        if (!File.Exists(m_settingsPath))
            return null;

        return JsonUtility.FromJson<RenderStreamingSettings>(File.ReadAllText(m_settingsPath));
    }


//---------------------------------------------------------------------------------------------------------------------

    static void SaveSettings(RenderStreamingSettings settings) {
        //Make sure the settings is not set as ReadOnly (might happen when importing sample in older versions of Unity)
        FileAttributes attributes = File.GetAttributes(m_settingsPath);
        if (attributes.HasFlag(FileAttributes.ReadOnly))
            File.SetAttributes(m_settingsPath, attributes & ~FileAttributes.ReadOnly);

        File.WriteAllText(m_settingsPath,JsonUtility.ToJson(settings));
    }

//---------------------------------------------------------------------------------------------------------------------


    static string m_settingsPath;
    static string m_unityPackageSamplePath;
    static string m_codePath;

    static bool m_sampleImported = false;

    const string PROGRESS_BAR_TITLE = "RenderStreaming";
    const string PROGRESS_BAR_INFO  = "Installing LegacyRP Sample";

}


//Automatically Modified to trigger recompilation