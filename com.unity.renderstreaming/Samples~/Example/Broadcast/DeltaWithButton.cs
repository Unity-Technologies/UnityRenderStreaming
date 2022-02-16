using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.Utilities;
using UnityEngine.Scripting;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Unity.RenderStreaming.Samples
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

#if UNITY_EDITOR
    [InitializeOnLoad]
#endif
    [Preserve]
    [DisplayStringFormat("{button}+{delta}")]
    class DeltaWithButton : InputBindingComposite<Vector2>
    {
#if UNITY_EDITOR
        static DeltaWithButton()
        {
            Initialize();
        }
#endif

        [RuntimeInitializeOnLoadMethod]
        static void Initialize()
        {
            InputSystem.RegisterBindingComposite<DeltaWithButton>();
        }

        [InputControl(layout = "Button")] public int delta;
        [InputControl(layout = "Button")] public int button;

        public override Vector2 ReadValue(ref InputBindingCompositeContext context)
        {
            return context.ReadValueAsButton(button) ? context.ReadValue<Vector2, Vector2MagnitudeComparer>(delta) : default;
        }

        public override float EvaluateMagnitude(ref InputBindingCompositeContext context)
        {
            return ReadValue(ref context).magnitude;
        }
    }
}
