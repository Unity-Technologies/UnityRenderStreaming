# Changelog
All notable changes to com.unity.renderstreaming package will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [3.1.0-exp.9] - 2024-12-13

### Changed
- deps: use com.unity.webrtc@3.0.0-pre.8
- doc: improve the API documentation of VideoCodecInfo
- doc: improve the API documentation of AudioCodecInfo
- doc: improve the API documentation of VideoStreamSender
- doc: improve the API documentation of VideoStreamReceiver
- doc: improve the API documentation of InputSender
- doc: improve the API documentation of InputReceiver
- doc: improve the API documentation of AudioStreamSender
- doc: improve the API documentation of AudioStreamReceiver
- doc: improve the API documentation of SignalingManager

## [3.1.0-exp.8] - 2023-11-30

### Changed

- Upgrade the version of WebRTC package `3.0.0-pre.7`.

## [3.1.0-exp.7] - 2023-08-09

### Added

- Added configurable logger to enable users to customize logging for their environment.

### Changed

- Upgrade the version of WebRTC package `3.0.0-pre.6`.
- Add `AudioStreamSender.loopback` property.

### Fixed

- Fixed error on HTTP signaling when using short polling interval.
- Fixed `SignalingManager` so that ICE server configurations aren't effected.
- Added a workaround to fix an issue where `InputField` wasn't worked when entering characters from browsers or a `Receiver` scene of package sample.

### Removed

- Removed Furioos Integration.

## [3.1.0-exp.6] - 2023-02-24

### Added

- Support Automatic Streaming.
- Support Render Streaming Wizard.
- Support Command line arguments.
- Add Render Streaming Settings in the Project Settings window.

### Changed

- Rename classes.
  - `RenderStreaming` > `SignalingManager`
- Websocket is in default for signaling protocol instead of HTTP polling.
- Changed a unit of the HTTP polling interval, second to millisecond.

## [3.1.0-exp.5] - 2023-01-16

### Changed

- Upgrade the version of Input System package `1.4.4`.
- Upgrade the version of WebRTC package `3.0.0-pre.4`.

### Fixed

- Fix incorrect mouse position in full-screen web player sample.

## [3.1.0-exp.4] - 2022-10-06

### Added

- Added APIs to select codec for audio/video streaming.
- Added APIs to configure settings like bitrate for audio/video streaming.

### Changed

- Upgraded the version of Input System package `1.4.1`.
- Upgraded the version of WebRTC package `2.4.0-exp.11`.
- Merge features of three classes to `VideoStreamSender`.
  - `MicrophoneStreamSender`
  - `AudioStreamReceiver`
  - `ScreenStreamSender`
- Improve user interface on the inspector window of components below.
  - `VideoStreamSender`
  - `VideoStreamReceiver`
  - `AudioStreamSender`
  - `AudioStreamReceiver`

### Fixed

- Reduce CPU loads of the rendering thread by updating WebRTC package.
- Fixed the crash when video resolutions higher than 1080p with hardware encoding.

### Removed

- No longer support Unity 2019.4.

## [3.1.0-exp.3] - 2022-03-01

### Changed

- Upgraded the version of Input System package `1.3.0`.
- Upgraded the version of WebRTC package `2.4.0-exp.6`.
- Added microphone device option for streaming audio in the "Bidirectional" sample.
- Integrate AR Foundation Sample (only use AR Foundation version 4.1.9, deprecated version 2.1)

### Fixed

- Fixed the worker thread is not closed when stopping the signaling client.
- Fixed keyboard input sending only sends one key at a time from browser.
- Fixed video flipped vertically when uging GLES/GLCore graphics API.
- Fixed the crach when encoding the high resolution on macOS using HWA.
- Fixed the gap of the input position between sender and receiver.
- Fixed the unexpected behaviour of input after using Gyro sample.

## [3.1.0-exp.2] - 2021-12-01

### Added

- Added audio streaming receiver.
- Added the `Multiplay` scene into samples.
- Added components for streaming.
  - `MicrophoneStreamSender`
  - `AudioStreamReceiver`
  - `ScreenStreamSender`
- Added components for controlling user input.
  - `InputSender`
  - `InputReceiver`

### Changed

- Upgraded WebRTC package to `2.4.0-exp.4`.
- Upgraded Input System package to `1.0.2`.
- Renamed components.
  - `CameraStreamer` -> `CameraStreamSender`
  - `ReceiveVideoViewer` -> `VideoStreamReceiver`
  - `WebCamStreamer` -> `WebCamStreamSender`
  - `AudioStreamer` -> `AudioStreamSender`
  - `InputSystemChannnelSender` -> `InputSender`
  - `InputSystemChannnelReceiver` -> `InputReceiver`
- Improved controlling user input from browsers.
- Updated manual documents.

### Fixed

