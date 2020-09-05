using System;
using UnityEngine;
using UnityEngine.Networking;

namespace Unity.RenderStreaming
{
#pragma warning disable 0649
    [Serializable]
    public class SignalingMessage
    {
        public string status;
        public string message;
        public string sessionId;
        public string connectionId;
        public string peerId;
        public string sdp;
        public string type;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    [Serializable]
    public class RoutedMessage<T>
    {
        public string from;
        public string to;
        public string type;
        public T data;
    }

    [Serializable]
    public class DescData
    {
        public string connectionId;
        public string sdp;
        public string type;
    }

    [Serializable]
    class OpenSessionData
    {
        public string sessionId;
    }

    [Serializable]
    class CreateConnectionResData
    {
        public string connectionId;
    }

    [Serializable]
    class OfferResDataList
    {
        public DescData[] offers;
    }

    [Serializable]
    class AnswerResDataList
    {
        public DescData[] answers;
    }

    
    [Serializable]
    class CandidateContainerResDataList
    {
        public CandidateContainerResData[] candidates;
    }

    [Serializable]
    class CandidateContainerResData
    {
        public string connectionId;
        public CandidateData[] candidates;
    }

    [Serializable]
    public class CandidateData
    {
        public string connectionId;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

#pragma warning restore 0649
}
