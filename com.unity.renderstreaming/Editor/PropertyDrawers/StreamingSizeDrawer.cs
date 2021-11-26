using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(StreamingSizeAttribute))]
    public class StreamingSizeDrawer : PropertyDrawer
    {
        readonly GUIContent[] streamingSizeText =
        {
            EditorGUIUtility.TrTextContent("640 x 480"), EditorGUIUtility.TrTextContent("1280 x 720"),
            EditorGUIUtility.TrTextContent("1600 x 1200"), EditorGUIUtility.TrTextContent("1920 x 1200"),
            EditorGUIUtility.TrTextContent("2560 x 1440"),
        };

        readonly Vector2Int[] streamingSizeValues =
        {
            new Vector2Int(640, 480), new Vector2Int(1280, 720), new Vector2Int(1600, 1200),
            new Vector2Int(1920, 1200), new Vector2Int(2560, 1440),
        };

        readonly GUIContent streamingSize =
            EditorGUIUtility.TrTextContent("Streaming Size", "Streaming size should match display aspect ratio.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            var selectIndex = 0;
            while (selectIndex < streamingSizeValues.Length && property.vector2IntValue != streamingSizeValues[selectIndex])
            {
                ++selectIndex;
            }
            selectIndex = EditorGUI.Popup(position, streamingSize, selectIndex, streamingSizeText);
            property.vector2IntValue = streamingSizeValues[selectIndex];
        }
    }
}
