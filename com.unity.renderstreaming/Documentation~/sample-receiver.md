# Receiver Sample

![ReceiveVideo Sample](images/renderstreaming_receivevideosample.png)

This is a sample for receiving VideoTrack of MediaStream. 
Click `SendOfferButton` to create a new `PeerConnection` and send the Offer to server.
You can also use any `ConnectionId`. In that case, enter the `ConnectionId` in the `InputField`.
This scene uses `ReceiveVideoViewer` component to receive a VideoTrack of MediaStream.
Please see [Components settings](components.md) page for details.

> [!NOTE]
> In version 2.2, the only video codec that can be received is VP8/VP9. If errors occur, check the video codec settings on the sending side.