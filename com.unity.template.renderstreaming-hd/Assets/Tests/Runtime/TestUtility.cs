using System;
using System.Threading;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class TestUtility
    {
        const string FileNameWebAppForMac = "webserver_mac";
        const string FileNameWebAppForLinux = "webserver";
        const string FileNameWebAppForWin = "webserver.exe";

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
    }
}
