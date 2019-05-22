using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using Unity.WebRTC;

public class TransmitText : MonoBehaviour
{
    [SerializeField] private Button callButton;
    [SerializeField] private InputField textSend;
    [SerializeField] private InputField textReceive;

    private MediaStream localStream;
    private RTCPeerConnection pc1, pc2;
    private Coroutine sdpCheck;
    private string msg;
    private readonly object syncObj = new object();

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
    }

    private void Update()
    {
        while(pc2 != null && pc2.dataChannelMsgs.Count > 0)
        {
            pc2.dataChannelMsgs.TryDequeue(out msg);
            textReceive.text = msg;
        }
    }

    private IEnumerator Loop()
    {
        while (true)
        {
            yield return new WaitForSeconds(1);

            var desc = pc1.GetLocalDescription();
            Debug.Log($"pc1 sdp: {desc.sdp}");

            //var conf = pc1.GetConfiguration();
            //Debug.Log($"ice servers count {conf.iceServers.Length}");
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
    void pc1OnIceCandidate(string sdp, string sdpMid, int sdpMlineIndex) 
    {
        RTCIceCandidate​ candidate;
        candidate.candidate = sdp;
        candidate.sdpMid = sdpMid;
        candidate.sdpMlineIndex = sdpMlineIndex;
        OnIceCandidate(pc1, candidate);
    }
    void pc2OnIceCandidate(string sdp, string sdpMid, int sdpMlineIndex)
    {
        RTCIceCandidate​ candidate;
        candidate.candidate = sdp;
        candidate.sdpMid = sdpMid;
        candidate.sdpMlineIndex = sdpMlineIndex;
        OnIceCandidate(pc2, candidate);
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
        pc1.RegisterOnIceCandidateReady(pc1OnIceCandidate);
        pc2 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created remote peer connection object pc2");
        pc2.RegisterOnIceCandidateReady(pc2OnIceCandidate);

        pc1.onIceConnectionStateChange = delegate () { OnIceStateChange(pc1); };
        pc2.onIceConnectionStateChange = delegate () { OnIceStateChange(pc2); };
        pc2.onTrack = delegate (
            ref RTCRtpReceiver receiver,
            MediaStream[] streams,
            ref MediaStreamTrack track,
            ref RTCRtpTransceiver transceiver)
        { GotRemoteStream(pc2); };

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
        //sdpCheck = StartCoroutine(Loop()); 
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="pc"></param>
    /// <param name="streamEvent"></param>
    void OnIceCandidate(RTCPeerConnection pc, RTCIceCandidate​ candidate)
    {
        GetOtherPc(pc).AddIceCandidate(ref candidate);
        //if (!op.isError)
        //{
        //    OnAddIceCandidateSuccess(pc);
        //}
        //else
        //{
        //    OnAddIceCandidateError(pc, op.error);
        //}
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

    void OnIceStateChange(RTCPeerConnection pc)
    {
        if (pc != null)
        {
            Debug.Log($"{GetName(pc)} ICE state: {pc.IceConnectionState}");
            Debug.Log("ICE state change event: ");
        }
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
    void OnApplicationQuit()
    {
        pc1.Close();
        pc2.Close();
        pc1 = null;
        pc2 = null;
    }
}
