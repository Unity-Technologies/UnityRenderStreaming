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
        RequestJobManager jobManager = GetOrCreateInstance();
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
        RequestJobManager jobManager = GetOrCreateInstance();
        ListRequest listReq = UnityEditor.PackageManager.Client.List(offlineMode, includeIndirectIndependencies);
        jobManager.m_requestJobs.Add(new RequestJob<PackageCollection>(listReq,onSuccess,onFail));
    }

//---------------------------------------------------------------------------------------------------------------------

    public static void CreateAddRequest(string packageName,
        Action<Request<PackageInfo>> onSuccess, Action<Request<PackageInfo>> onFail)
    {
        RequestJobManager jobManager = GetOrCreateInstance();
        AddRequest addReq = UnityEditor.PackageManager.Client.Add(packageName);
        jobManager.m_requestJobs.Add(new RequestJob<PackageInfo>(addReq,onSuccess,onFail));
    }
    
//---------------------------------------------------------------------------------------------------------------------   

    static void UpdateRequestJobs()
    {
        RequestJobManager jobManager = GetOrCreateInstance();

        {   //Update and register completed jobs
            var enumerator = jobManager.m_requestJobs.GetEnumerator();
            while (enumerator.MoveNext()) {
                StatusCode code = enumerator.Current.Update();
                if (StatusCode.Failure == code || StatusCode.Success == code) {
                    jobManager.m_jobsToDelete.Add(enumerator.Current);
                }
            }
        }
        
        { //delete completed jobs
            var enumerator = jobManager.m_jobsToDelete.GetEnumerator();
            while (enumerator.MoveNext()) {
                jobManager.m_requestJobs.Remove(enumerator.Current);
            }
            jobManager.m_jobsToDelete.Clear();
        }

    }

//---------------------------------------------------------------------------------------------------------------------   

    static RequestJobManager GetOrCreateInstance() {

        if (null == m_instance) {
            m_instance = UnityEngine.Object.FindObjectOfType<RequestJobManager>();

            if (null == m_instance) {
                m_instance = CreateInstance<RequestJobManager>();
                return m_instance;
            }
        }
        return m_instance;
    }

//---------------------------------------------------------------------------------------------------------------------

    System.Collections.Generic.HashSet<IRequestJob> m_requestJobs;
    System.Collections.Generic.List<IRequestJob> m_jobsToDelete;

    static RequestJobManager m_instance = null;

}

} //namespace Unity.RenderStreaming.Editor 
