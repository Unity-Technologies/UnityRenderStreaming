using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEditor.UIElements;
using UnityEditorInternal;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(SignalingSettingsAttribute))]
    class SignalingSettingsDrawer : PropertyDrawer
    {
        private VisualElement editorGUI;
        private ISignalingSettingEditor editor;
        private Dictionary<Type, SignalingSettings> table = new Dictionary<Type, SignalingSettings>();

        public override VisualElement CreatePropertyGUI(SerializedProperty property)
        {
            editor = CreateEditor(property);
            var root = new VisualElement();
            editorGUI = editor.CreateInspectorGUI(property);

            root.Add(CreatePopUpSignalingType(property, "Signaling Type"));
            root.Add(editorGUI);
            return root;
        }

        ISignalingSettingEditor CreateEditor(SerializedProperty property)
        {
            var handler = property.serializedObject.targetObject as RenderStreamingHandler;
            var settings = handler.GetSignalingSettings();
            var type = CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(settings.GetType());
            return Activator.CreateInstance(type) as ISignalingSettingEditor;
        }

        VisualElement CreatePopUpSignalingType(SerializedProperty property, string label)
        {
            var handler = property.serializedObject.targetObject as RenderStreamingHandler;
            var settings = handler.GetSignalingSettings();
            var defaultValue = CustomSignalingSettingsEditor.FindLabelByInspectedType(settings.GetType());
            var choices = CustomSignalingSettingsEditor.Labels().ToList();
            var element = new PopupField<string>(label: label, choices: choices, defaultValue: defaultValue);
            element.RegisterValueChangedCallback(e => OnChangedValue(e, property));
            return element;
        }

        void OnChangedValue(ChangeEvent<string> e, SerializedProperty property)
        {
            var handler = property.serializedObject.targetObject as RenderStreamingHandler;
            if (handler == null)
                return;

            // cache current settings.
            var settings = handler.GetSignalingSettings();
            var type = settings.GetType();
            table[type] = settings;

            var label = e.newValue;
            var inspectedType = CustomSignalingSettingsEditor.FindInspectedTypeByLabel(label);
            if (!table.ContainsKey(inspectedType))
            {
                var newSettings = Activator.CreateInstance(inspectedType) as SignalingSettings;
                table.Add(inspectedType, newSettings);
            }
            property.managedReferenceValue = table[inspectedType];

            var inspectorType = CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(inspectedType);
            var editor = Activator.CreateInstance(inspectorType) as ISignalingSettingEditor;
            var newValue = editor.CreateInspectorGUI(property);
            ReplaceElement(editorGUI, newValue);
            editorGUI = newValue;

            // bind new serializedObject.
            editorGUI.Bind(property.serializedObject);
        }

        static void ReplaceElement(VisualElement oldValue, VisualElement newValue)
        {
            var root = oldValue.parent;
            var index = root.IndexOf(oldValue);
            root.Remove(oldValue);
            root.Insert(index, newValue);
        }
    }

    /// <summary>
    ///
    /// </summary>
    public interface ISignalingSettingEditor
    {
        string Label { get; }
        VisualElement CreateInspectorGUI(SerializedProperty property);
        void SetSignalingSettings(SignalingSettings settings);
    }

    [CustomSignalingSettingsEditor(typeof(HttpSignalingSettings), "HTTP Polling")]
    internal class HttpSignalingSettingsEditor : ISignalingSettingEditor
    {
        public string Label => "HTTP Polling";

        public VisualElement CreateInspectorGUI(SerializedProperty property)
        {
            VisualElement root = new VisualElement();
            root.Add(new PropertyField(property.FindPropertyRelative("urlSignaling"), "URL"));
            root.Add(new PropertyField(property.FindPropertyRelative("iceServers"), "ICE Servers"));
            root.Add(new PropertyField(property.FindPropertyRelative("interval"), "Polling Interval"));
            return root;
        }

        public void SetSignalingSettings(SignalingSettings settings)
        {
            throw new System.NotImplementedException();
        }
    }

    [CustomSignalingSettingsEditor(typeof(WebSocketSignalingSettings), "WebSocket")]
    internal class WebSocketSignalingSettingsEditor : ISignalingSettingEditor
    {
        public string Label => "WebSocket";

        public VisualElement CreateInspectorGUI(SerializedProperty property)
        {
            VisualElement root = new VisualElement();
            root.Add(new PropertyField(property.FindPropertyRelative("urlSignaling"), "URL"));
            root.Add(new PropertyField(property.FindPropertyRelative("iceServers"), "ICE Servers"));
            return root;
        }

        public void SetSignalingSettings(SignalingSettings settings)
        {
            throw new System.NotImplementedException();
        }
    }

    [CustomSignalingSettingsEditor(typeof(FurioosSignalingSettings), "Furioos")]
    internal class FurioosSignalingSettingsEditor : ISignalingSettingEditor
    {
        public string Label => "Furioos";

        public VisualElement CreateInspectorGUI(SerializedProperty property)
        {
            return new TextField("Interval");
        }

        public void SetSignalingSettings(SignalingSettings settings)
        {
            throw new System.NotImplementedException();
        }
    }
}
