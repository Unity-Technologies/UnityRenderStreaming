using System;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Users;
using UnityEngine.InputSystem.Utilities;

namespace Unity.RenderStreaming.InputSystem.Editor
{
    [CustomEditor(typeof(InputReceiver))]
    internal class InputReceiverEditor : UnityEditor.Editor
    {
        public void OnEnable()
        {
            InputUser.onChange += OnUserChange;

            m_Local = serializedObject.FindProperty(DataChannelBase.LocalPropertyName);
            m_Label = serializedObject.FindProperty(DataChannelBase.LabelPropertyName);
            m_Actions = serializedObject.FindProperty(InputReceiver.ActionsPropertyName);
            m_ActionEvents = serializedObject.FindProperty(InputReceiver.ActionEventsPropertyName);
            m_DefaultActionMap = serializedObject.FindProperty(InputReceiver.DefaultActionMapPropertyName);
        }
        private void OnUserChange(InputUser user, InputUserChange change, InputDevice device)
        {
            Repaint();
        }

        public override void OnInspectorGUI()
        {
            EditorGUI.BeginChangeCheck();

            EditorGUILayout.PropertyField(m_Local);
            EditorGUILayout.PropertyField(m_Label);

            // Action config section.
            EditorGUI.BeginChangeCheck();
            EditorGUILayout.PropertyField(m_Actions);
            if (EditorGUI.EndChangeCheck() || !m_ActionAssetInitialized)
                OnActionAssetChange();
            ++EditorGUI.indentLevel;
            if (m_ActionMapOptions != null && m_ActionMapOptions.Length > 0)
            {
                // Default action map picker.

                var selected = EditorGUILayout.Popup(m_DefaultActionMapText, m_SelectedDefaultActionMap,
                    m_ActionMapOptions);
                if (selected != m_SelectedDefaultActionMap)
                {
                    if (selected == 0)
                    {
                        m_DefaultActionMap.stringValue = null;
                    }
                    else
                    {
                        // Use ID rather than name.
                        var asset = (InputActionAsset)m_Actions.objectReferenceValue;
                        var actionMap = asset.FindActionMap(m_ActionMapOptions[selected].text);
                        if (actionMap != null)
                            m_DefaultActionMap.stringValue = actionMap.id.ToString();
                    }
                    m_SelectedDefaultActionMap = selected;
                }
            }
            --EditorGUI.indentLevel;

            // Notifications/event section.
            m_EventsGroupUnfolded = EditorGUILayout.Foldout(m_EventsGroupUnfolded, m_EventsGroupText, toggleOnLabelClick: true);
            if (m_EventsGroupUnfolded)
            {
                // Action events. Group by action map.
                if (m_ActionNames != null)
                {
                    using (new EditorGUI.IndentLevelScope())
                    {
                        for (var n = 0; n < m_NumActionMaps; ++n)
                        {
                            m_ActionMapEventsUnfolded[n] = EditorGUILayout.Foldout(m_ActionMapEventsUnfolded[n],
                                m_ActionMapNames[n], toggleOnLabelClick: true);
                            using (new EditorGUI.IndentLevelScope())
                            {
                                if (m_ActionMapEventsUnfolded[n])
                                {
                                    for (var i = 0; i < m_ActionNames.Length; ++i)
                                    {
                                        if (m_ActionMapIndices[i] != n)
                                            continue;

                                        EditorGUILayout.PropertyField(m_ActionEvents.GetArrayElementAtIndex(i), m_ActionNames[i]);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (EditorGUI.EndChangeCheck())
                serializedObject.ApplyModifiedProperties();


            // Debug UI.
            if (EditorApplication.isPlaying)
                DoDebugUI();
        }

        private void DoDebugUI()
        {
            var playerInput = (InputReceiver)target;
            if (!playerInput.user.valid)
                return;

            ////TODO: show actions when they happen

            var user = playerInput.user.index.ToString();
            //var controlScheme = playerInput.user.controlScheme?.name;
            var devices = playerInput.user.pairedDevices.Select(_ => _.ToString()).OrderBy(x => x).ToArray();

            EditorGUILayout.Space();
            EditorGUILayout.LabelField(m_DebugText, EditorStyles.boldLabel);
            ++EditorGUI.indentLevel;
            EditorGUILayout.LabelField("User", $"#{user}");
            //EditorGUILayout.LabelField("Control Scheme", controlScheme);
            m_DevicesGroupUnfolded = EditorGUILayout.Foldout(
                m_DevicesGroupUnfolded, m_DevicesGroupText, toggleOnLabelClick: true);

            if (m_DevicesGroupUnfolded)
            {
                using (new EditorGUI.IndentLevelScope())
                {
                    foreach (var device in devices)
                    {
                        EditorGUILayout.LabelField(device.ToString());
                        // todo: InputDeviceDebuggerWindow is an internal class
                        // InputDeviceDebuggerWindow.CreateOrShowExisting(device);
                    }
                }
            }
            --EditorGUI.indentLevel;
        }

        private void OnActionAssetChange()
        {
            serializedObject.ApplyModifiedProperties();
            m_ActionAssetInitialized = true;

            var playerInput = (InputReceiver)target;
            var asset = (InputActionAsset)m_Actions.objectReferenceValue;
            if (asset == null)
            {
                m_ActionMapOptions = null;
                return;
            }

            var newActionNames = new List<GUIContent>();
            var newActionEvents = new List<PlayerInput.ActionEvent>();
            var newActionMapIndices = new List<int>();

            void AddEntry(InputAction action, PlayerInput.ActionEvent actionEvent)
            {
                newActionNames.Add(new GUIContent(action.name));
                newActionEvents.Add(actionEvent);

                var actionMapIndex = asset.actionMaps.IndexOfReference(action.actionMap);
                newActionMapIndices.Add(actionMapIndex);

                if (actionMapIndex >= m_NumActionMaps)
                    m_NumActionMaps = actionMapIndex + 1;

                ArrayHelpers.PutAtIfNotSet(ref m_ActionMapNames, actionMapIndex,
                    () => new GUIContent(action.actionMap.name));
            }

            // Bring over any action events that we already have and that are still in the asset.
            var oldActionEvents = playerInput.m_ActionEvents;
            if (oldActionEvents != null)
            {
                foreach (var entry in oldActionEvents)
                {
                    var guid = entry.actionId;
                    var action = asset.FindAction(guid);
                    if (action != null)
                        AddEntry(action, entry);
                }
            }

            // Add any new actions.
            foreach (var action in asset)
            {
                // Skip if it was already in there.
                if (oldActionEvents != null && oldActionEvents.Any(x => x.actionId == action.id.ToString()))
                    continue;

                ////FIXME: adds bindings to the name
                AddEntry(action, new PlayerInput.ActionEvent(action.id, action.ToString()));
            }

            m_ActionNames = newActionNames.ToArray();
            m_ActionMapIndices = newActionMapIndices.ToArray();
            Array.Resize(ref m_ActionMapEventsUnfolded, m_NumActionMaps);
            playerInput.m_ActionEvents = newActionEvents.ToArray();

            // Read out action maps.
            var selectedDefaultActionMap = !string.IsNullOrEmpty(playerInput.defaultActionMap)
                ? asset.FindActionMap(playerInput.defaultActionMap)
                : null;
            m_SelectedDefaultActionMap = asset.actionMaps.Count > 0 ? 1 : 0;
            var actionMaps = asset.actionMaps;
            m_ActionMapOptions = new GUIContent[actionMaps.Count + 1];
            m_ActionMapOptions[0] = new GUIContent(EditorGUIUtility.TrTextContent("<None>"));
            ////TODO: sort alphabetically
            for (var i = 0; i < actionMaps.Count; ++i)
            {
                var actionMap = actionMaps[i];
                m_ActionMapOptions[i + 1] = new GUIContent(actionMap.name);

                if (selectedDefaultActionMap != null && actionMap == selectedDefaultActionMap)
                    m_SelectedDefaultActionMap = i + 1;
            }
            if (m_SelectedDefaultActionMap <= 0)
                playerInput.defaultActionMap = null;
            else
                playerInput.defaultActionMap = m_ActionMapOptions[m_SelectedDefaultActionMap].text;

            serializedObject.Update();
        }

        [SerializeField] private bool m_EventsGroupUnfolded;
        [SerializeField] private bool m_DevicesGroupUnfolded;
        [SerializeField] private bool[] m_ActionMapEventsUnfolded;

        [NonSerialized]
        private readonly GUIContent m_EventsGroupText =
            EditorGUIUtility.TrTextContent("Events", "UnityEvents triggered by the PlayerInput component");
        [NonSerialized]
        private readonly GUIContent m_DefaultActionMapText =
            EditorGUIUtility.TrTextContent("Default Map", "Action map to enable by default. If not set, no actions will be enabled by default.");

        [NonSerialized]
        private readonly GUIContent m_DebugText = EditorGUIUtility.TrTextContent("Debug");
        [NonSerialized]
        private readonly GUIContent m_DevicesGroupText =
            EditorGUIUtility.TrTextContent("Paired Devices", "InputDevices paired by the PlayerInput component");

        [NonSerialized] private GUIContent[] m_ActionNames;
        [NonSerialized] private GUIContent[] m_ActionMapNames;
        [NonSerialized] private int[] m_ActionMapIndices;
        [NonSerialized] private int m_NumActionMaps;

        [NonSerialized] private SerializedProperty m_ActionEvents;
        [NonSerialized] private int m_SelectedDefaultActionMap;
        [NonSerialized] private GUIContent[] m_ActionMapOptions;

        [NonSerialized] private SerializedProperty m_Local;
        [NonSerialized] private SerializedProperty m_Label;
        [NonSerialized] private SerializedProperty m_Actions;
        [NonSerialized] private SerializedProperty m_DefaultActionMap;
        [NonSerialized] private bool m_ActionAssetInitialized;
    }
}
