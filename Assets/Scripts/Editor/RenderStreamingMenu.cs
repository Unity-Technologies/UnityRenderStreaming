using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    public static class RenderStreamingMenu
    {
        const string URLWebApp = "https://github.com/Unity-Technologies/UnityRenderStreaming/releases/download/{0}/webserver";

        [MenuItem("Tools/Render Streaming/Download web app")]
        static void DownloadWepApp()
        {
            //  string.Format(URLWebApp, packageVersion);
        }
    }
}

