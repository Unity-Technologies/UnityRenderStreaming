# Changelog
All notable changes to com.unity.renderstreaming package will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [1.2.2] - 2020-03-06

- Fixed: Fix build error when build standalone

## [1.2.1] - 2020-02-28

- Changed: Upgrading WebRTC to 1.1.1-preview

## [1.2.0] - 2020-02-25

- Added: Adding AudioStreamer class
- Added: Adding the flag to select encoder
- Changed: Upgrading Unity version to 2019.3
- Changed: Upgrading HDRP to 7.1.7
- Changed: Upgrading input system to 1.0.0-preview.1
- Changed: Upgrading WebRTC to 1.1.0-preview
- Fixed: Display the mouse pointer in the correct position
- Fixed: Try downloading webserver from a known version if the current version doesn't exist

## [1.1.2] - 2019-09-20

- Fixed: Fix image links for Japanese documentation on Unity website

## [1.1.1] - 2019-09-14

- Fixed: Fix image links for documentation on Unity website

## [1.1.0] - 2019-09-13

- Added: Adding menu to download web app
- Changed: Upgrading input system to 0.9.4-preview
- Changed: Upgrading HDRP to 5.16.1-preview
- Changed: Adding keyboard event to reset camera position

## [1.0.0] - 2019-08-22

- Added: Eliminates the necessity of user interaction when installing an HDRP sample
- Added: Adding a check to automatically set HDRP render pipeline settings on initial setup
- Changed: Refactor RenderStreamingHDRPAutomator to use json instead of ScriptableObject
- Fixed: Blit Render Texture using customRender instead of beginCameraRendering

## [0.2.0] - 2019-07-30

- Changed: Removed the direct dependency to HDRP package
- Added: Added a unitypackage file containing an HDRP sample, and a menu editor to install it

## [0.1.0] - 2019-07-22

- Initial Release
- Added: Added a sample project which uses RenderStreaming tech.