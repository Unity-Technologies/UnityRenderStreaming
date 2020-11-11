using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    public static class RenderStreamingMenu
    {
        [MenuItem("Edit/Render Streaming/Download web app")]
        static void DownloadWebAppFromMenu() {
            WebAppDownloader.GetPackageVersion("com.unity.renderstreaming", (version) => {
                var dstPath = EditorUtility.OpenFolderPanel("Select download folder", "", "");
                WebAppDownloader.DownloadWebApp(version, dstPath, null);
            });
        }

        [MenuItem("Edit/Render Streaming/Show web app documentation")]
        static void ShowWepAppDocumentation()
        {
            WebAppDownloader.GetPackageVersion("com.unity.renderstreaming", (version) => {
                var url = WebAppDownloader.GetURLDocumentation(version);
                Application.OpenURL(url);
            });
        }

        [MenuItem("Edit/Render Streaming/Show web app source code")]
        static void ShowWepAppSourceCode()
        {
            WebAppDownloader.GetPackageVersion("com.unity.renderstreaming", (version) => {
                var url = WebAppDownloader.GetURLSourceCode(version);
                Application.OpenURL(url);
            });
        }
    }
}

