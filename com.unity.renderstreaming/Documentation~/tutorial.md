# Tutorial

In this tutorial, we introduce the way to use samples published on Package Manager.
For template installation tutorial, please see the document on the [GitHub repository](https://github.com/Unity-Technologies/UnityRenderStreaming).

## Install package

Open an existing or create a new project
Click on the `Window` Menu and open `Package Manager`. Then, click on `Advanced` and make sure `Show preview packages` is checked

![Show Preview Packages](images/show_preview_packages.png)

Make sure to search `All packages`, and type `RenderStreaming` in the text box.

![Render Streaming Package](images/renderstreaming_package.png)

Click `Install` on the bottom right.

If an input system dialog box appears, click `Yes`

![Input system backend](images/input_system_backend.png)

## Install HDRP sample

Click on `Import in project` to import the HDRP sample.
   This will also automatically install the HDRP package *(com.unity.render-pipelines.high-definition)*.

![HDRP sample](images/hdrp_sample.png)

When this `Import Unity Package` dialog box appears, click `Import` to import all assets.

![HDRP Unitypackage](images/hdrp_unitypackage.png)

After all the import process has been finished, open `Assets/SimpleScene` in the Project View.

![Simple Scene](images/simplescene.png)

## Install web application

Click on `Edit/Render Streaming/Download web app` menu item to download [our web server](https://github.com/Unity-Technologies/UnityRenderStreaming/releases).

![Download webapp](images/simplescene.png)

When the select download folder window appears, click on `Select Folder` to download the file to the default folder

![Select Download folder](images/select_download_folder.png)

After the download is finished and a new explorer window is opened, click on `webserver.exe` to start 
Unity RenderStreaming web server.

![Web Server](images/webserver.png)

## Play Unity

Enter play mode in Unity

![Play mode](images/play_mode.png)

Open your browser and type `localhost` on the URL bar. Then click on the play button in the middle. 
The view in Unity should then appear on your browser.

![Browser](images/browser.png)

## After tutorial

About general questions, please see [FAQ](faq.md) page. About the operation of inspectors, please see [Components settings](components.md) page. About options of web application, please see [The web application](webapp.md) page.