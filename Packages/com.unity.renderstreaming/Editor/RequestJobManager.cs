using System.Collections.Generic;           //HashSet
using UnityEditor.PackageManager.Requests;  //ListRequest, AddRequest, etc
using UnityEditor.PackageManager;           //PackageCollection
using System;                               //Action


namespace Unity.RenderStreaming.Editor {


/// <summary>
/// An editor class to manage requests to UnityEditor.PackageManager.Client
/// This class will perform its operations in background while Unity is running.
/// </summary>
public class RequestJobManager 
{
    [UnityEditor.InitializeOnLoadMethod]
    static void OnLoad() {    
        UnityEditor.EditorApplication.update+=UpdateRequestJobs;        
    }

//---------------------------------------------------------------------------------------------------------------------

    /// <summary>
    /// Queue a job to create a ListRequest instance
    /// </summary>
    /// <param name="offlineMode">Specifies whether or not the Package Manager requests the latest information about
    ///     the project's packages from the remote Unity package registry. When offlineMode is true,
    ///     the PackageInfo objects in the PackageCollection returned by the Package Manager contain information
    ///     obtained from the local package cache, which could be out of date.</param>
    /// <param name="includeIndirectIndependencies">Set to true to include indirect dependencies in the
    ///     PackageCollection returned by the Package Manager. Indirect dependencies include packages referenced
    ///     in the manifests of project packages or in the manifests of other indirect dependencies. Set to false
    ///     to include only the packages listed directly in the project manifest.</param>
    /// <param name="onSuccess">Action which is executed if the request succeeded</param>
    /// <param name="onFail">Action which is executed if the request failed </param>
    /// 
    public static void CreateListRequest(bool offlineMode, bool includeIndirectIndependencies,
       Action<Request<PackageCollection>> onSuccess, Action<Request<PackageCollection>> onFail)
    {
        m_pendingListRequests.Enqueue(new ListRequestInfo(offlineMode, includeIndirectIndependencies, onSuccess, onFail));
    }

//---------------------------------------------------------------------------------------------------------------------

    /// <summary>
    /// Queue a job to create an AddRequest instance
    /// </summary>
    /// <param name="packageName">The name or ID of the package to add. If only the name is specified,
    ///     the latest version of the package is installed.</param>
    /// <param name="onSuccess">Action which is executed if the request succeeded</param>
    /// <param name="onFail">Action which is executed if the request failed </param>
    /// 
    public static void CreateAddRequest(string packageName,
        Action<Request<PackageInfo>> onSuccess, Action<Request<PackageInfo>> onFail)
    {
        m_pendingAddRequests.Enqueue(new AddRequestInfo(packageName, onSuccess, onFail));
    }
    
//---------------------------------------------------------------------------------------------------------------------   

    static void UpdateRequestJobs()
    {
        {   //Process pending list requests
            var enumerator = m_pendingListRequests.GetEnumerator();
            while (enumerator.MoveNext()) {
                ListRequestInfo info = enumerator.Current;
                ListRequest listReq = Client.List(info.OfflineMode, info.IncludeIndirectIndependencies);
                m_requestJobs.Add(new RequestJob<PackageCollection>(listReq,info.OnSuccessAction,info.OnFailAction));               
            }
            m_pendingListRequests.Clear();
        }

        {   //Process pending addrequests
            var enumerator = m_pendingAddRequests.GetEnumerator();
            while (enumerator.MoveNext()) {
                AddRequestInfo info = enumerator.Current;
                AddRequest addReq = Client.Add(info.PackageName);
                m_requestJobs.Add(new RequestJob<PackageInfo>(addReq,info.OnSuccessAction,info.OnFailAction));
            }
            m_pendingAddRequests.Clear();
        }

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

    static Queue<ListRequestInfo> m_pendingListRequests = new Queue<ListRequestInfo>();
    static Queue<AddRequestInfo>  m_pendingAddRequests = new Queue<AddRequestInfo>();

    static System.Collections.Generic.HashSet<IRequestJob> m_requestJobs = new HashSet<IRequestJob>();
    static System.Collections.Generic.List<IRequestJob> m_jobsToDelete = new List<IRequestJob>();

}

} //namespace Unity.RenderStreaming.Editor 
