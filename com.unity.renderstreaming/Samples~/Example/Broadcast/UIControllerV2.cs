using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    [RequireComponent(typeof(RectTransform))]
    class UIControllerV2 : MonoBehaviour
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
        private RectTransform m_rectTransform = null;
        private bool isSubscribing = false;


        public void SetDevice(InputDevice device, bool add = false)
        {
        }

        void Start()
        {
            m_rectTransform = GetComponent<RectTransform>();
            canvasGroup.alpha = 0;
            text.text = string.Empty;
        }

        private void FixedUpdate()
        {
            if (!Mathf.Approximately(canvasGroup.alpha, 0f))
            {
                timeTransition += Time.deltaTime;
                canvasGroup.alpha = transitionCurve.Evaluate(timeTransition);
            }
        }

        public void OnPressAnyKey(InputAction.CallbackContext context)
        {
            var keyboard = context.control.device as Keyboard;

            if (!isSubscribing)
            {
                keyboard.onTextInput += OnTextInput;
                isSubscribing = true;
            }
        }

        void OnTextInput(char c)
        {
            canvasGroup.alpha = 1f;
            text.text = c.ToString();
            timeTransition = 0;
        }

        public void OnPoint(InputAction.CallbackContext context)
        {
            if (m_rectTransform == null)
                return;
            var position = context.ReadValue<Vector2>();
            var screenSize = new Vector2Int(Screen.width, Screen.height);
            position = position / screenSize * new Vector2(m_rectTransform.rect.width, m_rectTransform.rect.height);
            pointer.rectTransform.anchoredPosition = position;
        }

        public void OnPress(InputAction.CallbackContext context)
        {
            var button = context.ReadValueAsButton();
            pointer.color = button ? Color.red : Color.clear;
        }
    }
}
