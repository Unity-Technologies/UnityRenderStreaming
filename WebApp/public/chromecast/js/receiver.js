import { VideoPlayer } from "./video-player.js";
import { getServerConfig } from "../../js/config.js";
import * as Logger from "./logger.js";

const context = cast.framework.CastReceiverContext.getInstance();
const playerManager = context.getPlayerManager();

const castDebugLogger = cast.debug.CastDebugLogger.getInstance();

// Enable debug logger and show a 'DEBUG MODE' overlay at top left corner.
castDebugLogger.setEnabled(false);
castDebugLogger.showDebugLogs(false);

export function getCastDebugLogger() {
  return castDebugLogger;
}

let videoPlayer;
let useWebSocket;

// https://stackoverflow.com/questions/61967468/chrome-android-not-using-all-codec-options-in-sdp-offer/62163448#62163448
function checkH264(count) {
  if (count > 0) {
    Logger.log('Getting video codec capabilities');
    let capabilities = JSON.stringify(RTCRtpSender.getCapabilities('video').codecs);
    Logger.log(capabilities);
    if (capabilities.toLowerCase().includes('video/h264')) {
      Logger.log('H.264 support found. Ready to proceed now! =)');
      // Proceed to SDP offer creation...
    } else {
      setTimeout(checkH264, 1000, count - 1);
    }
  } else {
    Logger.warn('H.264 support not found');
    // Proceed with caution. SDP offer may not contain H.264...
  }
}

function initialize() {
  Logger.debug("initialize");
  Logger.log('Waiting for H.264 codec support');
  checkH264(20);
  setup();
}

window.addEventListener('load', initialize);

window.document.oncontextmenu = function () {
    return false;     // cancel default menu
}

window.addEventListener('resize', function () {
    videoPlayer.resizeVideo();
}, true);

async function setup() {
  const res = await getServerConfig();
  Logger.debug("useWebSocket " + res.useWebSocket);
  useWebSocket = res.useWebSocket;
  Logger.debug( "startupMode " + res.startupMode);
  showWarningIfNeeded(res.startupMode);
  play();
}

function showWarningIfNeeded(startupMode) {
    if (startupMode === "private") {
      Logger.warn("This sample is not working on Private Mode.")
    }
}

function play() {
    const playerDiv = document.getElementById('player');

    // add video player
    const elementVideo = document.createElement('video');
    elementVideo.id = 'Video';
    elementVideo.style.touchAction = 'none';
    playerDiv.appendChild(elementVideo);

    setupVideoPlayer([elementVideo]).then(value => videoPlayer = value);


  //openFullscreen()
}

function openFullscreen() {
  if (document.documentElement.requestFullscreen) {
    document.documentElement.requestFullscreen();
  } else if (document.documentElement.webkitRequestFullscreen) { /* Safari */
    document.documentElement.webkitRequestFullscreen();
  } else if (document.documentElement.msRequestFullscreen) { /* IE11 */
    document.documentElement.msRequestFullscreen();
  }
}

async function setupVideoPlayer(elements) {
  Logger.debug('setupVideoPlayer: ' + elements[0]);
  const videoPlayer = new VideoPlayer(elements);
  await videoPlayer.setupConnection(useWebSocket);
  videoPlayer.ondisconnect = onDisconnect;
  Logger.debug('setupVideoPlayer videoPlayer: ' + videoPlayer);

  return videoPlayer;
}

function onDisconnect() {
  Logger.debug("onDisconnect");
  const playerDiv = document.getElementById('player')
  clearChildren(playerDiv);
  videoPlayer = null;
}

function clearChildren(element) {
    while (element.firstChild) {
        element.removeChild(element.firstChild);
    }

}

/*
function makeRequest (method, url) {
    return new Promise(function (resolve, reject) {
        let xhr = new XMLHttpRequest();
        xhr.open(method, url);
        xhr.onload = function () {
            if (this.status >= 200 && this.status < 300) {
                resolve(JSON.parse(xhr.response));
            } else {
                reject({
                    status: this.status,
                    statusText: xhr.statusText
                });
            }
        };
        xhr.onerror = function () {
            reject({
                status: this.status,
                statusText: xhr.statusText
            });
        };
        xhr.send();
    });
}

playerManager.setMessageInterceptor(
    cast.framework.messages.MessageType.LOAD,
    request => {
        return new Promise((resolve, reject) => {
            // Fetch content repository by requested contentId
            makeRequest('GET', 'https://storage.googleapis.com/cpe-sample-media/content.json').then(function (data) {
                let item = data[request.media.contentId];
                if(!item) {
                    // Content could not be found in repository
                    reject();
                } else {
                    // Add metadata
                    let metadata = new
                    cast.framework.messages.GenericMediaMetadata();
                    metadata.title = item.title;
                    metadata.subtitle = item.author;

                    request.media.contentUrl = item.stream.hls;
                    request.media.contentType = 'application/x-mpegurl';
                    request.media.hlsSegmentFormat = cast.framework.messages.HlsSegmentFormat.FMP4;
                    request.media.hlsVideoSegmentFormat = cast.framework.messages.HlsVideoSegmentFormat.FMP4;


                    request.media.metadata = metadata;

                    // Resolve request
                    resolve(request);
                }
            });
        });
    });*/


playerManager.addEventListener(
  cast.framework.RemotePlayerEventType.ANY_CHANGE, function(event) {
    Logger.debug(event);

  });

context.start();

