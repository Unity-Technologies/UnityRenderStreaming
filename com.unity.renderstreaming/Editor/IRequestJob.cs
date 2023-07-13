using UnityEditor.PackageManager;           //StatusCode

namespace Unity.RenderStreaming.Editor
{

    internal interface IRequestJob
    {
        StatusCode Update();

    }


} //namespace Unity.RenderStreaming.Editor
