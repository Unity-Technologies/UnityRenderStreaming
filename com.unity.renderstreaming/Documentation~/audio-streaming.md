# Audio Streaming

`AudioSender` class and `AudioReceiver` class are the base classes for sending and receiving audio. And various components are implemented using inheritance relationships with these class. Components are provided for the sender and receiver. There are multiple components for the sender side, but `AudioClipReceiver` can receive from them the sent video.

> [!NOTE] 
> There is currently a limitation in the audio codec selection. Currently, only **Opus** is used for the audio codec.

## `AudioClipSender` component

This component streams the `AudioListener` component's audio rendering results. 

![AudioClipSender inspector](images/audioclipsender_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Audio Source** | `AudioSource` instance for sending audio | None |

## `MicrophoneSender` component

This component streams the `AudioListener` component's audio rendering results. 

![MicrophoneSender inspector](images/microphonesender_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Audio Source** | `AudioSource` instance for sending audio | None |
| **Device Index** | An index of the list of available microphone devices | 0 |

An index of the list of available microphone devices


## `AudioClipReceiver` component

This component streams the `AudioListener` component's audio rendering results. 

![AudioClipReceiver inspector](images/audioclipreceiver_inspector.png)