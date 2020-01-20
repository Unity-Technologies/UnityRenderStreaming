using System.Collections;                   //IEnumerator
using NUnit.Framework;                      //Timeout
using UnityEngine.TestTools;                //UnityTest

public class ImportTest
{
    [UnityTest]
    [Timeout(3600000)] //1 hour
    public IEnumerator ImportURPSampleTest() {

        RenderStreamingURPAutomator.TryAddURPPackageAndImportSample();
        while (!RenderStreamingURPAutomator.IsSampleImported()) {
            yield return null;
        }
    }
}

