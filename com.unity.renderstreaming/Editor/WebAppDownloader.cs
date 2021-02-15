using UnityEditor;
using UnityEngine;
using System.Net;

namespace Unity.RenderStreaming.Editor
{
    public static class WebAppDownloader
    {
        const string URLRoot = "https://github.com/Unity-Technologies/UnityRenderStreaming";
        const string LatestKnownVersion = "3.0.0-preview";

        // TODO::fix release process of webserver runtime.
        const string FileNameWebAppForMac = "webserver_mac";
        const string FileNameWebAppForLinux = "webserver";
        const string FileNameWebAppForWin = "webserver.exe";
        //

        const string PathWebAppSourceCode = "tree/release/{0}/WebApp";
        const string URLWebAppDocumentation = "https://docs.unity3d.com/Packages/com.unity.renderstreaming@{0}/manual/webapp.html";

        public static string GetFileName()
        {
#if UNITY_EDITOR_WIN
            return FileNameWebAppForWin;
#elif UNITY_EDITOR_OSX
            return FileNameWebAppForMac;
#elif UNITY_EDITOR_LINUX
            return FileNameWebAppForLinux;
#endif
        }

        public static string GetWebAppURL(string version)
        {
            if (version == null)
            {
                version = LatestKnownVersion;
            }
            string path = string.Format("releases/download/{0}", version);
            string fileName = GetFileName();
            return $"{URLRoot}/{path}/{fileName}";
        }

        public static string GetURLDocumentation(string version)
        {
            var pattern = @"\d+.\d+";
            var result = System.Text.RegularExpressions.Regex.Match(version, pattern);
            return string.Format(URLWebAppDocumentation, result.Value);
        }

        public static string GetURLSourceCode(string version)
        {
            var pattern = @"\d+.\d+.\d+";
            var result = System.Text.RegularExpressions.Regex.Match(version, pattern);
            return System.IO.Path.Combine(URLRoot, string.Format(PathWebAppSourceCode, result.Value));
        }

        public static void DownloadCurrentVersionWebApp(string dstPath, System.Action<bool> callback) {
            GetPackageVersion("com.unity.renderstreaming", (version) => {
                DownloadWebApp(version, dstPath, callback);
            });
        }

        public static void DownloadWebApp(string version, string dstPath, System.Action<bool> callback)
        {
            var url = GetWebAppURL(version);
            var client = new WebClient();
            var filename = System.IO.Path.GetFileName(url);
            var tmpFilePath = System.IO.Path.Combine(Application.temporaryCachePath, filename);

            if (string.IsNullOrEmpty(dstPath))
            {
                callback?.Invoke(false);
                return;
            }

            client.DownloadFileCompleted += (sender, e) =>
            {
                EditorUtility.ClearProgressBar();
                if (e.Error != null) {
                    //Try downloading using the latest known version to work.
                    if (version != LatestKnownVersion) {
                        DownloadWebApp(LatestKnownVersion, dstPath, callback);
                    } else {
                        Debug.LogError("Failed downloading webserver from: " + url + " . Error: " + e.Error.ToString());
                    }
                    callback?.Invoke(false);
                    return;
                }

                if (!System.IO.File.Exists(tmpFilePath))
                {
                    Debug.LogErrorFormat("Download failed. url:{0}", url);
                    callback?.Invoke(false);
                    return;
                }

                dstPath = System.IO.Path.Combine(dstPath, filename);
                if (System.IO.File.Exists(dstPath))
                {
                    System.IO.File.Delete(dstPath);
                }
                System.IO.File.Move(tmpFilePath, dstPath);
                EditorUtility.RevealInFinder(dstPath);
                callback?.Invoke(true);
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

