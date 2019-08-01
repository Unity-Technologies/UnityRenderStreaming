using UnityEditor.PackageManager.Requests;  //ListRequest
using UnityEditor.PackageManager;       //PackageCollection  
using System.Collections.Generic;       //IEnumerable


namespace Unity.RenderStreaming.Editor {
    public static class RequestExtensions
    {
        /// <summary>
        /// [TODO-sin: 2019-8-1] 
        /// </summary>
        /// <param name="todo"></param>
        /// <returns></returns>
        /// 
        public static PackageInfo FindPackage(this Request<PackageCollection> listRequest, string packageName) {
            IEnumerable<PackageInfo> packageInfoCollection = listRequest.Result as IEnumerable<PackageInfo>;
            if (null == packageInfoCollection) {
                return null;
            }

            var enumerator = packageInfoCollection.GetEnumerator();
            while (enumerator.MoveNext()) {
                PackageInfo curInfo = enumerator.Current;
                if (curInfo.name == packageName) {
                    return curInfo;
                }
            }

            return null;
        }
    }   
}
