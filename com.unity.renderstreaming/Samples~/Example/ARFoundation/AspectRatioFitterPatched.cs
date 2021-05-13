using UnityEngine.UI;

/// <summary>
/// See below
/// https://forum.unity.com/threads/aspectratiofitter-new-restriction-in-2020-2.1022683/
/// </summary>
internal class AspectRatioFitterPatched : AspectRatioFitter
{
    protected override void Start()
    {
        base.Start();
        SetDirty();
    }
}
