using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    public static class RenderStreamingMenu
    {
        [MenuItem("Edit/Render Streaming/Download web app")]
        static void DownloadWebAppFromMenu() {
            var dstPath = EditorUtility.OpenFolderPanel("Select download folder", "", "");
            WebAppDownloader.DownloadWebApp(dstPath, null);
        }

        [MenuItem("Edit/Render Streaming/Show web app documentation")]
        static void ShowWepAppDocumentation()
        {
            var url = WebAppDownloader.GetURLDocumentation();
            Application.OpenURL(url);
        }

        [MenuItem("Edit/Render Streaming/Show web app source code")]
        static void ShowWepAppSourceCode()
        {
            var url = WebAppDownloader.GetURLSourceCode();
            Application.OpenURL(url);
        }
    }
}

