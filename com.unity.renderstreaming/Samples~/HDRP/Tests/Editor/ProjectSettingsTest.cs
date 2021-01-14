using NUnit.Framework;
using UnityEngine;                  //ColorSpace
using UnityEngine.Rendering;        //GraphicsSettings
using UnityEditor;                  //PlayerSettings

namespace Tests.Editor
{

public class ProjectSettingsTest {
    [Test]
    public void VerifyGraphicsSettings()  {
        Assert.AreNotEqual(null, GraphicsSettings.renderPipelineAsset);
    }

//---------------------------------------------------------------------------------------------------------------------

    [Test]
    public void VerifyPlayerSettings()  {
        Assert.AreEqual(ColorSpace.Linear, PlayerSettings.colorSpace);
    }
}

} //namespace Tests.Editor
