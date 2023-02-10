using System;

namespace Unity.RenderStreaming.Editor
{
    public static class RenderStreamingEditor
    {
        /// <summary>
        ///
        /// </summary>
        /// <param name="settings"></param>
        /// <exception cref="ArgumentNullException"></exception>
        public static void SetRenderStreamingSettings(RenderStreamingSettings settings)
        {
            if (settings == null)
            {
                throw new ArgumentNullException(nameof(settings));
            }

            RenderStreaming.Settings = settings;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="settings"></param>
        /// <exception cref="ArgumentNullException"></exception>
        public static void SetSignalingSettings(SignalingSettings settings)
        {
            if (settings == null)
            {
                throw new ArgumentNullException(nameof(settings));
            }

            RenderStreaming.Settings.signalingSettings = settings;
            RenderStreaming.ApplySettings();
        }
    }
}
