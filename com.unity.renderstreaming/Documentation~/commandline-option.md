# Command line arguments

Unity Render Streaming supports configuration changes via command line arguments. you can change settings to match the deployment environments. Command line arguments work with Windows, macOS, and Linux runtime applications. Mobile platforms aren't supported.

> [!WARNING]
> **-batchmode** and **-nographics** options don't work with Unity Render Streaming.

Command line arguments are evaluated when initializing the [**SignalingManager**](streaming-management.md) component. When command line arguments are evaluated in the **SignalingManager** component, it overrides the settings specified in the [Settings Window](settings.md). If you wish to ignore the command line options, specify **Evaluate command line arguments** off in the SignalingManager inspector. If a command line arguments contain invalid values, the contents of the configuration window are evaluated, instead of the arguments of the command line.

Use a command line with the following arguments to start your standalone app.

| Argument | Description |
| --- | --- |
| `signalingType <value>` | Specify the signaling type. **websocket** or **http**. |
|  `signalingUrl  <value>` | Specify the URL of signaling server. |
| `iceServerUrl <value>` | Set the URL of STUN/TURN server. |
| `iceServerUsername <value>` | Specify the user name of the TURN server. |
| `iceServerCredential <value>` | Specify the password of the TURN server. |
| `iceServerCredentialType <value>` | Specify the credential type for the TURN server. **password** or **oauth**. |
| `pollingInterval  <value>` | Specify the polling interval in milliseconds to the signaling server. This value is used when using **http** for the signaling type. |
| `importJson <path>` | Specify the path of the JSON file. |

## Import JSON

You can change the configuration via the JSON file when using **-importJson** option in command line. The JSON file must be written in the following format:

```javascript
{
    "signalingType": "websocket",
    "signalingUrl":  "ws://192.168.11.11",
    "iceServers": [{
        "urls": [
            "stun:stun.l.google.com:19302"
        ],
        "username": "user",
        "credential": "pass",
        "credentialType": "password"
    }],
    "pollingInterval": "5000"
}
```

## Command line examples

Change the signaling type to **http**:

```
RenderStreaming.exe -signalingType http
```

Change the URL of the signaling server:

```
RenderStreaming.exe -signalingUrl http://192.168.10.1
```

Specify multiple URLs of STAN servers:

```
RenderStreaming.exe -iceServerUrl stun:stun.l.google.com:19302 -iceServerUrl stun:stun.l.google.com:19303
```

Specify the URL, username, and password of the TURN server:

```
RenderStreaming.exe -iceServerUrl turn:192.168.10.10:3478?transport=udp -iceServerUsername username -iceServerCredentialType password -iceServerCredential pass
```

Specify the path of the JSON file:

```
RenderStreaming.exe -importJson config.json
```
