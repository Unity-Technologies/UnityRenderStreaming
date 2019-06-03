using System;
using UnityEngine;
using UnityEngine.Networking;

namespace Unity.RenderStreaming
{
    public class DownloadHandlerJson<T> : DownloadHandlerScript
    {
        private T m_obj;

        public DownloadHandlerJson() : base()
        {
        }

        public DownloadHandlerJson(byte[] buffer) : base(buffer)
        {
        }

        protected override byte[] GetData() { return null; }

        protected override bool ReceiveData(byte[] data, int dataLength)
        {
            if (data == null || data.Length < 1)
            {
                return false;
            }
            var text = System.Text.Encoding.UTF8.GetString(data);
            try
            {
                m_obj = JsonUtility.FromJson<T>(text);
            }
            catch(Exception e)
            {
                Debug.LogError(text);
                throw e;
            }
            return true;
        }

        public T GetObject()
        {
            return m_obj;
        }
    }

    static class DownloadHandlerExtension
    {
        public static T FromJson<T>(this DownloadHandler handler)
        {
            return JsonUtility.FromJson<T>(handler.text);
        }
    }

    static class UnityWebRequestExtension
    {
        public static UnityWebRequestAsyncOperation SendWebRequest<T>(this UnityWebRequest own)
        {
            own.downloadHandler = new DownloadHandlerJson<T>();
            var req = own.SendWebRequest();
            return req;
        }

        public static DownloadHandlerJson<T> DownloadHandlerJson<T>(this UnityWebRequest own)
        {
            return own.downloadHandler as DownloadHandlerJson<T>;
        }
    }

#pragma warning disable 0649
    [Serializable]
    class NewResData
    {
        public string sessionId;
    }

    [Serializable]
    class OfferResDataList
    {
        public OfferResData[] offers;
    }

    [Serializable]
    class CandidateContainerResDataList
    {
        public CandidateContainerResData[] candidates;
    }

    [Serializable]
    class OfferResData
    {
        public string connectionId;
        public string sdp;
    }

    [Serializable]
    class AnswerResData
    {
        public string connectionId;
        public string sdp;
    }

    [Serializable]
    class CandidateContainerResData
    {
        public string connectionId;
        public CandidateResData[] candidates;
    }

    [Serializable]
    class CandidateResData
    {
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }



#pragma warning restore 0649


    public class Signaling
    {
        public string Url { get; }

        public Signaling(string url)
        {
            Url = url;
        }

        [Serializable]
        class OfferReqData
        {
            public string connectionId;
            public string sdp;
        }

        [Serializable]
        class AnswerReqData
        {
            public string connectionId;
            public string sdp;
        }

        [Serializable]
        class CandidateReqData
        {
            public string connectionId;
            public string candidate;
            public string sdpMid;
            public int sdpMlineIndex;
        }

        public UnityWebRequestAsyncOperation Create()
        {
            var req = new UnityWebRequest($"{Url}/signaling", "PUT");
            var op = req.SendWebRequest<NewResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation Delete()
        {
            var req = new UnityWebRequest($"{Url}/signaling", "DELETE");
            var op = req.SendWebRequest();
            return op;
        }

        public UnityWebRequestAsyncOperation PostOffer(string sessionId, string connectionId, string sdp)
        {
            var obj = new OfferReqData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/offer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest();
            return op;
        }
        public UnityWebRequestAsyncOperation GetOffer(string sessionId)
        {
            var req = new UnityWebRequest($"{Url}/signaling/offer", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<OfferResDataList>();
            return op;
        }
        public UnityWebRequestAsyncOperation PostAnswer(string sessionId, string connectionId, string sdp)
        {
            var obj = new AnswerReqData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/answer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest();
            return op;
        }
        public UnityWebRequestAsyncOperation GetAnswer(string sessionId, string connectionId)
        {
            var req = new UnityWebRequest($"{Url}/signaling/answer", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<AnswerResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation PostCandidate(string sessionId, string connectionId, string candidate, string sdpMid, int sdpMlineIndex)
        {
            var obj = new CandidateReqData { connectionId = connectionId, candidate = candidate, sdpMid = sdpMid, sdpMlineIndex = sdpMlineIndex };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/candidate", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest();
            return op;
        }
        public UnityWebRequestAsyncOperation GetCandidate(string sessionId)
        {
            var req = new UnityWebRequest($"{Url}/signaling/candidate", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<CandidateContainerResDataList>();
            return op;
        }
    }
}
