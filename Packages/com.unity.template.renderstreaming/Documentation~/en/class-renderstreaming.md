# The Render Streaming Class

**Render Streaming** includes the following features.

- Communication with signaling servers
- Streaming input from cameras 
- Associating browser input with events

<img src="/Users/kazuki/UnityRenderStreaming/Packages/com.unity.template.renderstreaming/Documentation~/images/renderstreaming_inspector.png" width=400 align=center>

## Properties

| Parameter                | Description                                              | Default                          |
| ------------------------ | -------------------------------------------------------- | -------------------------------- |
| URL Signaling            | Set the signaling server URL                      | http://localhost                 |
| URLs Ice Server          | Set a list of STUN/TURN servers                    | \[stun:stun.l.google.com:19302\] |
| Interval                 | Set the polling frequency (in seconds) to the signaling server | 5                                |
| Capture Camera           | Set the streaming camera                     |                                  |
| Array Button Click Event | Register browser button click events           |                                  |

## Details

