using System;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Unity.RenderStreaming.RuntimeTest
{
    public class MockLogger : ILogger
    {
        public void LogFormat(LogType logType, Object context, string format, params object[] args)
        {
            throw new NotImplementedException();
        }

        public void LogException(Exception exception, Object context)
        {
            throw new NotImplementedException();
        }

        public bool IsLogTypeAllowed(LogType logType)
        {
            throw new NotImplementedException();
        }

        public void Log(LogType logType, object message)
        {
            throw new NotImplementedException();
        }

        public void Log(LogType logType, object message, Object context)
        {
            throw new NotImplementedException();
        }

        public void Log(LogType logType, string tag, object message)
        {
            throw new NotImplementedException();
        }

        public void Log(LogType logType, string tag, object message, Object context)
        {
            throw new NotImplementedException();
        }

        public void Log(object message)
        {
            throw new NotImplementedException();
        }

        public void Log(string tag, object message)
        {
            throw new NotImplementedException();
        }

        public void Log(string tag, object message, Object context)
        {
            throw new NotImplementedException();
        }

        public void LogWarning(string tag, object message)
        {
            throw new NotImplementedException();
        }

        public void LogWarning(string tag, object message, Object context)
        {
            throw new NotImplementedException();
        }

        public void LogError(string tag, object message)
        {
            throw new NotImplementedException();
        }

        public void LogError(string tag, object message, Object context)
        {
            throw new NotImplementedException();
        }

        public void LogFormat(LogType logType, string format, params object[] args)
        {
            throw new NotImplementedException();
        }

        public void LogException(Exception exception)
        {
            throw new NotImplementedException();
        }

        public ILogHandler logHandler { get; set; }
        public bool logEnabled { get; set; }
        public LogType filterLogType { get; set; }
    }
}
