using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    [RequireComponent(typeof(RectTransform))]
    class UIController : MonoBehaviour
    {
        [SerializeField] Text text;
        [SerializeField] CanvasGroup canvasGroup;
        [SerializeField] Image pointer;
        [SerializeField] GameObject noticeTouchControl;
        [SerializeField]
        private AnimationCurve transitionCurve =
            new AnimationCurve(
                new Keyframe(0.75f, 1f, 0f, 0f),
                new Keyframe(1f, 0f, 0f, 0f));

        private float timeTransition = 0f;
        private Color transparentColor = new Color(0, 0, 0, 0);
        private RectTransform m_rectTransform = null;

        private Keyboard m_keyboard;
        private Mouse m_mouse;
        private Touchscreen m_screen;

        public void SetDevice(InputDevice device, bool add = false)
        {
            switch (device)
            {
                case Mouse mouse:
                    m_mouse = add ? mouse : null;
                    return;
                case Keyboard keyboard:
                    m_keyboard = add ? keyboard : null;
                    if (add)
                        m_keyboard.onTextInput += OnTextInput;
                    return;
                case Touchscreen screen:
                    m_screen = add ? screen : null;
                    if (noticeTouchControl != null)
                    {
                        noticeTouchControl.SetActive(add);
                    }
                    return;
            }
        }

        void Start()
        {
            m_rectTransform = GetComponent<RectTransform>();
            canvasGroup.alpha = 0;
            text.text = string.Empty;
        }

        void FixedUpdate()
        {
            if (m_keyboard != null && !m_keyboard.anyKey.isPressed &&
                !Mathf.Approximately(canvasGroup.alpha, 0f))
            {
                timeTransition += Time.deltaTime;
                canvasGroup.alpha = transitionCurve.Evaluate(timeTransition);
                if (Mathf.Approximately(canvasGroup.alpha, 0f))
                {
                    text.text = string.Empty;
                }
            }

            bool pointerFromMouse = HighlightPointerFromMouse(
                m_mouse, new Vector2Int(Screen.width, Screen.height));
            if (pointerFromMouse)
                return;

            var touches = m_screen?.GetTouches();

            if (touches != null && touches.Count() > 0)
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
        bool HighlightPointerFromMouse(Mouse mouse, Vector2Int screenSize)
        {
            if (mouse == null)
                return false;
            if (!Screen.safeArea.Contains(mouse.position.ReadValue()))
                return false;

            if (!mouse.leftButton.isPressed && !mouse.rightButton.isPressed)
                return false;
            Vector2 mousePos = mouse.position.ReadValue();
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
