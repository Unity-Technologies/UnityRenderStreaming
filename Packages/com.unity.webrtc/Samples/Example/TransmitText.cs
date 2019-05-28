using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using Unity.WebRTC;

public class TransmitText : MonoBehaviour
{
    [SerializeField] private Button callButton;
    [SerializeField] private Button sendButton;
    [SerializeField] private InputField textSend;
    [SerializeField] private InputField textReceive;

    private MediaStream localStream;
    private RTCPeerConnection pc1, pc2;
    private Coroutine sdpCheck;
    private string msg;
    private DelegateOnIceConnectionChange pc1OnIceConnectionChange;
    private DelegateOnIceConnectionChange pc2OnIceConnectionChange;
    private DelegateIceCandidateReady pc1OnIceCandidateReady;
    private DelegateIceCandidateReady pc2OnIceCandidateReady;

    private RTCOfferOptions OfferOptions = new RTCOfferOptions
    {
        iceRestart = false,
        offerToReceiveAudio = true,
        offerToReceiveVideo = false
    };

    private RTCAnswerOptions AnswerOptions = new RTCAnswerOptions
    {
        iceRestart = false, 
    };

    private void Awake()
    {
        WebRTC.Initialize();
        callButton.onClick.AddListener(() => { StartCoroutine(Call()); });
    }

    private void OnDestroy()
    {
        WebRTC.Finalize();
    }

    private IEnumerator Start()
    {
        Debug.Log("Requesting local stream");
        var op = MediaDevices.GetUserMedia(new MediaStreamConstraints { audio = true, video = true });
        yield return op;
        var stream = op.stream;
        Debug.Log("Received local stream");
        localStream = stream;
        callButton.interactable = true;

        pc1OnIceConnectionChange = new DelegateOnIceConnectionChange(Pc1OnIceConnectinChange);
        pc2OnIceConnectionChange = new DelegateOnIceConnectionChange(Pc2OnIceConnectionChange);
        pc1OnIceCandidateReady = new DelegateIceCandidateReady(Pc1OnIceCandidate);
        pc2OnIceCandidateReady = new DelegateIceCandidateReady(Pc2OnIceCandidate);
    }

    private void Update()
    {
        if(pc2 != null && WebRTC.S_dataChannelMsgs.Count > 0)
        {
            WebRTC.S_dataChannelMsgs.TryDequeue(out msg);
            textReceive.text = msg;
        }
    }

    RTCConfiguration GetSelectedSdpSemantics()
    {
        RTCConfiguration config = default;
        config.iceServers = new RTCIceServer[]
        {
            new RTCIceServer { urls = new string[] { "stun:stun.l.google.com:19302" } }
        };

        return config;
    }
    void OnIceConnectionChange(RTCPeerConnection pc, RTCIceConnectionState state)
    {
        WebRTC.SyncContext.Post(_ =>
        {
            switch (state)
            {
                case RTCIceConnectionState.New:
                    Debug.Log($"{GetName(pc)} IceConnectionState: New");
                    break;
                case RTCIceConnectionState.Checking:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Checking");
                    break;
                case RTCIceConnectionState.Closed:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Closed");
                    break;
                case RTCIceConnectionState.Completed:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Completed");
                    break;
                case RTCIceConnectionState.Connected:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Connected");
                    sendButton.interactable = true;
                    break;
                case RTCIceConnectionState.Disconnected:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Disconnected");
                    break;
                case RTCIceConnectionState.Failed:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Failed");
                    break;
                case RTCIceConnectionState.Max:
                    Debug.Log($"{GetName(pc)} IceConnectionState: Max");
                    break;
                default:
                    break;
            }
        }, null);
    }
    void Pc1OnIceConnectinChange(RTCIceConnectionState state)
    {
        OnIceConnectionChange(pc1, state);
    }
    void Pc2OnIceConnectionChange(RTCIceConnectionState state)
    {
        OnIceConnectionChange(pc2, state);
    }

