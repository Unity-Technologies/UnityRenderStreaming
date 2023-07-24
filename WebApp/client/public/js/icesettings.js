// This code is referenced from webrtc sample.
// https://github.com/webrtc/samples/blob/gh-pages/src/content/peerconnection/trickle-ice/js/main.js

const servers = document.querySelector('select#servers');
const urlInput = document.querySelector('input#url');
const usernameInput = document.querySelector('input#username');
const passwordInput = document.querySelector('input#password');

const allServersKey = 'servers';

export function addServer() {
  const scheme = urlInput.value.split(':')[0];
  if (!['stun', 'stuns', 'turn', 'turns'].includes(scheme)) {
    alert(`URI scheme ${scheme} is not valid`);
    return;
  }

  // Store the ICE server as a stringified JSON object in option.value.
  const option = document.createElement('option');
  const iceServer = {
    urls: [urlInput.value],
    username: usernameInput.value,
    credential: passwordInput.value
  };
  option.value = JSON.stringify(iceServer);
  option.text = `${urlInput.value} `;
  const username = usernameInput.value;
  const password = passwordInput.value;
  if (username || password) {
    option.text += (` [${username}:${password}]`);
  }
  option.ondblclick = selectServer;
  servers.add(option);
  urlInput.value = usernameInput.value = passwordInput.value = '';
  writeServersToLocalStorage();
}

export function removeServer() {
  for (let i = servers.options.length - 1; i >= 0; --i) {
    if (servers.options[i].selected) {
      servers.remove(i);
    }
  }
  writeServersToLocalStorage();
}

export function reset() {
  window.localStorage.clear();
  document.querySelectorAll('select#servers option').forEach(option => option.remove());
  const serversSelect = document.querySelector('select#servers');
  setDefaultServer(serversSelect);
}

function selectServer(event) {
  const option = event.target;
  const value = JSON.parse(option.value);
  urlInput.value = value.urls[0];
  usernameInput.value = value.username || '';
  passwordInput.value = value.credential || '';
}

function setDefaultServer(serversSelect) {
  const option = document.createElement('option');
  option.value = '{"urls":["stun:stun.l.google.com:19302"]}';
  option.text = 'stun:stun.l.google.com:19302';
  option.ondblclick = selectServer;
  serversSelect.add(option);
}

function writeServersToLocalStorage() {
  const serversSelect = document.querySelector('select#servers');
  const allServers = JSON.stringify(Object.values(serversSelect.options).map(o => JSON.parse(o.value)));
  window.localStorage.setItem(allServersKey, allServers);
}

export function readServersFromLocalStorage() {
  document.querySelectorAll('select#servers option').forEach(option => option.remove());
  const serversSelect = document.querySelector('select#servers');
  const storedServers = window.localStorage.getItem(allServersKey);

  if (storedServers === null || storedServers === '') {
    setDefaultServer(serversSelect);
  } else {
    JSON.parse(storedServers).forEach((server) => {
      const o = document.createElement('option');
      o.value = JSON.stringify(server);
      o.text = server.urls[0];
      o.ondblclick = selectServer;
      serversSelect.add(o);
    });
  }
}

export function getServers() {
  const storedServers = window.localStorage.getItem(allServersKey);

  if (storedServers === null || storedServers === '') {
    return [{ urls: ['stun:stun.l.google.com:19302'] }];
  }
  else {
    return JSON.parse(storedServers);
  }
}
