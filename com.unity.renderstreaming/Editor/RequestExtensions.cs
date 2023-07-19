using System.Collections.Generic;       //IEnumerable
using UnityEditor.PackageManager;       //PackageCollection  
using UnityEditor.PackageManager.Requests;  //ListRequest


namespace Unity.RenderStreaming.Editor
{

    /// <summary>
    /// An extension class to extend the functionalities of UnityEditor.PackageManager.Requests classes
    /// </summary>
    internal static class RequestExtensions
    {

        /// <summary>
        /// Find a PackageInfo which has the passed parameter 
        /// </summary>
        /// <param name="listRequest">list of <see cref="Request">Request</see> object</param>
        /// <param name="packageName">the package name</param>
        /// <returns>The PackageInfo if found, otherwise null</returns>
        public static PackageInfo FindPackage(this Request<PackageCollection> listRequest, string packageName)
        {
            IEnumerable<PackageInfo> packageInfoCollection = listRequest.Result as IEnumerable<PackageInfo>;
            if (null == packageInfoCollection)
            {
                return null;
            }

            var enumerator = packageInfoCollection.GetEnumerator();
            while (enumerator.MoveNext())
            {
                PackageInfo curInfo = enumerator.Current;
                if (curInfo.name == packageName)
                {
                    return curInfo;
                }
            }

            return null;
        }
    }

} //namespace Unity.RenderStreaming.Editor
