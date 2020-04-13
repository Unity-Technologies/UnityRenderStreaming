using Unity.WebRTC;

namespace Unity.RenderStreaming.Signaling
{
    public delegate void OnOfferHandler(ISignaling signaling, DescData e);
    public delegate void OnAnswerHandler(ISignaling signaling, DescData e);
    public delegate void OnIceCandidateHandler(ISignaling signaling, CandidateData e);

    public interface ISignaling
    {
        void Start();
        void Stop();

        event OnOfferHandler OnOffer;
        event OnAnswerHandler OnAnswer;
        event OnIceCandidateHandler OnIceCandidate;

        void SendOffer();
        void SendAnswer(string connectionId, RTCSessionDescription answer);
        void SendCandidate(string connectionId, RTCIceCandidate​ candidate);
    }
}
