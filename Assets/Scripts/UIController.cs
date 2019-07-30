using UnityEngine;
using UnityEngine.InputSystem;
using TMPro;

namespace Unity.RenderStreaming
{
    public class UIController : MonoBehaviour
    {
        [SerializeField] TextMeshProUGUI text = null;
        [SerializeField] CanvasGroup canvasGroup = null;
        [SerializeField] private AnimationCurve transitionCurve =
            new AnimationCurve(
                new Keyframe(0.75f, 1f, 0f, 0f),
                new Keyframe(1f, 0f, 0f, 0f));

        private float timeTransition;

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
        }

        void OnTextInput(char c)
        {
            canvasGroup.alpha = 1f;
            text.text = c.ToString();
            timeTransition = 0;
        }
    }
}
