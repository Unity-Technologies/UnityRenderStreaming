using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    ///
    /// </summary>
    public interface ISignalingSettingEditor
    {
        VisualElement ExtendInspectorGUI();
        void SetSignalingSettings(SignalingSettings settings);
    }

    // ToDo: implement HttpSignalingSettings
    // [CustomSignalingSettingsEditor(typeof(HttpSignalingSettings))]
    // internal class HttpSignalingSettingsEditor : ISignalingSettingEditor
    // {
    //     public VisualElement ExtendInspectorGUI()
    //     {
    //         return new TextField("Interval");
    //     }

    //     public void SetSignalingSettings(SignalingSettings settings)
    //     {
    //         throw new System.NotImplementedException();
    //     }
    // }
}
