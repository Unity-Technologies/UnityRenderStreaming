
using System;
using System.Collections.Generic;
using System.IO;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [Serializable]
    internal struct CommandLineInfo
    {
        public string signalingType;
        public string signalingUrl;
        public RTCIceServer[] iceServers;
        public string pollingInterval;
    }

    static class CommandLineParser
    {
        internal static readonly StringArgument SignalingUrl = new StringArgument("-signalingUrl");
        internal static readonly StringArgument SignalingType = new StringArgument("-signalingType");
        internal static readonly StringArrayArgument IceServerUrls = new StringArrayArgument("-iceServerUrl");
        internal static readonly StringArgument IceServerUsername = new StringArgument("-iceServerUsername");
        internal static readonly StringArgument IceServerCredential = new StringArgument("-iceServerCredential");
        internal static readonly EnumArgument<IceCredentialType> IceServerCredentialType = new EnumArgument<IceCredentialType>("-iceServerCredentialType");
        internal static readonly IntArgument PollingInterval = new IntArgument("-pollingInterval");
        internal static readonly JsonFileArgument<CommandLineInfo> ImportJson = new JsonFileArgument<CommandLineInfo>("-importJson");

        static readonly List<IArgument> options = new List<IArgument>() { SignalingUrl, SignalingType, IceServerUrls, IceServerUsername, IceServerCredential, IceServerCredentialType, PollingInterval, ImportJson };

        internal delegate bool TryParseDelegate<T>(string[] arguments, string argumentName, out T result);

        internal interface IArgument
        {
            bool TryParse(string[] arguments);
        }

        internal abstract class BaseArgument<T> : IArgument
        {
            /// <summary>
            /// A switch that will either retrieve the argument using the resolver, or use the readonly
            /// argument name string set on construction.
            /// </summary>
            public string ArgumentName { get; }

            /// <summary>
            /// 
            /// </summary>
            public bool Defined => m_defined;

            /// <summary>
            /// 
            /// </summary>
            public readonly bool Required;


            /// <summary>
            /// 
            /// </summary>
            public T Value => m_value;

            protected bool m_defined;
            protected T m_value;
            readonly TryParseDelegate<T> m_parser;

            protected abstract bool DefaultParser(string[] arguments, string argumentName, out T parsedResult);

            public bool TryParse(string[] arguments)
            {
                m_defined =
                    m_parser != null &&
                    m_parser(arguments, ArgumentName, out m_value);
                return m_defined;
            }

            internal BaseArgument(string argumentName, bool required = false)
            {
                Required = required;
                ArgumentName = argumentName;
                m_parser = DefaultParser;
            }

            internal BaseArgument(string argumentName, TryParseDelegate<T> tryParseDelegate, bool required = false)
            {
                Required = required;
                ArgumentName = argumentName;
                m_parser = tryParseDelegate;
            }
        }

        internal class StringArgument : BaseArgument<string>
        {
            protected override bool DefaultParser(string[] arguments, string argumentName, out string parsedResult) => TryParseStringArgument(arguments, argumentName, out parsedResult, Required);

            internal StringArgument(string argumentName, bool required = false) : base(argumentName, required) { }
            internal StringArgument(string argumentName, TryParseDelegate<string> tryParse, bool required = false) : base(argumentName, tryParse, required) { }

            public static implicit operator string(StringArgument argument) => !argument.Defined ? null : argument.Value;
        }

        internal class EnumArgument<T> : BaseArgument<T?> where T : struct
        {
            protected override bool DefaultParser(string[] arguments, string argumentName, out T? parsedResult) => TryParseEnumArgument(arguments, argumentName, out parsedResult, Required);

            internal EnumArgument(string argumentName, bool required = false) : base(argumentName, required) { }

            public static implicit operator T?(EnumArgument<T> argument) => !argument.Defined ? null : argument.Value;
        }

        internal class StringArrayArgument : BaseArgument<string[]>
        {
            protected override bool DefaultParser(string[] arguments, string argumentName, out string[] parsedResult) => TryParseStringArrayArgument(arguments, argumentName, out parsedResult, Required);

            internal StringArrayArgument(string argumentName, bool required = false) : base(argumentName, required) { }

            public static implicit operator string[](StringArrayArgument argument) => !argument.Defined ? null : argument.Value;
        }


        internal class IntArgument : BaseArgument<int?>
        {
            protected override bool DefaultParser(string[] arguments, string argumentName, out int? parsedResult) => TryParseIntArgument(arguments, argumentName, out parsedResult, Required);

            internal IntArgument(string argumentName, bool required = false) : base(argumentName, required) { }

            public static implicit operator int?(IntArgument argument) => !argument.Defined ? null : argument.Value;
        }

        internal class JsonFileArgument<T> : BaseArgument<T?> where T : struct
        {
            protected override bool DefaultParser(string[] arguments, string argumentName, out T? parsedResult) => TryParseJsonFileArgument(arguments, argumentName, out parsedResult, Required);

            internal JsonFileArgument(string argumentName, bool required = false) : base(argumentName, required) { }

            public static implicit operator T?(JsonFileArgument<T> argument) => !argument.Defined ? null : argument.Value;
        }

        static bool TryParseStringArgument(string[] arguments, string argumentName, out string argumentValue, bool required = false)
        {
            var startIndex = System.Array.FindIndex(arguments, x => x == argumentName);
            if (startIndex < 0)
            {
                argumentValue = null;
                return !required;
            }

            if (startIndex + 1 >= arguments.Length)
            {
                argumentValue = null;
                return false;
            }

            argumentValue = arguments[startIndex + 1];
            return !string.IsNullOrEmpty(argumentValue);
        }

        static bool TryParseEnumArgument<T>(string[] arguments, string argumentName, out T? argumentValue,
            bool required = false) where T : struct
        {
            bool result = TryParseStringArgument(arguments, argumentName, out string value, required);
            if (result && !string.IsNullOrEmpty(value))
            {
                if (Enum.TryParse(value, true, out T enumValue))
                {
                    argumentValue = enumValue;
                    return true;
                }
                result = false;
            }
            argumentValue = null;
            return result;
        }


        static bool TryParseIntArgument(string[] arguments, string argumentName, out int? argumentValue, bool required = false)
        {
            var startIndex = System.Array.FindIndex(arguments, x => x == argumentName);
            if (startIndex < 0)
            {
                argumentValue = null;
                return !required;
            }

            if (startIndex + 1 >= arguments.Length)
            {
                argumentValue = null;
                return false;
            }

            if (!int.TryParse(arguments[startIndex + 1], out var result))
            {
                argumentValue = null;
                return false;
            }
            argumentValue = result;
            return true;
        }

        static bool TryParseStringArrayArgument(string[] arguments, string argumentName, out string[] argumentValue, bool required = false)
        {
            List<string> list = new List<string>();

            for (int i = 0; i < arguments.Length;)
            {
                var startIndex = Array.FindIndex(arguments, i, x => x == argumentName);
                if (startIndex < 0)
                    break;
                if (startIndex + 1 >= arguments.Length)
                    break;
                list.Add(arguments[startIndex + 1]);
                i = startIndex + 2;
            }
            if (list.Count == 0 && required)
            {
                argumentValue = null;
                return false;
            }
            argumentValue = list.ToArray();
            return true;
        }

        static bool TryParseJsonFileArgument<T>(string[] arguments, string argumentName, out T? argumentValue,
            bool required = false) where T : struct
        {
            bool result = TryParseFilePathArgument(arguments, argumentName, out string value);
            if (result && !string.IsNullOrEmpty(value))
            {
                string text = File.ReadAllText(value);
                try
                {
                    argumentValue = JsonUtility.FromJson<T>(text);
                    return true;
                }
                catch (Exception)
                {
                    result = false;
                }
            }
            else if (required)
                result = false;
            argumentValue = null;
            return result;
        }

        static bool TryParseFilePathArgument(string[] arguments, string argumentName, out string argumentValue)
        {
            bool ret = TryParseStringArgument(arguments, argumentName, out string value);
            if (!ret)
            {
                argumentValue = null;
                return false;
            }
            if (string.IsNullOrEmpty(value))
            {
                argumentValue = null;
                return true;
            }

            if (!File.Exists(value))
            {
                argumentValue = null;
                return false;
            }
            argumentValue = value;
            return true;
        }

        internal static bool TryParse(string[] arguments)
        {
            foreach (var option in options)
            {
                if (!option.TryParse(arguments))
                    return false;
            }
            return true;
        }
    }
}
