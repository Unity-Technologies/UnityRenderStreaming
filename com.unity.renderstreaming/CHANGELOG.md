# Changelog
All notable changes to com.unity.renderstreaming package will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [3.0.0] - 2021-02-19

Version 3.0.0 has a big change in the package design. This mainly addresses moving scripts from the sample folder to Package Manager.

### Added

- Supported iOS platform.
- Added `AR Foundation` sample.
- Added `Gyro` sample.

### Changed

- Moved scripts from the sample folder to Package Manager.
- Upgrading WebRTC package to `2.3.2-preview`.

## [2.2.2] - 2020-12-15

### Changed

- Updated unitypackage files of the package samples

## [2.2.1] - 2020-11-19

### Added

- Added `ReceiveVideo` scene in the sample project
- Added documentation which explains how to customize webapp

### Changed

- Upgrading WebRTC to `2.2.1-preview`

### Fixed

- Fixed `NullPointerException` occurred when `UIController` instance is not attached `SimpleCameraController` component
- Fixed behavior of the editor menu

## [2.2.0] - 2020-11-09

### Changed

- Upgrading WebRTC to `2.2.1-preview`


## [2.1.1] - 2020-09-11

### Changed

- Upgrading WebRTC to `2.1.1-preview`
- Changed not to customize SDP to make stabilize bitrate of video streaming
- Erase Japanese documentation due to migrating to internal translation system

## [2.1.0] - 2020-08-28

### Added 

- Add a sample used Built-in Render Pipeline as graphics render pipeline

### Changed

- Upgrading WebRTC to `2.1.0-preview`

### Fixed

- Fixed the issue when pressing multiple buttons on a keyboard

## [2.0.2] - 2020-05-14

### Changed

- Upgrading input system to `1.0.0`
- Upgrading WebRTC to `2.0.2-preview`

### Fixed

- Fix documents

## [2.0.1] - 2020-05-01

### Fixed

- Fixed versioning issue

## [2.0.0] - 2020-04-30

### Changed

- Upgrading input system to `1.0.0-preview.6`
- Upgrading WebRTC to `2.0.0-preview`
- Update URP/HDRP sample

## [1.2.2] - 2020-03-06

### Fixed

- Fix build error when build standalone

## [1.2.1] - 2020-02-28

### Changed

- Upgrading WebRTC to `1.1.1-preview`

## [1.2.0] - 2020-02-25

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

## [1.1.2] - 2019-09-20

Fixed

- Fix image links for Japanese documentation on Unity website

## [1.1.1] - 2019-09-14

### Fixed

- Fix image links for documentation on Unity website

## [1.1.0] - 2019-09-13

### Added

- Adding menu to download web app

### Changed

- Upgrading input system to `0.9.4-preview`
- Upgrading HDRP to `5.16.1-preview`
- Adding keyboard event to reset camera position

## [1.0.0] - 2019-08-22

### Added

- Eliminates the necessity of user interaction when installing an HDRP sample
- Adding a check to automatically set HDRP render pipeline settings on initial setup

### Changed

- Refactor `RenderStreamingHDRPAutomator` to use json instead of ScriptableObject

### Fixed

- Blit Render Texture using customRender instead of `Rendering.RenderPipeline.BeginCameraRendering`

## [0.2.0] - 2019-07-30

### Changed

- Removed the direct dependency to HDRP package

### Added

- Added a unitypackage file containing an HDRP sample, and a menu editor to install it

## [0.1.0] - 2019-07-22

- Initial Release

### Added

- Added a sample project which uses RenderStreaming tech.