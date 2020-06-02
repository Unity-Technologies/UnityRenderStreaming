using System.Linq;
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

        private float timeTransition = 0f;
        private Color transparentColor = new Color(0, 0, 0, 0);
        private RectTransform m_rectTransform = null;

        private Gamepad m_gamepad;
        private Keyboard m_keyboard;
        private Mouse m_mouse;
        private Touchscreen m_screen;

        public void SetInput(IInput input)
        {
            m_mouse = input.RemoteMouse;
            m_keyboard = input.RemoteKeyboard;
            m_screen = input.RemoteTouchscreen;
            m_gamepad = input.RemoteGamepad;

            m_keyboard.onTextInput += OnTextInput;
        }

        void Start()
        {
            m_rectTransform = GetComponent<RectTransform>();
            canvasGroup.alpha = 0;
            text.text = string.Empty;
        }

        void FixedUpdate()
        {
            if (!m_keyboard.anyKey.isPressed && !Mathf.Approximately(canvasGroup.alpha, 0f))
            {
                timeTransition += Time.deltaTime;
                canvasGroup.alpha = transitionCurve.Evaluate(timeTransition);
                if (Mathf.Approximately(canvasGroup.alpha, 0f))
                {
                    text.text = string.Empty;
                }
            }

            bool pointerFromMouse
                =  HighlightPointerFromMouse(m_mouse, new Vector2Int(Screen.width, Screen.height));
            if (pointerFromMouse)
                return;

            var touches = m_screen.GetTouches();

            if (touches.Count() > 0)
            {
                var position = Vector2.zero;
                var count = touches.Count();
                var activeTouches = touches.ToArray();

                for (var i = 0; i < count; i++)
                {
                    position += activeTouches[i].screenPosition;
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
