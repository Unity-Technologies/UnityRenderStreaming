using System;
using System.Threading;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    internal class TestUtility
    {
        const string FileNameWebAppForMac = "webserver_mac";
        const string FileNameWebAppForLinux = "webserver";
        const string FileNameWebAppForWin = "webserver.exe";
        public const int PortNumber = 8081;

        public static string GetFileName()
        {
            switch (Application.platform)
            {
                case RuntimePlatform.OSXEditor:
                case RuntimePlatform.OSXPlayer:
                    return FileNameWebAppForMac;
                case RuntimePlatform.WindowsPlayer:
                case RuntimePlatform.WindowsEditor:
                    return FileNameWebAppForWin;
                case RuntimePlatform.LinuxPlayer:
                case RuntimePlatform.LinuxEditor:
                    return FileNameWebAppForLinux;
                default:
                    throw new ArgumentOutOfRangeException($"this platform ({Application.platform} does not support.");
            }
        }

        public static bool Wait(Func<bool> condition, int millisecondsTimeout = 1000, int millisecondsInterval = 100)
        {
            if (millisecondsTimeout < millisecondsInterval)
            {
                throw new ArgumentException();
            }

            int time = 0;
            while (!condition() && millisecondsTimeout > time)
            {
                Thread.Sleep(millisecondsInterval);
                time += millisecondsInterval;
            }

            return millisecondsTimeout > time;
        }

        public static string GetWebAppLocationFromEnv()
        {
            var path = Environment.GetEnvironmentVariable("WEBAPP_PATH");

            if (!string.IsNullOrEmpty(path))
            {
                return path;
            }

            var args = Environment.GetCommandLineArgs();
            for (int i = 0; i < args.Length; i++)
            {
                if (args[i] == "-webapp-path")
                {
                    return args[i + 1];
                }
            }

            return null;
        }
    }
}
