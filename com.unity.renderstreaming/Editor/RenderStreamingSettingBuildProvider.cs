using System.Linq;
using UnityEditor;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;

namespace Unity.RenderStreaming.Editor
{
    internal class RenderStreamingSettingBuildProvider : IPreprocessBuildWithReport, IPostprocessBuildWithReport
    {
        public int callbackOrder => 0;

        public void OnPreprocessBuild(BuildReport report)
        {
            if (RenderStreaming.s_settings == null)
                return;

            // If we operate on temporary object instead of input setting asset,
            // adding temporary asset would result in preloadedAssets containing null object "{fileID: 0}".
            // Hence we ignore adding temporary objects to preloaded assets.
            if (!EditorUtility.IsPersistent(RenderStreaming.s_settings))
                return;

            // Add InputSettings object assets, if it's not in there already.
            var preloadedAssets = PlayerSettings.GetPreloadedAssets();
            if (!preloadedAssets.Contains(RenderStreaming.s_settings))
            {
                ArrayHelpers.Append(ref preloadedAssets, RenderStreaming.s_settings);
                PlayerSettings.SetPreloadedAssets(preloadedAssets);
            }
        }

        public void OnPostprocessBuild(BuildReport report)
        {
            // Revert back to original state by removing all input settings from preloaded assets.
            var preloadedAssets = PlayerSettings.GetPreloadedAssets();
            while (preloadedAssets != null && preloadedAssets.Length > 0)
            {
                var target = preloadedAssets.FirstOrDefault(x => x is RenderStreamingSettings);
                var index = ArrayHelpers.IndexOf(preloadedAssets, target);
                if (index != -1)
                {
                    ArrayHelpers.EraseAt(ref preloadedAssets, index);
                    PlayerSettings.SetPreloadedAssets(preloadedAssets);
                }
                else
                    break;
            }
        }
    }
}