- Fixed resource leak of sender stream tracks.

## [3.1.0-exp.1] - 2021-06-15

### Added

- Android platform support
- Added the Menu scene into samples

### Changed

- Upgraded WebRTC package to `2.4.0-exp.3`.
- Added drop-down list UI for selecting the video capture device in the `Bidirectional` scene
- Changed the signaling process for exchanging SDPs

### Fixed

- Fixed errors when disposing the scene while streaming video
- Fixed the long processing time when disposing the signaling instance

## [3.0.1-preview] - 2021-03-04

### Fixed

- Fixed a script error when importing the package sample

### Changed

- `Camerastreamer` uses a `TargetTexture` on the `Camera` component if the `RenderTexture` is attached on the `Camera`.

## [3.0.0-preview] - 2021-03-02

Version 3.0.0 has a big change in the package design. This mainly addresses moving scripts from the sample folder to Package Manager.

### Added

- Supported iOS platform.
- Added new samples.
- Added a documentation for samples.
- Added components (`InputSystemChannelSender`, `InputSystemChannelReceiver`, `WebBrowserInputChannelReceiver`, `SingleConnection` and `Broadcast`).

### Changed

- Moved scripts from the sample folder to Package Manager.
- Upgrading WebRTC package to `2.3.3-preview`.

## [2.2.2-preview] - 2020-12-15

### Changed

- Updated unitypackage files of the package samples

## [2.2.1-preview] - 2020-11-19

### Added

- Added `ReceiveVideo` scene in the sample project
- Added documentation which explains how to customize webapp

### Changed

- Upgrading WebRTC to `2.2.1-preview`

### Fixed

- Fixed `NullPointerException` occurred when `UIController` instance is not attached `SimpleCameraController` component
- Fixed behavior of the editor menu

## [2.2.0-preview] - 2020-11-09

### Changed

- Upgrading WebRTC to `2.2.1-preview`


## [2.1.1-preview] - 2020-09-11

### Changed

- Upgrading WebRTC to `2.1.1-preview`
- Changed not to customize SDP to make stabilize bitrate of video streaming
- Erase Japanese documentation due to migrating to internal translation system

## [2.1.0-preview] - 2020-08-28

### Added

- Add a sample used Built-in Render Pipeline as graphics render pipeline

### Changed

- Upgrading WebRTC to `2.1.0-preview`

### Fixed

- Fixed the issue when pressing multiple buttons on a keyboard

## [2.0.2-preview] - 2020-05-14

### Changed

- Upgrading input system to `1.0.0`
- Upgrading WebRTC to `2.0.2-preview`

### Fixed

- Fix documents

## [2.0.1-preview] - 2020-05-01

### Fixed

- Fixed versioning issue

## [2.0.0-preview] - 2020-04-30

### Changed

- Upgrading input system to `1.0.0-preview.6`
- Upgrading WebRTC to `2.0.0-preview`
- Update URP/HDRP sample

## [1.2.2-preview] - 2020-03-06

### Fixed

- Fix build error when build standalone

## [1.2.1-preview] - 2020-02-28

### Changed

- Upgrading WebRTC to `1.1.1-preview`

## [1.2.0-preview] - 2020-02-25

### Added

- Adding `AudioStreamer` class
- Adding the flag to select encoder

### Changed

- Upgrading Unity version to `2019.3`
- Upgrading HDRP to `7.1.7`
- Upgrading input system to `1.0.0-preview.1`
- Upgrading WebRTC to `1.1.0-preview`

### Fixed

- Display the mouse pointer in the correct position
- Try downloading webserver from a known version if the current version doesn't exist

## [1.1.2-preview] - 2019-09-20

Fixed

- Fix image links for Japanese documentation on Unity website

## [1.1.1-preview] - 2019-09-14

### Fixed

- Fix image links for documentation on Unity website

## [1.1.0-preview] - 2019-09-13

### Added

- Adding menu to download web app

### Changed

- Upgrading input system to `0.9.4-preview`
- Upgrading HDRP to `5.16.1-preview`
- Adding keyboard event to reset camera position

## [1.0.0-preview] - 2019-08-22

### Added

- Eliminates the necessity of user interaction when installing an HDRP sample
- Adding a check to automatically set HDRP render pipeline settings on initial setup

### Changed

- Refactor `RenderStreamingHDRPAutomator` to use json instead of ScriptableObject

### Fixed

- Blit Render Texture using customRender instead of `Rendering.RenderPipeline.BeginCameraRendering`

## [0.2.0-preview] - 2019-07-30

### Changed

- Removed the direct dependency to HDRP package

### Added

- Added a unitypackage file containing an HDRP sample, and a menu editor to install it

## [0.1.0-preview] - 2019-07-22

- Initial Release

### Added

- Added a sample project which uses RenderStreaming tech.
