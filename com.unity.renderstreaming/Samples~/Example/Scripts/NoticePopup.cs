using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    internal class NoticePopup : MonoBehaviour
    {
        // Display only once regardless of the scene
        private static bool popupAlreadyShownOnce = false;

        [SerializeField] private GameObject panel;
        [SerializeField] private Button turnOffButton;
        [SerializeField] private Button ignoreButton;

        private void Awake()
        {
            if (!RenderStreaming.AutomaticStreaming || popupAlreadyShownOnce)
            {
                return;
            }

            turnOffButton.onClick.AddListener(() =>
            {
                RenderStreaming.AutomaticStreaming = false;
                panel.SetActive(false);
            });
            ignoreButton.onClick.AddListener(() =>
            {
                panel.SetActive(false);
            });

            panel.SetActive(true);
            popupAlreadyShownOnce = true;
        }
    }
}
