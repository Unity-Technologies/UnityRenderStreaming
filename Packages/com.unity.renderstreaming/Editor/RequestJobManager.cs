using UnityEngine;                          //FindObjectOfType, CreateInstance
using System.Collections.Generic;           //HashSet
using UnityEditor.PackageManager.Requests;  //ListRequest, AddRequest, etc
using UnityEditor.PackageManager;           //PackageCollection
using System;                               //Action


namespace Unity.RenderStreaming.Editor {

public class RequestJobManager : ScriptableObject
{
    [UnityEditor.InitializeOnLoadMethod]
    static void OnLoad() {
        UnityEditor.EditorApplication.update+=UpdateRequestJobs;        
    }

//---------------------------------------------------------------------------------------------------------------------

    RequestJobManager() {
        m_requestJobs = new HashSet<IRequestJob>();
        m_jobsToDelete = new List<IRequestJob>();
    }

//---------------------------------------------------------------------------------------------------------------------

    public static void CreateListRequest(bool offlineMode, bool includeIndirectIndependencies,
       Action<Request<PackageCollection>> onSuccess, Action<Request<PackageCollection>> onFail)
    {
        ListRequest listReq = UnityEditor.PackageManager.Client.List(offlineMode, includeIndirectIndependencies);
        m_requestJobs.Add(new RequestJob<PackageCollection>(listReq,onSuccess,onFail));
    }

//---------------------------------------------------------------------------------------------------------------------

    public static void CreateAddRequest(string packageName,
        Action<Request<PackageInfo>> onSuccess, Action<Request<PackageInfo>> onFail)
    {
        AddRequest addReq = UnityEditor.PackageManager.Client.Add(packageName);
        m_requestJobs.Add(new RequestJob<PackageInfo>(addReq,onSuccess,onFail));
    }
    
//---------------------------------------------------------------------------------------------------------------------   

    static void UpdateRequestJobs()
    {
        {   //Update and register completed jobs
            var enumerator = m_requestJobs.GetEnumerator();
            while (enumerator.MoveNext()) {
                StatusCode code = enumerator.Current.Update();
                if (StatusCode.Failure == code || StatusCode.Success == code) {
                    m_jobsToDelete.Add(enumerator.Current);
                }
            }
        }
        
        { //delete completed jobs
            var enumerator = m_jobsToDelete.GetEnumerator();
            while (enumerator.MoveNext()) {
                m_requestJobs.Remove(enumerator.Current);
            }
            m_jobsToDelete.Clear();
        }

    }

//---------------------------------------------------------------------------------------------------------------------

    static System.Collections.Generic.HashSet<IRequestJob> m_requestJobs = new HashSet<IRequestJob>();
    static System.Collections.Generic.List<IRequestJob> m_jobsToDelete = new List<IRequestJob>();

}

} //namespace Unity.RenderStreaming.Editor 
