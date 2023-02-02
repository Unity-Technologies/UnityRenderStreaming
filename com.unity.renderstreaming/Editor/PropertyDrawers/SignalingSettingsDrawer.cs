using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;
using UnityEditor.UIElements;
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
            var box = new Box();
            root.Add(box);
            editorGUI = editor.CreateInspectorGUI(property);
            box.Add(CreatePopUpSignalingType(property, "Signaling Type"));
            box.Add(editorGUI);
            return root;
        }

        ISignalingSettingEditor CreateEditor(SerializedProperty property)
        {
            var settings = fieldInfo.GetValue(property.serializedObject.targetObject) as SignalingSettings;
            var type = CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(settings.GetType());
            return Activator.CreateInstance(type) as ISignalingSettingEditor;
        }

        VisualElement CreatePopUpSignalingType(SerializedProperty property, string label)
        {
            var settings = fieldInfo.GetValue(property.serializedObject.targetObject) as SignalingSettings;
            var defaultValue = CustomSignalingSettingsEditor.FindLabelByInspectedType(settings.GetType());
            var choices = CustomSignalingSettingsEditor.Labels().ToList();
            var element = new PopupField<string>(label: label, choices: choices, defaultValue: defaultValue);
            element.RegisterValueChangedCallback(e => OnChangedValue(e, property));
            return element;
        }

        void OnChangedValue(ChangeEvent<string> e, SerializedProperty property)
        {
            if(!(fieldInfo.GetValue(property.serializedObject.targetObject) is SignalingSettings settings))
                return;

            // cache current settings.
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
            property.serializedObject.ApplyModifiedProperties();

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
        VisualElement CreateInspectorGUI(SerializedProperty property);
    }

    [CustomSignalingSettingsEditor(typeof(HttpSignalingSettings), "HTTP Polling")]
    internal class HttpSignalingSettingsEditor : ISignalingSettingEditor
    {
        public VisualElement CreateInspectorGUI(SerializedProperty property)
        {
            VisualElement root = new VisualElement();
            root.Add(new PropertyField(property.FindPropertyRelative("m_url"), "URL"));
            root.Add(new PropertyField(property.FindPropertyRelative("m_interval"), "Polling Interval"));
            root.Add(new PropertyField(property.FindPropertyRelative("m_iceServers"), "ICE Servers"));
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
        public VisualElement CreateInspectorGUI(SerializedProperty property)
        {
            VisualElement root = new VisualElement();
            root.Add(new PropertyField(property.FindPropertyRelative("m_url"), "URL"));
            root.Add(new PropertyField(property.FindPropertyRelative("m_iceServers"), "ICE Servers"));
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
        public VisualElement CreateInspectorGUI(SerializedProperty property)
        {
            VisualElement root = new VisualElement();
            root.Add(new PropertyField(property.FindPropertyRelative("m_url"), "URL"));
            root.Add(new PropertyField(property.FindPropertyRelative("m_iceServers"), "ICE Servers"));
            return root;
        }

        public void SetSignalingSettings(SignalingSettings settings)
        {
            throw new System.NotImplementedException();
        }
    }
}
