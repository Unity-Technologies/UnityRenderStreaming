using Unity.WebRTC;

namespace Unity.RenderStreaming.Signaling
{
    public delegate void OnStartHandler(ISignaling signaling);
    public delegate void OnConnectHandler(ISignaling signaling, string connectionId);
    public delegate void OnOfferHandler(ISignaling signaling, DescData e);
    public delegate void OnAnswerHandler(ISignaling signaling, DescData e);
    public delegate void OnIceCandidateHandler(ISignaling signaling, CandidateData e);

    public interface ISignaling
    {
        void Start();
        void Stop();

        event OnStartHandler OnStart;
        event OnConnectHandler OnCreateConnection;
        event OnOfferHandler OnOffer;
        event OnAnswerHandler OnAnswer;
        event OnIceCandidateHandler OnIceCandidate;

        void CreateConnection();
        void SendOffer(string connectionId, RTCSessionDescription answer);
        void SendAnswer(string connectionId, RTCSessionDescription answer);
        void SendCandidate(string connectionId, RTCIceCandidate​ candidate);
    }
}
