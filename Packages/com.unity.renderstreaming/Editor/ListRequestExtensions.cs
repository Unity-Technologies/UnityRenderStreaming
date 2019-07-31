using UnityEditor.PackageManager.Requests;  //ListRequest
using UnityEditor.PackageManager;  
using System.Collections.Generic;       //IEnumerable


namespace Unity.RenderStreaming.Editor {
    public static class ListRequestExtensions
    {
        public static PackageInfo FindPackage(this ListRequest listRequest, string packageName) {
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
