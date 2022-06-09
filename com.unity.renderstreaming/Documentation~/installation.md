# Installing The Package

> [!NOTE]
> In Unity **2020.3** and **2019.4**, there are differences about how to install Unity Render Streaming package so please pay attention to the Unity version you are using and follow the instructions below.

## Case of Unity 2019.4

Select **Window > Package Manager** in the menu bar.

![Install Package Manager from menu bar](images/install_select_packman_menu_unity2019.png)

Check Package Manager window, Click `Advanced` button and enable `Show preview packages`.

![Select show preview packages on advanced options](images/install_select_show_preview_packages.png)

Input `renderstreaming` to the search box at the top of the Package Manager window.

![Search renderstreaming package](images/install_search_renderstreaming_package.png)

Click `Install` button at the bottom left of the window, and will start install the package.

If an input system dialog box appears, click `Yes`

![Input system backend](images/input_system_backend.png)

## Case of Unity 2020.3

Select **Window > Package Manager** in the menu bar.

![Install Package Manager from menu bar](images/install_select_packman_menu_unity2020.png)

Check Package Manager window, Click `+` button and select `Add package from git URL...`.

![Select add package from git url](images/install_select_add_package_from_git_url.png)

Input the string below to the input field.

```
com.unity.renderstreaming@3.1.0-exp.4
```

The list of version string is [here](https://github.com/Unity-Technologies/UnityRenderStreaming/tags). In most cases, the latest version is recommended to use.

 Click `Add` button, and will start install the package.

If an input system dialog box appears, click `Yes`

![Input system backend](images/input_system_backend.png)

## Next Step

After installing the package in the Unity Editor, you can launch the web application for signaling. Please see the next page [Launching The Web Application](launch-webapp.md).