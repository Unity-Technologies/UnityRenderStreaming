using NUnit.Framework;                      //Timeout, Assert
using System.Collections;                   //IEnumerator
using UnityEngine;                          //Debug
using UnityEngine.TestTools;                //UnityTest
using Unity.RenderStreaming.Editor;         //RequestJobManager
using UnityEditor.PackageManager.Requests;  //ListRequest, AddRequest, etc
using UnityEditor.PackageManager;           //PackageCollection

namespace Unity.RenderStreaming
{
public class RequestJobTest
{
    [UnityTest]
    [Timeout(5000)]
    public IEnumerator VerifyRenderStreamingPackage() {

        RequestJobManager.CreateListRequest(false, false, OnListRequestSucceeded, OnListRequestFailed);
        while (!m_listRequestCompleted)
            yield return null;

        Assert.True(m_packageFound);
    }

//---------------------------------------------------------------------------------------------------------------------
    void OnListRequestSucceeded(Request<PackageCollection> packageCollection) {
        m_listRequestCompleted = true;
        m_packageFound = null!=packageCollection.FindPackage("com.unity.renderstreaming");
    }

//---------------------------------------------------------------------------------------------------------------------
    void OnListRequestFailed(Request<PackageCollection> packageCollection) {
        m_listRequestCompleted = true;
    }

//---------------------------------------------------------------------------------------------------------------------

    bool m_listRequestCompleted = false;
    bool m_packageFound = false;

}

} //namespace Unity.RenderStreaming
