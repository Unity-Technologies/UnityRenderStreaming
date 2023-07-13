namespace Unity.RenderStreaming.Samples
{
    internal class SampleManager
    {
        static SampleManager s_instance;

        public static SampleManager Instance
        {
            get
            {
                if (s_instance == null)
                    s_instance = new SampleManager();
                return s_instance;
            }
        }

        public RenderStreamingSettings Settings
        {
            get
            {
                return m_settings;
            }
        }

        RenderStreamingSettings m_settings;

        public void Initialize()
        {
            if (m_settings == null)
                m_settings = new RenderStreamingSettings();
        }
    }
}
