using UnityEditor;
using UnityEditor.PackageManager;
using UnityEditor.PackageManager.Requests;
using UnityEngine;
using System.Net;

namespace Unity.RenderStreaming.Editor
{
    public static class RenderStreamingMenu
    {
        const string URLRoot = "https://github.com/Unity-Technologies/UnityRenderStreaming";
        const string PathWebAppForMac = "releases/download/{0}/webserver";
        const string PathWebAppForWin = "releases/download/{0}/webserver.exe";
        const string PathWebAppSourceCode = "tree/release/{0}/WebApp";
        const string PathWebAppDocumentation = "blob/release/{0}/Packages/com.unity.template.renderstreaming/Documentation~/en/webserver.md";

        [MenuItem("Edit/Render Streaming/Download web app")]
        static void DownloadWepApp()
        {
            RequestPackageVersion("com.unity.renderstreaming", (version) =>
            {
#if UNITY_EDITOR_WIN
                var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForWin, version));
#elif UNITY_EDITOR_OSX
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForMac, version));
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
                    if (string.IsNullOrEmpty(dstPath))
                    {
                        return;
                    }
                    dstPath = System.IO.Path.Combine(dstPath, filename);
                    if (System.IO.File.Exists(dstPath))
                    {
                        System.IO.File.Delete(dstPath);
                    }
                    System.IO.File.Move(tmpFilePath, dstPath);
                    EditorUtility.RevealInFinder(dstPath);
                };
                client.DownloadProgressChanged += (object sender, DownloadProgressChangedEventArgs e) =>
                {
                    var progress = e.ProgressPercentage / 100f;
                    if(EditorUtility.DisplayCancelableProgressBar("Downloading", url, progress))
                    {
                        client.CancelAsync();
                    }
                };
                client.DownloadFileAsync(new System.Uri(url), tmpFilePath);
            });
        }

        static void RequestPackageVersion(string packageName, System.Action<string> callback)
        {
            // request package list to get package version
            RequestJobManager.CreateListRequest(false, true, (req) =>
            {
                var packageInfo = req.FindPackage(packageName);
                if (null == packageInfo)
                {
                    Debug.LogErrorFormat("Not found package \"{0}\"", packageName);
                    return;
                }
                callback(packageInfo.version);
            }, null);
        }

        [MenuItem("Edit/Render Streaming/Show web app documentation")]
        static void ShowWepAppDocumentation()
        {
            RequestPackageVersion("com.unity.renderstreaming", (version) =>
            {
                var pattern = @"\d+.\d+.\d+";
                var result = System.Text.RegularExpressions.Regex.Match(version, pattern);
                var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppDocumentation, result.Value));
                Application.OpenURL(url);
            });
        }
        [MenuItem("Edit/Render Streaming/Show web app source code")]
        static void ShowWepAppSourceCode()
        {
            RequestPackageVersion("com.unity.renderstreaming", (version) =>
            {
                var pattern = @"\d+.\d+.\d+";
                var result = System.Text.RegularExpressions.Regex.Match(version, pattern);
                var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppSourceCode, result.Value));
                Application.OpenURL(url);
            });
        }
    }
}

