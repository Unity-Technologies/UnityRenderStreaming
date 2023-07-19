using System;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    ///
    /// </summary>
    public class CustomSignalingSettingsEditor : Attribute
    {
        private static readonly TypeCache.TypeCollection inspectorTypes =
            TypeCache.GetTypesWithAttribute<CustomSignalingSettingsEditor>();

        private readonly Type inspectedType;
        private readonly string label;

        /// <summary>
        ///
        /// </summary>
        /// <param name="inspectedType"></param>
        /// <param name="label"></param>
        public CustomSignalingSettingsEditor(Type inspectedType, string label)
        {
            if (inspectedType == null)
                RenderStreaming.Logger.Log(LogType.Error, "Failed to load CustomEditor inspected type");
            this.inspectedType = inspectedType;
            this.label = label;
        }

        internal static Type FindInspectorTypeByInspectedType(Type inspectedType)
        {
            foreach (var type in inspectorTypes)
            {
                foreach (CustomSignalingSettingsEditor custom in
                         type.GetCustomAttributes(typeof(CustomSignalingSettingsEditor), false))
                {
                    if (custom.inspectedType == inspectedType)
                    {
                        return type;
                    }
                }
            }
            return null;
        }

        internal static Type FindInspectedTypeByLabel(string label)
        {
            foreach (var type in inspectorTypes)
            {
                foreach (CustomSignalingSettingsEditor custom in
                         type.GetCustomAttributes(typeof(CustomSignalingSettingsEditor), false))
                {
                    if (custom.label == label)
                    {
                        return custom.inspectedType;
                    }
                }
            }
            return null;
        }

        internal static string FindLabelByInspectedType(Type inspectedType)
        {
            foreach (var type in inspectorTypes)
            {
                foreach (CustomSignalingSettingsEditor custom in
                         type.GetCustomAttributes(typeof(CustomSignalingSettingsEditor), false))
                {
                    if (custom.inspectedType == inspectedType)
                    {
                        return custom.label;
                    }
                }
            }
            return null;
        }

        internal static string FindLabelByInspectorType(Type inspectorType)
        {
            var attributes =
                inspectorType.GetCustomAttributes(typeof(CustomSignalingSettingsEditor), false);
            foreach (var attribute in attributes)
            {
                if (attribute is CustomSignalingSettingsEditor custom)
                    return custom.label;
            }
            return null;
        }

        internal static IEnumerable<string> Labels()
        {
            return inspectorTypes.Select(FindLabelByInspectorType);
        }
    }
}
