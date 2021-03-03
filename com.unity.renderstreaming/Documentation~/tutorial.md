# Tutorial 

In this page, we introduce the way to use samples published on Package Manager.

## Install package

> [!NOTE]
> In Unity `2020.2` and `2019.4`, there are differences about how to install Unity Render Streaming package so please pay attention to the Unity version you are using and follow the instructions below.

### Case of Unity 2019.4

Select `Window/Package Manager` in the menu bar.

![Install Package Manager from menu bar](images/install_select_packman_menu_unity2019.png)

Check Package Manager window, Click `Advanced` button and enable `Show preview packages`.

![Select show preview packages on advanced options](images/install_select_show_preview_packages.png)

Input `renderstreaming` to the search box at the top of the Package Manager window.

![Search renderstreaming package](images/install_search_renderstreaming_package.png)

Click `Install` button at the bottom left of the window, and will start install the package.

If an input system dialog box appears, click `Yes`

![Input system backend](images/input_system_backend.png)

### Case of Unity 2020.2

Select `Window/Package Manager` in the menu bar.

![Install Package Manager from menu bar](images/install_select_packman_menu_unity2020.png)

Check Package Manager window, Click `+` button and select `Add package from git URL...`.

![Select add package from git url](images/install_select_add_package_from_git_url.png)

Input the string below to the input field.

```
com.unity.renderstreaming@3.0.1-preview
```

The list of version string is [here](https://github.com/Unity-Technologies/com.unity.renderstreaming/tags). In most cases, the latest version is recommended to use.

 Click `Add` button, and will start install the package.

If an input system dialog box appears, click `Yes`

![Input system backend](images/input_system_backend.png)

## Install web application

Click on `Edit/Render Streaming/Download web app` menu item to download the application from [this page](https://github.com/Unity-Technologies/UnityRenderStreaming/releases).

![Download webapp](images/download_webapp.png)

When the select download folder window appears, click on `Select Folder` to download the file to the default folder

![Select Download folder](images/select_download_folder.png)

After the download is finished and a new explorer window is opened, click on `webserver.exe` to start 
Unity RenderStreaming web server.

![Web Server](images/webserver.png)

## Install samples

You can import Samples from the bottom of the `com.unity.renderstreaming` package in the PackageManager Window.

![Sample List](images/renderstreaming_samples.png)

## Play Unity

Open `WebBrowserInput` scene.

![Open WebBrowserInput scene](images/open_webbrowserinput_scene.png)

Enter play mode in Unity Editor.

![Play mode](images/play_mode.png)

Open your web browser and access `http://localhost`. You can see the top page. Click a `VideoPlayer Sample` link.

![Main page on the browser](images/browser_mainpage.png)

You can see the Unity scene on the browser, and control a camera in the Unity scene.

![Video player sample on the browser](images/browser_videoplayer.png)

## After tutorial

About general questions, please see [FAQ](faq.md) page. About the operation of inspectors, please see [Components settings](components.md) page. 

About options of web application, please see [The web application](webapp.md) page.

You can see more details for samples on the [Samples](samples.md) page.