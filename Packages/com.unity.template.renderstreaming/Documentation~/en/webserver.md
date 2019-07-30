# The Web Application

The **Web application** 

- handles signaling between Unity and the Web browser
- is the location of the Web page 

## The Web Client

The **Render Streaming** web application uses WebRTC technologies, which means that your browser must also support WebRTC. See [this page](https://caniuse.com/#search=webrtc) for details on which browsers support WebRTC.

The following browsers are compatible:

- Chrome
- Safari
- Firefox
- iOS Safari
- Chrome for Android

> [!NOTE]
> In **Safari** and **iOS Safari**, WebRTC features cannot be used with **http**. Instead, **https** must be used.

## The Web Server

 The .exe file is located in the `Assets/bin~/` folder. Run it from the command line.

<img src="../images/launch_webserver_explorer.png" width=500 align=center>

### Command Options

|Option|Details|Default|
|-------|-----|-------|
|-h --help|Show the help menu||
|-p —port \<number\>|Set the port number|80|
|-s --secure|Use https||
|-k —keyfile \<path\>|Designate the private key file to use with https|server.key|
|-c —certfile \<path\>|Designate the certification file to use with https|server.cert|

### Command Examples

Use the following command to start the server as http. Port 80 will be used.

```shell
.\webserver
```

This command will run the server as https. Port 443 will be used. A certificate (`server.cert`) and a key (`server.key`) must also be set up ([details](../en/https.md)).

```shell
.\webserver -s -p 443
```

Use this command to display the help guide. 

```shell
.\webserver -h
```
