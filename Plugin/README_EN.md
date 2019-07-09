# Building the plugin

### Embedding libwebrtc

The plugin relies on **libwebrtc**, so building it requires a static link to libwebrtc. `webrtc-win.zip` can be found on the Github Release page.

 <img src="../Packages/com.unity.webrtc/Documentation~/images/libwebrtc_github_release.png" width=600 align=center>

Extract the files from the zip, and place them in the Plugin folder.

<img src="../Packages/com.unity.webrtc/Documentation~/images/deploy_libwebrtc.png" width=500 align=center>

### Build

version 1.0 is currently built with **Visual Studio 2017**. Version 1.2 and after may migrate to **CMake**.

### Debug

Runtime debugging requires changes to the WebRTCPlugin project's properties. 

Set the Output Directory to `Packages\com.unity.webrtc\Runtime\Plugins\x86_64`.

<img src="../Packages/com.unity.webrtc/Documentation~/images/outputdirectory_config_vs2017.png" width=600 align=center>

Set the Unity.exe file path under Command, and set the project path under Command Arguments.

<img src="../Packages/com.unity.webrtc/Documentation~/images/command_config_vs2017.png" width=600 align=center>
