using UnityEditor;
using UnityEditor.PackageManager;
using UnityEditor.PackageManager.Requests;
using UnityEngine;
using System.Net;

namespace Unity.RenderStreaming.Editor
{
    public static class WebAppDownloader
    {
        const string URLRoot = "https://github.com/Unity-Technologies/UnityRenderStreaming";
        const string LatestKnownVersion = "1.1.1-preview";

        // TODO::fix release process of webserver runtime.
        const string PathWebAppForMac = "releases/download/{0}/webserver";
        const string PathWebAppForLinux = "releases/download/{0}/webserver";
        const string PathWebAppForWin = "releases/download/{0}/webserver.exe";
        //

        const string PathWebAppSourceCode = "tree/release/{0}/WebApp";
        const string PathWebAppDocumentation = "blob/release/{0}/com.unity.renderstreaming/Documentation~/en/webapp.md";


        static string GetWebAppURL(string version)
        {
#if UNITY_EDITOR_WIN
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForWin, version));
#elif UNITY_EDITOR_OSX
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForMac, version));
#elif UNITY_EDITOR_LINUX
            var url = System.IO.Path.Combine(URLRoot, string.Format(PathWebAppForLinux, version));
#endif
            return url;
        }

        public static string GetURLDocumentation(string version)
        {
            var pattern = @"\d+.\d+.\d+";
            var result = System.Text.RegularExpressions.Regex.Match(version, pattern);
            return System.IO.Path.Combine(URLRoot, string.Format(PathWebAppDocumentation, result.Value));
        }

        public static string GetURLSourceCode(string version)
        {
            var pattern = @"\d+.\d+.\d+";
            var result = System.Text.RegularExpressions.Regex.Match(version, pattern);
            return System.IO.Path.Combine(URLRoot, string.Format(PathWebAppSourceCode, result.Value));
        }

        public static void DownloadCurrentVersionWebApp(string dstPath) {
            GetPackageVersion("com.unity.renderstreaming", (version) => {
                DownloadWebApp(version, dstPath);
            });
        }

        public static void DownloadWebApp(string version, string dstPath)
        {
            var url = GetWebAppURL(version);
            var client = new WebClient();
            var filename = System.IO.Path.GetFileName(url);
            var tmpFilePath = System.IO.Path.Combine(Application.temporaryCachePath, filename);

            client.DownloadFileCompleted += (sender, e) =>
            {
                EditorUtility.ClearProgressBar();
                if (e.Error != null) {
                    //Try downloading using the latest known version to work.
                    if (version != LatestKnownVersion) {
                        DownloadWebApp(LatestKnownVersion, dstPath);
                    } else {
                        Debug.LogError("Failed downloading webserver from: " + url + " . Error: " + e.Error.ToString());
                    }
                    return;
                }

                if (!System.IO.File.Exists(tmpFilePath))
                {
                    Debug.LogErrorFormat("Download failed. url:{0}", url);
                    return;
                }

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

        }

        public static void GetPackageVersion(string packageName, System.Action<string> callback)
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
    }
}

