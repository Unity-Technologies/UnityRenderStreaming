using System.Linq;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class AudioCodecSelect : MonoBehaviour
    {
        [SerializeField] private Dropdown m_codecSelectDropdown;

        private int m_selectIndex = -1;
        public int SelectIndex => m_selectIndex;

        private void Awake()
        {
            m_codecSelectDropdown.onValueChanged.AddListener(OnChangeIndex);
        }

        public void ChangeInteractable(bool isOn)
        {
            m_codecSelectDropdown.interactable = isOn;
        }

        private void OnEnable()
        {
            var list = AvailableCodecsUtils.GetAvailableAudioCodecsName()
                .OrderBy(codec => codec.Key)
                .Select(codec => new Dropdown.OptionData {text = codec.Value})
                .ToList();

            m_codecSelectDropdown.options.AddRange(list);
        }

        private void OnChangeIndex(int index)
        {
            m_selectIndex = index - 1;
        }

        private void OnDisable()
        {
            m_codecSelectDropdown.options.Clear();
        }
    }
}
