using NUnit.Framework;
using UnityEngine;                  //ColorSpace
using UnityEngine.Rendering;        //GraphicsSettings
using UnityEditor;                  //PlayerSettings

namespace Tests.Editor
{

public class ProjectSettingsTest {
    [Test]
    public void VerifyGraphicsSettings()  {
        Assert.True(null != GraphicsSettings.renderPipelineAsset);
        Assert.True(ColorSpace.Linear == PlayerSettings.colorSpace );
    }

//---------------------------------------------------------------------------------------------------------------------

    [Test]
    public void VerifyPlayerSettings()  {
        Assert.True(ColorSpace.Linear == PlayerSettings.colorSpace );
    }
}

} //namespace Tests.Editor
