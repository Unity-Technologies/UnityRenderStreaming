using System;

namespace Unity.RenderStreaming.Signaling
{
#pragma warning disable 0649

    [Serializable]
    class OpenSessionData
    {
        public string sessionId;
    }

    [Serializable]
    public class DescData
    {
        public string connectionId;
        public string sdp;
        public string type;
    }

    [Serializable]
    class OfferList
    {
        public DescData[] offers;
    }


    [Serializable]
    public class CandidateData
    {
        public string connectionId;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    [Serializable]
    class CandidatesContainer
    {
        public string connectionId;
        public CandidateData[] candidates;
    }

    [Serializable]
    class CandidatesContainerList
    {
        public CandidatesContainer[] candidates;
    }

    [Serializable]
    public class RoutedMessage<T>
    {
        public string from;
        public string to;
        public string type;
        public T data;
    }

    [Flags]
    enum SslProtocolsHack
    {
        Tls = 192,
        Tls11 = 768,
        Tls12 = 3072
    }

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

#pragma warning restore 0649
}
