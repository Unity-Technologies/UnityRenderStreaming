using System;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    ///
    /// </summary>
    public class CustomSignalingSettingsEditor : Attribute
    {
        private static readonly TypeCache.TypeCollection customInspectorType =
            TypeCache.GetTypesWithAttribute<CustomSignalingSettingsEditor>();

        private readonly Type inspectedType;

        public CustomSignalingSettingsEditor(Type inspectedType)
        {
            if (inspectedType == null)
                Debug.LogError("Failed to load CustomEditor inspected type");
            this.inspectedType = inspectedType;
        }

        internal static Type FindCustomInspectorTypeByType(Type inspectorType)
        {
            foreach (var type in customInspectorType)
            {
                foreach (CustomSignalingSettingsEditor custom in
                         type.GetCustomAttributes(typeof(CustomSignalingSettingsEditor), false))
                {
                    if (custom.inspectedType == inspectorType)
                    {
                        return type;
                    }
                }
            }

            return null;
        }
    }
}
