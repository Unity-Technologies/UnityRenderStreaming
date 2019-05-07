using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using Unity.WebRTC;

public class Main : MonoBehaviour
{
    [SerializeField] private Button startButton;
    [SerializeField] private Button callButton;
    [SerializeField] private Button hangupButton;

    private MediaStream localStream;
    private RTCPeerConnection pc1, pc2;

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

        startButton.onClick.AddListener(() => { StartCoroutine(Start()); });
        callButton.onClick.AddListener(() => { StartCoroutine(Call()); });
        hangupButton.onClick.AddListener(() => { Hangup(); });
    }

    private void OnDestroy()
    {
        WebRTC.Finalize();
    }

    private IEnumerator Start()
    {
        Debug.Log("Requesting local stream");
        var op = MediaDevices.GetUserMedia(new MediaStreamConstraints{audio = true, video = true});
        yield return op;
        var stream = op.stream;
        Debug.Log("Received local stream");
        localStream = stream;

        startButton.interactable = false;
        callButton.interactable = true;
    }

    private IEnumerator Loop()
    {
        while(true)
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

    IEnumerator Call()
    {
        callButton.interactable = false;
        hangupButton.interactable = true;

        var videoTracks = localStream.GetVideoTracks();
        var audioTracks = localStream.GetAudioTracks();
        if (videoTracks.Length > 0) {
            Debug.Log($"Using video device: {videoTracks.Length}");
        }
        if (audioTracks.Length > 0) {
            Debug.Log($"Using audio device: {audioTracks.Length}");
        }
        Debug.Log("GetSelectedSdpSemantics");
        var configuration = GetSelectedSdpSemantics();
        pc1 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created local peer connection object pc1");
        pc1.onIceCandidate = delegate (ref RTCIceCandidate​ e) { StartCoroutine(OnIceCandidate(pc1, e)); };
        pc2 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created remote peer connection object pc2");
        pc2.onIceCandidate = delegate (ref RTCIceCandidate​ e) { StartCoroutine(OnIceCandidate(pc2, e)); };

        pc1.onIceConnectionStateChange = delegate () { OnIceStateChange(pc1); };
        pc2.onIceConnectionStateChange = delegate () { OnIceStateChange(pc2); };
        pc2.onTrack = delegate (
            ref RTCRtpReceiver receiver,
            MediaStream[] streams,
            ref MediaStreamTrack track,
            ref RTCRtpTransceiver transceiver) { GotRemoteStream (pc2); };

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
        StartCoroutine(Loop());
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="pc"></param>
    /// <param name="streamEvent"></param>
    IEnumerator OnIceCandidate(RTCPeerConnection pc, RTCIceCandidate​ candidate)
    {
        var op = GetOtherPc(pc).AddIceCandidate(ref candidate);
        yield return op;
        if (!op.isError)
        {
            OnAddIceCandidateSuccess(pc);
        }
        else
        {
            OnAddIceCandidateError(pc, op.error);
        }
        Debug.Log($"{GetName(pc)} ICE candidate:\n {candidate.candidate}" );
    }

    string GetName(RTCPeerConnection pc)
    {
        return (pc == pc1) ? "pc1" : "pc2";
    }
    
    void OnIceStateChange(RTCPeerConnection pc)
    {
        if (pc != null) {
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
    
    void OnSetSessionDescriptionError(ref RTCError error){}

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
    void Hangup()
    {
        Debug.Log("Ending call");
        pc1.Close();
        pc2.Close();
        pc1 = null;
        pc2 = null;
        hangupButton.interactable = false;
        callButton.interactable = true;
    }
}
