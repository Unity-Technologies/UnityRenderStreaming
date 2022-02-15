# Changelog
All notable changes to the render streaming template package will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [3.1.0-exp.2] - 2022-02-15

### Changed

- Upgrading Unity version to `2020.3`
- Upgrading RenderStreaming package to `3.1.0-exp.2`.
- Upgrading HDRP package to `10.8.1`

## [3.0.0] - 2021-03-02

Version 3.0.0 has a big change in the package design. This mainly addresses moving scripts from template to package.

### Changed

- Upgrading RenderStreaming package to `3.0.1-preview`.

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

### Changed

- Upgrading WebRTC to `2.2.0-preview`

## [2.1.1] - 2020-09-11

### Added

- Added methods `ChangeBitrate` and `ChangeFramerate` to `CameraStreamer` class
- Added methods `ChangeBitrate` and `ChangeFramerate` to `WebCamStreamer` class

### Changed

- Upgrading WebRTC to `2.1.1-preview`

## [2.1.0] - 2020-08-28

### Changed

- Upgrading WebRTC to `2.1.0-preview`
- Upgrading Unity version to `2019.4`

## [2.0.1] - 2020-05-14

### Changed

- Upgrading input system to `1.0.0`
- Upgrading WebRTC to `2.0.2-preview`

## [2.0.1] - 2020-05-01

### Fixed

- Fixed versioning issue

## [2.0.0] - 2020-04-30

### Added

- Multi camera support
- Multi controller support
- Gamepad support
- Websocket support

### Changed

- Upgrading HDRP to `7.3.1`
- Upgrading input system to `1.0.0-preview.6`

## [1.2.2] - 2020-03-06

### Fixed

- Fix build error when build standalone

## [1.2.1] - 2020-02-28

### Changed

- Upgrading WebRTC to `1.1.1-preview`

## [1.2.0] - 2020-02-25

### Added

- Adding AudioStreamer class
- Adding the flag to select encoder
- Adding the Universal Render Pipeline sample

### Changed

- Upgrading Unity version to `2019.3`
- Upgrading HDRP to `7.1.7`
- Upgrading input system to `1.0.0-preview.1`
- Upgrading WebRTC to `1.1.0-preview`

### Fixed

- Display the mouse pointer in the correct position
- Try downloading webserver from a known version if the current version doesn't exist

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

### Fixed

- Fixed Readme of the template project

## [1.0.0] - 2019-08-22

### Added

- Added audio sample
- Added streaming size parameter to RenderStreaming inspector

### Fixed

- Ice server list items on renderstreaming inspector not displayed

## [0.2.0] - 2019-07-30

### Added

- Add new package com.unity.renderstreaming
- Add UI to display keyboard typing

### Changed

- Don't exit the Application when esc is pressed and don't lock the cursor
- Add docs for TURN server integration 
- Make several minor modifications

### Fixed

- Use the default registry instead of the staging 
- Fix bugs that not worked streaming on Safari MacOS

## [0.1.0] - 2019-07-02

- Initial Release