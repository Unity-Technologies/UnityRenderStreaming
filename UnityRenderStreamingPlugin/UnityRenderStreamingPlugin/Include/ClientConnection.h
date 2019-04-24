#pragma once

namespace WebRTC
{
    class ClientConnection
        : public webrtc::CreateSessionDescriptionObserver
        , public webrtc::PeerConnectionObserver
        , public webrtc::DataChannelObserver
    {
    public:
        ClientConnection(int32 id);

        sigslot::signal2<int32, std::string&> SendAnswer;
        sigslot::signal2<int32, std::string&> SendIceCandidate;
        sigslot::signal1<int32> DeleteClient;
        sigslot::signal0<> StartEncoder;
        //webrtc::CreateSessionDescriptionObserver
        // This callback transfers the ownership of the |desc|.
        void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
        // The OnFailure callback takes an RTCError, which consists of an
        // error code and a string.
        void OnFailure(webrtc::RTCError error) override;

        // webrtc::PeerConnectionObserver
        // Triggered when the SignalingState changed.
        void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {}
        // Triggered when media is received on a new stream from remote peer.
        void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}
        // Triggered when a remote peer closes a stream.
        void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}
        // Triggered when a remote peer opens a data channel.
        void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
        // Triggered when renegotiation is needed. For example, an ICE restart
        // has begun.
        void OnRenegotiationNeeded() override {}
        // Called any time the IceConnectionState changes.
        void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
        // Called any time the IceGatheringState changes.
        void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
        // A new ICE candidate has been gathered.
        void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
        // Ice candidates have been removed.
        void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) override {}
        // Called when the ICE connection receiving status changes.
        void OnIceConnectionReceivingChange(bool Receiving) override {}
        // This is called when signaling indicates a transceiver will be receiving
        // media from the remote endpoint. This is fired during a call to
        // SetRemoteDescription. The receiving track can be accessed by:
        // |transceiver->receiver()->track()| and its associated streams by
        // |transceiver->receiver()->streams()|.
        // Note: This will only be called if Unified Plan semantics are specified.
        void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override {}
        // Called when signaling indicates that media will no longer be received on a
        // track.
        void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override {}

        //werbrtc::DataChannelObserver
        // The data channel state have changed.
        void OnStateChange() override {}
        //  A data buffer was successfully received.
        void OnMessage(const webrtc::DataBuffer& buffer) override;
        // The data channel's buffered_amount has changed.
        void OnBufferedAmountChange(uint64_t previous_amount) override {}

        template<class T>
        void ReadData(T& value, const uint8* &data);
    public:
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection;
    private:
        int32 id;
    };

}
