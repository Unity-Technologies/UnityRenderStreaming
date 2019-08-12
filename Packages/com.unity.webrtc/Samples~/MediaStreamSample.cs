using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.WebRTC;
using UnityEngine.UI;
using System.Text;
using UnityEngine.SceneManagement;

public class MediaStreamSample : MonoBehaviour
{
#pragma warning disable 0649
    [SerializeField] private Button callButton;
    [SerializeField] private Button addTracksButton;
    [SerializeField] private Button removeTracksButton;
    [SerializeField] private Camera cam;
    [SerializeField] private InputField infoText;
    [SerializeField] private RawImage RtImage;
#pragma warning restore 0649

    private RTCPeerConnection pc1, pc2;
    private List<RTCRtpSender> pc1Senders, pc2Senders;
    private Unity.WebRTC.MediaStream audioStream, videoStream;
    private RTCDataChannel dataChannel, remoteDataChannel;
    private Coroutine sdpCheck;
    private string msg;
    private DelegateOnIceConnectionChange pc1OnIceConnectionChange;
    private DelegateOnIceConnectionChange pc2OnIceConnectionChange;
    private DelegateOnIceCandidate pc1OnIceCandidate;
    private DelegateOnIceCandidate pc2OnIceCandidate;
    private DelegateOnTrack pc2Ontrack;
    private DelegateOnNegotiationNeeded pc1OnNegotiationNeeded;
    private StringBuilder trackInfos;
    private bool videoUpdateStarted = false;

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
        callButton.onClick.AddListener(() => { Call(); });
        addTracksButton.onClick.AddListener(() => { AddTracks(); });
        removeTracksButton.onClick.AddListener(() => { RemoveTracks(); });
    }

    private void OnDestroy()
    {
        Audio.Stop();
        WebRTC.Finalize();
    }

    private void Start()
    {
        trackInfos = new StringBuilder();
        pc1Senders = new List<RTCRtpSender>();
        pc2Senders = new List<RTCRtpSender>();
        callButton.interactable = true;

        pc1OnIceConnectionChange = new DelegateOnIceConnectionChange(state => { OnIceConnectionChange(pc1, state); });
        pc2OnIceConnectionChange = new DelegateOnIceConnectionChange(state => { OnIceConnectionChange(pc2, state); });
        pc1OnIceCandidate = new DelegateOnIceCandidate(candidate => { OnIceCandidate(pc1, candidate); });
        pc2OnIceCandidate = new DelegateOnIceCandidate(candidate => { OnIceCandidate(pc1, candidate); });
        pc2Ontrack = new DelegateOnTrack(e => { OnTrack(pc2, e); });
        pc1OnNegotiationNeeded = new DelegateOnNegotiationNeeded(() => { StartCoroutine(Pc1OnNegotiationNeeded()); });
        if (!WebRTC.HWEncoderSupport)
        {
            addTracksButton.interactable = false;
            callButton.interactable = false;
            infoText.text = "Current GPU doesn't support Nvidia Encoder";
        }
        else
        {
            infoText.text = "Current GPU supports Nvidia Encoder";
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
    }
    IEnumerator Pc1OnNegotiationNeeded()
    {
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
    void Pc1OnIceConnectinChange(RTCIceConnectionState state)
    {
        OnIceConnectionChange(pc1, state);
    }
    void Pc2OnIceConnectionChange(RTCIceConnectionState state)
    {
        OnIceConnectionChange(pc2, state);
    }

    void Pc1OnIceCandidate(RTCIceCandidate candidate)
    {
        OnIceCandidate(pc1, candidate);
    }
    void Pc2OnIceCandidate(RTCIceCandidate candidate)
    {
        OnIceCandidate(pc2, candidate);
    }
    public void AddTracks() 
    {
        foreach (var track in audioStream.GetTracks())
        {
            pc1Senders.Add (pc1.AddTrack(track));  
        }
        foreach(var track in videoStream.GetTracks())
        {
            pc1Senders.Add(pc1.AddTrack(track));
        }
        if(!videoUpdateStarted)
        {
            StartCoroutine(WebRTC.Update());
            videoUpdateStarted = true;
        }
        addTracksButton.interactable = false;
        removeTracksButton.interactable = true;
    }

    public void RemoveTracks()
    {
        foreach(var sender in pc1Senders)
        {
            pc1.RemoveTrack(sender);
        }
        foreach (var sender in pc2Senders)
        {
            pc2.RemoveTrack(sender);
        }
        pc1Senders.Clear();
        pc2Senders.Clear();
        addTracksButton.interactable = true;
        removeTracksButton.interactable = false;
        trackInfos.Clear();
        infoText.text = "";
    }

    void Call()
    {
        callButton.interactable = false;
        Debug.Log("GetSelectedSdpSemantics");
        var configuration = GetSelectedSdpSemantics();
        pc1 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created local peer connection object pc1");
        pc1.OnIceCandidate = pc1OnIceCandidate;
        pc1.OnIceConnectionChange = pc1OnIceConnectionChange;
        pc1.OnNegotiationNeeded = pc1OnNegotiationNeeded;
        pc2 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created remote peer connection object pc2");
        pc2.OnIceCandidate = pc2OnIceCandidate;
        pc2.OnIceConnectionChange = pc2OnIceConnectionChange;
        pc2.OnTrack = pc2Ontrack;

        RTCDataChannelInit conf = new RTCDataChannelInit(true);
        dataChannel = pc1.CreateDataChannel("data", ref conf);
        audioStream = Audio.CaptureStream();
        videoStream = cam.CaptureStream(1280, 720);
        RtImage.texture = cam.targetTexture;
 
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
    void OnTrack(RTCPeerConnection pc, RTCTrackEvent e)
    {
        pc2Senders.Add(pc.AddTrack(e.Track));
        trackInfos.Append($"{GetName(pc)} receives remote track:\r\n");
        trackInfos.Append($"Track kind: {e.Track.Kind}\r\n");
        trackInfos.Append($"Track id: {e.Track.Id}\r\n");
        infoText.text = trackInfos.ToString();
    }

    string GetName(RTCPeerConnection pc)
    {
        return (pc == pc1) ? "pc1" : "pc2";
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
