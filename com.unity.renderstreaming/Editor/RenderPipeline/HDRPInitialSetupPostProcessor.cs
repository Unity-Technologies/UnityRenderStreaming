#if URS_USE_HDRP_EDITOR
using UnityEngine; //Debug
using UnityEditor; //AssetPostProcessor
using UnityEngine.Rendering; //GraphicsSettings
#if UNITY_2019_3_OR_NEWER
using UnityEngine.Rendering.HighDefinition; //HDRenderPipelineAsset
#else
using UnityEngine.Experimental.Rendering.HDPipeline;    //HDRenderPipelineAsset
#endif

namespace Unity.RenderStreaming.Editor
{
    public class HDRPPostProcessor
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
                if (curAssetPath.EndsWith("HDRenderPipelineAsset.asset"))
                {
                    HDRenderPipelineAsset pipelineAsset =
                        AssetDatabase.LoadAssetAtPath<HDRenderPipelineAsset>(curAssetPath);
                    GraphicsSettings.renderPipelineAsset = pipelineAsset;
                    PlayerSettings.colorSpace = ColorSpace.Linear;
                }
            }
        }
    }
}
#endif
