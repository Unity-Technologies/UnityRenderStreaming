import process from "process";

export function waitFor(conditionFunction) {

  const poll = resolve => {
    if (conditionFunction()) resolve();
    else setTimeout(() => poll(resolve), 100);
  };

  return new Promise(poll);
}

export async function sleep(milisecond) {
  return new Promise(resolve => setTimeout(resolve, milisecond));
}

export function serverExeName() {
  switch (process.platform) {
    case 'win32':
      return 'webserver.exe';
    case 'darwin':
      return 'webserver_mac';
    case 'linux':
      return 'webserver';
    default:
      return null;
  }
}

export function getUniqueId() {
  return new Date().getTime().toString(16) + Math.floor(1000 * Math.random()).toString(16);
}
