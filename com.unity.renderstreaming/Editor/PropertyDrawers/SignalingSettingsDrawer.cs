using System;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(SignalingSettingsAttribute))]
    class SignalingSettingsDrawer : PropertyDrawer
    {
        private VisualElement editorGUI;
        private PopupField<string> popupFieldSignalingType;
        private ISignalingSettingEditor editor;
        private Dictionary<Type, SignalingSettings> table = new Dictionary<Type, SignalingSettings>();

        public override VisualElement CreatePropertyGUI(SerializedProperty property)
        {
            editor = CreateEditor(property);
            var root = new VisualElement();
            root.RegisterCallback<SerializedPropertyChangeEvent, SerializedProperty>(OnSignalingSettingsObjectChange, property);

            var box = new Box();
            root.Add(box);
            editorGUI = editor.CreateInspectorGUI(property);
            popupFieldSignalingType = CreatePopUpSignalingType(property, "Signaling Type");
            popupFieldSignalingType.RegisterValueChangedCallback(e => OnPopupFieldValueChange(e, property));
            box.Add(popupFieldSignalingType);
            box.Add(editorGUI);
            return root;
        }

        ISignalingSettingEditor CreateEditor(SerializedProperty property)
        {
            var settings = fieldInfo.GetValue(property.serializedObject.targetObject) as SignalingSettings;
            var type = CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(settings.GetType());
            return Activator.CreateInstance(type) as ISignalingSettingEditor;
        }

        PopupField<string> CreatePopUpSignalingType(SerializedProperty property, string label)
        {
            var settings = fieldInfo.GetValue(property.serializedObject.targetObject) as SignalingSettings;
            var defaultValue = CustomSignalingSettingsEditor.FindLabelByInspectedType(settings.GetType());
            var choices = CustomSignalingSettingsEditor.Labels().ToList();
            var field = new PopupField<string>(label: label, choices: choices, defaultValue: defaultValue);
            field.tooltip = "Choose the signaling type. \"WebSocket\" or \"HTTP Polling\".";
            return field;
        }

        static void ReplaceVisualElement(VisualElement oldValue, VisualElement newValue)
        {
            var root = oldValue.parent;
            var index = root.IndexOf(oldValue);
            root.Remove(oldValue);
            root.Insert(index, newValue);
        }

        void OnSignalingSettingsObjectChange(SerializedPropertyChangeEvent e, SerializedProperty property)
        {
            var settings = fieldInfo.GetValue(property.serializedObject.targetObject) as SignalingSettings;
            var label = CustomSignalingSettingsEditor.FindLabelByInspectedType(settings.GetType());

            if (popupFieldSignalingType.value == label)
                return;
            popupFieldSignalingType.value = label;
            RecreateEditorGUI(label, property);
        }

        void OnPopupFieldValueChange(ChangeEvent<string> e, SerializedProperty property)
        {
            if (!(fieldInfo.GetValue(property.serializedObject.targetObject) is SignalingSettings settings))
                return;

            // cache current settings.
            var type = settings.GetType();
            table[type] = settings;

            var label = e.newValue;
            RecreateEditorGUI(label, property);
        }

        void RecreateEditorGUI(string label, SerializedProperty property)
        {
            var inspectedType = CustomSignalingSettingsEditor.FindInspectedTypeByLabel(label);
            if (!table.ContainsKey(inspectedType))
            {
                var newSettings = Activator.CreateInstance(inspectedType) as SignalingSettings;
                table.Add(inspectedType, newSettings);
            }

            property.managedReferenceValue = table[inspectedType];
            property.serializedObject.ApplyModifiedProperties();

            var inspectorType = CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(inspectedType);
            editor = Activator.CreateInstance(inspectorType) as ISignalingSettingEditor;
            var newValue = editor.CreateInspectorGUI(property);

            // Unbind old element to serializedObject.
            editorGUI.Unbind();

            ReplaceVisualElement(editorGUI, newValue);
            editorGUI = newValue;

            // bind new element to serializedObject.
            editorGUI.Bind(property.serializedObject);
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
            root.Add(new PropertyField(property.FindPropertyRelative("m_interval"), "Polling Interval (msec)"));
            root.Add(new PropertyField(property.FindPropertyRelative("m_iceServers"), "ICE Servers"));
            return root;
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
    }
}
