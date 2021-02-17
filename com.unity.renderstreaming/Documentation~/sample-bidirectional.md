# Bidirectional Sample

![1on1 Sample](images/renderstreaming_1on1sample.png)

This is a sample of bidirectional video send/receive.
This sample is intended for use in [WebApplication](webapp.md) in Private mode. (It does not work in Public mode.)
Please add the `-m private` argument on starting the Web Application.
Enter the same `ConnectionId` as the person you want to communicate with in `Input Field`.
Click `SetUp` to create a new` PeerConnection` corresponding to the entered `ConnectionId` and connects to the server.
If other side is already connected, Offer will be sent automatically.
Click `HangUp` to end the current communication.