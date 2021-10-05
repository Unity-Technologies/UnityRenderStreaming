import * as Config from "./config.js";

const startupDiv = document.getElementById("startup");
startupDiv.innerHTML = "<h3>Server Configuration</h3>";

const displayConfig = async () => {
  const res = await Config.getServerConfig();
  if (res.useWebSocket) {
    startupDiv.innerHTML += "<li>Signaling Protocol : <b>WebSocket</b></li>";
  } else {
    startupDiv.innerHTML += "<li>Signaling Protocol : <b>HTTP</b></li>";
  }

  const mode = res.startupMode.replace(/^./, res.startupMode[0].toUpperCase());
  startupDiv.innerHTML += `<li>Signaling Mode : <b>${mode}</b></li>`;
};

displayConfig();
