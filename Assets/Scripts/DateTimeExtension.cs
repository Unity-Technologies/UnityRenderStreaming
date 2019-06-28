using System;

namespace Unity.RenderStreaming
{
    static class DateTimeExtension
    {
        private static readonly long DatetimeMinTimeTicks =
           (new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).Ticks;

        /// <summary>
        /// It returns Javascript format timestamp
        /// </summary>
        /// <param name="dt"></param>
        /// <returns></returns>
        public static long ToJsMilliseconds(this DateTime dt)
        {
            return (long)((dt.ToUniversalTime().Ticks - DatetimeMinTimeTicks) / 10000);
        }

        public static DateTime ParseHttpDate(string text)
        {
            return DateTime.ParseExact(text,
                                "ddd, dd MMM yyyy HH:mm:ss Z",
                                System.Globalization.CultureInfo.InvariantCulture);
        }
    }
}
