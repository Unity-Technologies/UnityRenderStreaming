using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using TMPro;

namespace Unity.RenderStreaming
{
    public class UIController : MonoBehaviour
    {
        [SerializeField] TextMeshProUGUI text = null;
        [SerializeField] CanvasGroup canvasGroup = null;
        [SerializeField] Image pointer = null;
        [SerializeField] private AnimationCurve transitionCurve =
            new AnimationCurve(
                new Keyframe(0.75f, 1f, 0f, 0f),
                new Keyframe(1f, 0f, 0f, 0f));

        private float timeTransition = 0f;
        private Color transparentColor = new Color(0, 0, 0, 0);

        void Start()
        {
            Keyboard.current.onTextInput += OnTextInput;
            canvasGroup.alpha = 0;
            text.text = string.Empty;
        }

        void FixedUpdate()
        {
            if (!Keyboard.current.anyKey.isPressed && !Mathf.Approximately(canvasGroup.alpha, 0f))
            {
                timeTransition += Time.deltaTime;
                canvasGroup.alpha = transitionCurve.Evaluate(timeTransition);
                if (Mathf.Approximately(canvasGroup.alpha, 0f))
                {
                    text.text = string.Empty;
                }
            }
            if (Mouse.current.leftButton.isPressed || Mouse.current.rightButton.isPressed)
            {
                var position = Mouse.current.position.ReadValue();
                pointer.rectTransform.anchoredPosition = position;
                pointer.color = Color.red;
            }
            else if (UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches.Count > 0)
            {
                var position = Vector2.zero;
                var count = UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches.Count;
                for (var i = 0; i < count; i++)
                {
                    position += UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches[i].screenPosition;
                }
                pointer.rectTransform.anchoredPosition = position / (float)count;
                pointer.color = Color.red;
            }
            else
            {
                pointer.color = transparentColor;
            }
        }

        void OnTextInput(char c)
        {
            canvasGroup.alpha = 1f;
            text.text = c.ToString();
            timeTransition = 0;
        }
    }
}
