# Changelog
All notable changes to com.unity.renderstreaming package will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

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