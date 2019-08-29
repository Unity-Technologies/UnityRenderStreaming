using UnityEditor;
using UnityEngine;
using System.Net;

namespace Unity.RenderStreaming.Editor
{
    public static class RenderStreamingMenu
    {
        const string PackageVersion = "1.0.0-preview";
        const string BranchName = "release/1.0.0";
        const string URLRoot = "https://github.com/Unity-Technologies/UnityRenderStreaming";
        const string PathWebAppForMac = "releases/download/{0}/webserver";
        const string PathWebAppForWin = "releases/download/{0}/webserver.exe";
        const string PathWebAppSourceCode = "tree/{0}/WebApp";
        const string PathWebAppDocumentation = "blob/{0}/Packages/com.unity.template.renderstreaming/Documentation~/en/webserver.md";

        [MenuItem("Edit/Render Streaming/Download web app")]
        static void DownloadWepApp()
        {
#if UNITY_EDITOR_WIN
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForWin, PackageVersion));
#elif UNITY_EDITOR_OSX
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForMac, PackageVersion));
#endif
            var client = new WebClient();
            var filename = System.IO.Path.GetFileName(url);
            var tmpFilePath = System.IO.Path.Combine(Application.temporaryCachePath, filename);

            client.DownloadFileCompleted += (object sender, System.ComponentModel.AsyncCompletedEventArgs e) =>
            {
                EditorUtility.ClearProgressBar();

                if (!System.IO.File.Exists(tmpFilePath))
                {
                    Debug.LogErrorFormat("Download failed. url:{0}", url);
                    return;
                }

                var dstPath = EditorUtility.OpenFolderPanel("Select download folder", "", "");
                dstPath = System.IO.Path.Combine(dstPath, filename);
                if(System.IO.File.Exists(dstPath))
                {
                    System.IO.File.Delete(dstPath);
                }
                System.IO.File.Move(tmpFilePath, dstPath);
            };
            client.DownloadProgressChanged += (object sender, DownloadProgressChangedEventArgs e) =>
            {
                var progress = e.ProgressPercentage / 100f;
                var progressing = EditorUtility.DisplayCancelableProgressBar("Downloading", url, progress);
                if (progressing)
                    return;
                client.CancelAsync();
            };
            client.DownloadFileAsync(new System.Uri(url), tmpFilePath);
        }

        [MenuItem("Edit/Render Streaming/Show web app documentation")]
        static void ShowWepAppDocumentation()
        {
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppDocumentation, BranchName));
            Application.OpenURL(url);
        }
        [MenuItem("Edit/Render Streaming/Show web app source code")]
        static void ShowWepAppSourceCode()
        {
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppSourceCode, BranchName));
            Application.OpenURL(url);
        }
    }
}

