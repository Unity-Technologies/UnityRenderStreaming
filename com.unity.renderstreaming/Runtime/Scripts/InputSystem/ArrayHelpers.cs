// note:: This script is using code snippets in InputSystem.
// https://github.com/Unity-Technologies/InputSystem/blob/develop/Packages/com.unity.inputsystem/InputSystem/Utilities/ArrayHelpers.cs
// todo(kazuki):: This script should be moved into the WebRTC package.
// #if UNITY_WEBRTC_ENABLE_INPUT_SYSTEM
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

// namespace Unity.WebRTC.InputSystem
namespace Unity.RenderStreaming
{
    internal static class ArrayHelpers
    {
        public static int LengthSafe<TValue>(this TValue[] array)
        {
            if (array == null)
                return 0;
            return array.Length;
        }

        public static int Append<TValue>(ref TValue[] array, TValue value)
        {
            if (array == null)
            {
                array = new TValue[1];
                array[0] = value;
                return 0;
            }

            var length = array.Length;
            Array.Resize(ref array, length + 1);
            array[length] = value;
            return length;
        }

        public static int Append<TValue>(ref TValue[] array, IEnumerable<TValue> values)
        {
            if (array == null)
            {
                array = values.ToArray();
                return 0;
            }

            var oldLength = array.Length;
            var valueCount = values.Count();

            Array.Resize(ref array, oldLength + valueCount);

            var index = oldLength;
            foreach (var value in values)
                array[index++] = value;

            return oldLength;
        }

        public static int IndexOf<TValue>(TValue[] array, TValue value, int startIndex = 0, int count = -1)
        {
            if (array == null)
                return -1;

            if (count < 0)
                count = array.Length - startIndex;
            var comparer = EqualityComparer<TValue>.Default;
            for (var i = startIndex; i < startIndex + count; ++i)
                if (comparer.Equals(array[i], value))
                    return i;

            return -1;
        }

        public static bool Erase<TValue>(ref TValue[] array, TValue value)
        {
            var index = IndexOf(array, value);
            if (index != -1)
            {
                EraseAt(ref array, index);
                return true;
            }
            return false;
        }

        public static void EraseAt<TValue>(ref TValue[] array, int index)
        {
            Debug.Assert(array != null);
            Debug.Assert(index >= 0 && index < array.Length);

            var length = array.Length;
            if (index == 0 && length == 1)
            {
                array = null;
                return;
            }

            if (index < length - 1)
                Array.Copy(array, index + 1, array, index, length - index - 1);

            Array.Resize(ref array, length - 1);
        }

        public static void PutAtIfNotSet<TValue>(ref TValue[] array, int index, Func<TValue> valueFn)
        {
            if (array.LengthSafe() < index + 1)
                Array.Resize(ref array, index + 1);

            if (EqualityComparer<TValue>.Default.Equals(array[index], default(TValue)))
                array[index] = valueFn();
        }
    }
}
// #endif
