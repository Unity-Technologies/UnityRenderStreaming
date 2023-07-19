using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.SceneManagement;

namespace Unity.RenderStreaming.Samples
{
    internal class BackButton : MonoBehaviour
    {
        [SerializeField]
        GameObject m_BackButton;
        public GameObject backButton
        {
            get => m_BackButton;
            set => m_BackButton = value;
        }

        void Start()
        {
            if (Application.CanStreamedLevelBeLoaded("Menu"))
            {
                m_BackButton.SetActive(true);
            }
        }

        void Update()
        {
            if (Keyboard.current != null && Keyboard.current[Key.Escape].wasPressedThisFrame)
            {
                BackButtonPressed();
            }
        }

        public void BackButtonPressed()
        {
            if (Application.CanStreamedLevelBeLoaded("Menu"))
            {
                SceneManager.LoadScene("Menu", LoadSceneMode.Single);
            }
        }
    }
}