    void OnIceCandidate(RTCPeerConnection pc, string sdp, string sdpMid,int sdpMlineIndex)
    {
        WebRTC.SyncContext.Post(_ =>
        {
            RTCIceCandidate​ candidate;
            candidate.candidate = sdp;
            candidate.sdpMid = sdpMid;
            candidate.sdpMlineIndex = sdpMlineIndex;
            OnIceCandidate(pc, candidate);
        }, null);
    }
    void Pc1OnIceCandidate(string sdp, string sdpMid, int sdpMlineIndex) 
    {
        OnIceCandidate(pc1, sdp, sdpMid, sdpMlineIndex);
    }
    void Pc2OnIceCandidate(string sdp, string sdpMid, int sdpMlineIndex)
    {
        OnIceCandidate(pc2, sdp, sdpMid, sdpMlineIndex);
    }

IEnumerator Call()
    {
        callButton.interactable = false;
        var videoTracks = localStream.GetVideoTracks();
        var audioTracks = localStream.GetAudioTracks();
        if (videoTracks.Length > 0)
        {
            Debug.Log($"Using video device: {videoTracks.Length}");
        }
        if (audioTracks.Length > 0) 
        {
            Debug.Log($"Using audio device: {audioTracks.Length}");
        }
        Debug.Log("GetSelectedSdpSemantics");
        var configuration = GetSelectedSdpSemantics();
        pc1 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created local peer connection object pc1");
        pc1.RegisterOnIceCandidateReady(pc1OnIceCandidateReady);
        pc1.RegisterOnIceConnectionChange(pc1OnIceConnectionChange);
        pc2 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created remote peer connection object pc2"); 
        pc2.RegisterOnIceCandidateReady(pc2OnIceCandidateReady);
        pc2.RegisterOnIceConnectionChange(pc2OnIceConnectionChange);

        RTCDataChannelInit conf = new RTCDataChannelInit(true);
        pc1.CreateDataChannel("data", ref conf);

        foreach (var track in localStream.GetTracks())
        {
            pc1.AddTrack(track, localStream);
        }
        Debug.Log("Added local stream to pc1");

        Debug.Log("pc1 createOffer start");
        var op = pc1.CreateOffer(ref OfferOptions);
        yield return op;

        if (!op.isError)
        {
            yield return StartCoroutine(OnCreateOfferSuccess(op.desc));
        }
        else
        {
            OnCreateSessionDescriptionError(op.error);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="pc"></param>
    /// <param name="streamEvent"></param>
    void OnIceCandidate(RTCPeerConnection pc, RTCIceCandidate​ candidate)
    {
        GetOtherPc(pc).AddIceCandidate(ref candidate);
        Debug.Log($"{GetName(pc)} ICE candidate:\n {candidate.candidate}");
    }


    public void SendMsg()
    {
        pc1.SendData(textSend.text);
    }
    string GetName(RTCPeerConnection pc)
    {
        return (pc == pc1) ? "pc1" : "pc2";
    }

    void GotRemoteStream(RTCPeerConnection e)
    {

    }

    RTCPeerConnection GetOtherPc(RTCPeerConnection pc)
    {
        return (pc == pc1) ? pc2 : pc1;
    }

    IEnumerator OnCreateOfferSuccess(RTCSessionDescription desc)
    {
        Debug.Log($"Offer from pc1\n{desc.sdp}");
        Debug.Log("pc1 setLocalDescription start");
        var op = pc1.SetLocalDescription(ref desc);
        yield return op;

        if (!op.isError)
        {
            OnSetLocalSuccess(pc1);
        }
        else
        {
            OnSetSessionDescriptionError(ref op.error);
        }

        Debug.Log("pc2 setRemoteDescription start");
        var op2 = pc2.SetRemoteDescription(ref desc);
        yield return op2;
        if (!op2.isError)
        {
            OnSetRemoteSuccess(pc2);
        }
        else
        {
            OnSetSessionDescriptionError(ref op2.error);
        }
        Debug.Log("pc2 createAnswer start");
        // Since the 'remote' side has no media stream we need
        // to pass in the right constraints in order for it to
        // accept the incoming offer of audio and video.

        var op3 = pc2.CreateAnswer(ref AnswerOptions);
        yield return op3;
        if (!op3.isError)
        {
            yield return OnCreateAnswerSuccess(op3.desc);
        }
        else
        {
            OnCreateSessionDescriptionError(op3.error);
        }
    }

    void OnSetLocalSuccess(RTCPeerConnection pc)
    {
        Debug.Log($"{GetName(pc)} SetLocalDescription complete");
    }

    void OnSetSessionDescriptionError(ref RTCError error) { }

    void OnSetRemoteSuccess(RTCPeerConnection pc)
    {
        Debug.Log($"{GetName(pc)} SetRemoteDescription complete");
    }

    IEnumerator OnCreateAnswerSuccess(RTCSessionDescription desc)
    {
        Debug.Log($"Answer from pc2:\n{desc.sdp}");
        Debug.Log("pc2 setLocalDescription start");
        var op = pc2.SetLocalDescription(ref desc);
        yield return op;

        if (!op.isError)
        {
            OnSetLocalSuccess(pc2);
        }
        else
        {
            OnSetSessionDescriptionError(ref op.error);
        }

        Debug.Log("pc1 setRemoteDescription start");

        var op2 = pc1.SetRemoteDescription(ref desc);
        yield return op2;
        if (!op2.isError)
        {
            OnSetRemoteSuccess(pc1);
        }
        else
        {
            OnSetSessionDescriptionError(ref op2.error);
        }
    }

    void OnAddIceCandidateSuccess(RTCPeerConnection pc)
    {
        Debug.Log($"{GetName(pc)} addIceCandidate success");
    }

    void OnAddIceCandidateError(RTCPeerConnection pc, RTCError error)
    {
        Debug.Log($"{GetName(pc)} failed to add ICE Candidate: ${error}");
    }

    void OnCreateSessionDescriptionError(RTCError e)
    {

    }
}
