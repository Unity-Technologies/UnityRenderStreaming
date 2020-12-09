using System.Collections;                   //IEnumerator
using NUnit.Framework;                      //Timeout
using UnityEngine.TestTools;                //UnityTest

public class ImportTest
{
    [UnityTest]
    [Timeout(3600000)] //1 hour
    public IEnumerator ImportCommonSampleTest() {

        RenderStreamingCommonAutomator.TryAddCommonPackageAndImportSample();
        while (!RenderStreamingCommonAutomator.IsSampleImported()) {
            yield return null;
        }
    }
}

