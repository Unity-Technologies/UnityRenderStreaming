#if URS_USE_URS_EDITOR
using UnityEngine; //Debug
using UnityEditor; //AssetPostProcessor
using UnityEngine.Rendering; //GraphicsSettings

namespace Unity.RenderStreaming.Editor
{
    public class URPInitialSetupPostProcessor
    {
        [InitializeOnLoadMethod]
        static void OnLoad()
        {
            if (null != GraphicsSettings.renderPipelineAsset)
            {
                return;
            }

            var allAssetPaths = AssetDatabase.GetAllAssetPaths();

            foreach (var curAssetPath in allAssetPaths)
            {
                if (curAssetPath.EndsWith("UniversalRenderPipelineAsset.asset"))
                {
                    UnityEngine.Rendering.Universal.UniversalRenderPipelineAsset pipelineAsset =
                        AssetDatabase.LoadAssetAtPath<UnityEngine.Rendering.Universal.UniversalRenderPipelineAsset>(
                            curAssetPath);
                    GraphicsSettings.renderPipelineAsset = pipelineAsset;
                    PlayerSettings.colorSpace = ColorSpace.Linear;
                }
            }
        }
    }
}
#endif
