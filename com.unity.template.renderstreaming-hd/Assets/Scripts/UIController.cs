using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using TMPro;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(RectTransform))]
    public class UIController : MonoBehaviour
    {
        [SerializeField] TextMeshProUGUI text = null;
        [SerializeField] CanvasGroup canvasGroup = null;
        [SerializeField] Image pointer = null;
        [SerializeField] private AnimationCurve transitionCurve =
            new AnimationCurve(
                new Keyframe(0.75f, 1f, 0f, 0f),
                new Keyframe(1f, 0f, 0f, 0f));
        [SerializeField] RenderStreaming m_renderStreaming = null;

        private float timeTransition = 0f;
        private Color transparentColor = new Color(0, 0, 0, 0);
        private RectTransform m_rectTransform = null;

        void Start()
        {
            m_rectTransform = GetComponent<RectTransform>();
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

            bool pointerFromMouse
                =  HighlightPointerFromMouse(RemoteInput.RemoteMouse, m_renderStreaming.GetStreamingSize())
                || HighlightPointerFromMouse(Mouse.current, new Vector2Int(Screen.width, Screen.height));
            if (pointerFromMouse)
                return;


            if (UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches.Count > 0)
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

//----------------------------------------------------------------------------------------------------------------------
        bool HighlightPointerFromMouse(Mouse m, Vector2Int screenSize)
        {
            if (!Screen.safeArea.Contains(m.position.ReadValue()))
                return false;

            if (!m.leftButton.isPressed && !m.rightButton.isPressed)
                return false;
            Vector2 mousePos = m.position.ReadValue();
            Vector2 pos = mousePos / screenSize * new Vector2(m_rectTransform.rect.width, m_rectTransform.rect.height);

            pointer.rectTransform.anchoredPosition = pos;
            pointer.color = Color.red;
            return true;
        }

        void OnTextInput(char c)
        {
            canvasGroup.alpha = 1f;
            text.text = c.ToString();
            timeTransition = 0;
        }
    }
}
