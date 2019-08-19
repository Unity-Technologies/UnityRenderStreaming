import { VideoPlayer } from "./video-player.js";
import { registerKeyboardEvents, registerMouseEvents, sendClickEvent } from "./register-events.js";

let playButton;
let videoPlayer;

showPlayButton();

function showPlayButton() {
  if (!document.getElementById('playButton')) {
    let elementPlayButton = document.createElement('img');
    elementPlayButton.id = 'playButton';
    elementPlayButton.src = 'images/Play.png';
    elementPlayButton.alt = 'Start Streaming';
    playButton = document.getElementById('player').appendChild(elementPlayButton);
    playButton.addEventListener('click', onClickPlayButton);
  }
}

function onClickPlayButton() {

  playButton.style.display = 'none';

  const playerDiv = document.getElementById('player');

  // add video player
  let elementVideos = [];
  for (let i=0; i<2; i++)
  {
    const elementVideo = document.createElement('video');
    elementVideo.id = "Video"+i;
    elementVideo.style.touchAction = 'none';
    playerDiv.appendChild(elementVideo);

    elementVideos.push(elementVideo);
  }


  setupVideoPlayer(elementVideos).then(value => videoPlayer = value);


  // add green button
  const elementBlueButton = document.createElement('button');
  elementBlueButton.id = "blueButton";
  elementBlueButton.innerHTML = "Light on";
  playerDiv.appendChild(elementBlueButton);
  elementBlueButton.addEventListener ("click", function() {
    sendClickEvent(videoPlayer, 1);
  });

  // add green button
  const elementGreenButton = document.createElement('button');
  elementGreenButton.id = "greenButton";
  elementGreenButton.innerHTML = "Light off";
  playerDiv.appendChild(elementGreenButton);
  elementGreenButton.addEventListener ("click", function() {
    sendClickEvent(videoPlayer, 2);
  });

  // add fullscreen button
  const elementFullscreenButton = document.createElement('img');
  elementFullscreenButton.id = 'fullscreenButton';
  elementFullscreenButton.src = 'images/FullScreen.png';
  playerDiv.appendChild(elementFullscreenButton);
  elementFullscreenButton.addEventListener ("click", function() {
    if (!document.fullscreenElement) {
      if(document.documentElement.requestFullscreen) {
        document.documentElement.requestFullscreen();
      }
      else if(document.documentElement.webkitRequestFullscreen){
        document.documentElement.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT);
      }
    }
  });
  document.addEventListener('webkitfullscreenchange', onFullscreenChange);
  document.addEventListener('fullscreenchange', onFullscreenChange);

  function onFullscreenChange(e) {
    if(document.webkitFullscreenElement || document.fullscreenElement) {
      elementFullscreenButton.style.display = 'none';
    }
    else {
      elementFullscreenButton.style.display = 'block';
    }
  }

}

function setupMediaSelector(options)
{
  const playerDiv = document.getElementById('player');
  let mediaSelectDiv = document.createElement("div");
  mediaSelectDiv.id = "mediaSelect";
  mediaSelectDiv.setAttribute("style", "width:200px;");
  mediaSelectDiv.className = "custom-select";
  playerDiv.appendChild(mediaSelectDiv);
  const mediaSelect = document.createElement("select");
  mediaSelectDiv.appendChild(mediaSelect);
  let index = 0;
  options.forEach(option=>{
    let optionItem = document.createElement("Option");
    optionItem.value = index++;
    optionItem.innerHTML = option;
    mediaSelect.appendChild(optionItem);
  })


  let customSelects, selElmnt;
  /*look for any elements with the class "custom-select":*/
  customSelects = document.getElementsByClassName("custom-select");
  for (let i = 0; i < customSelects.length; i++) {
    selElmnt = customSelects[i].getElementsByTagName("select")[0];
    /*for each element, create a new DIV that will act as the selected item:*/
    let a = document.createElement("DIV");
    a.setAttribute("class", "select-selected");
    a.innerHTML = selElmnt.options[selElmnt.selectedIndex].innerHTML;
    customSelects[i].appendChild(a);
    /*for each element, create a new DIV that will contain the option list:*/
    let b = document.createElement("DIV");
    b.setAttribute("class", "select-items select-hide");
    for (let j = 1; j < selElmnt.length; j++) {
      /*for each option in the original select element,
      create a new DIV that will act as an option item:*/
      let c = document.createElement("DIV");
      c.innerHTML = selElmnt.options[j].innerHTML;
      c.addEventListener("click", function(e) {
        /*when an item is clicked, update the original select box,
        and the selected item:*/
        let y, i, k, s, h;
        s = this.parentNode.parentNode.getElementsByTagName("select")[0];

        videoPlayer.selectMediaStream(this.innerHTML);
        console.log(this.innerHTML);

        h = this.parentNode.previousSibling;
        for (i = 0; i < s.length; i++) {
          if (s.options[i].innerHTML == this.innerHTML) {
            s.selectedIndex = i;
            h.innerHTML = this.innerHTML;
            y = this.parentNode.getElementsByClassName("same-as-selected");
            for (k = 0; k < y.length; k++) {
              y[k].removeAttribute("class");
            }
            this.setAttribute("class", "same-as-selected");
            break;
          }
        }
        h.click();
      });
      b.appendChild(c);
    }
    customSelects[i].appendChild(b);
    a.addEventListener("click", function(e) {
      /*when the select box is clicked, close any other select boxes,
      and open/close the current select box:*/
      e.stopPropagation();
      closeAllSelect(this);
      this.nextSibling.classList.toggle("select-hide");
      this.classList.toggle("select-arrow-active");
    });
  }
  function closeAllSelect(elmnt) {
    /*a function that will close all select boxes in the document,
    except the current select box:*/
    var x, y, i, arrNo = [];
    x = document.getElementsByClassName("select-items");
    y = document.getElementsByClassName("select-selected");
    for (i = 0; i < y.length; i++) {
      if (elmnt == y[i]) {
        arrNo.push(i)
      } else {
        y[i].classList.remove("select-arrow-active");
      }
    }
    for (i = 0; i < x.length; i++) {
      if (arrNo.indexOf(i)) {
        x[i].classList.add("select-hide");
      }
    }
  }
  /*if the user clicks anywhere outside the select box,
  then close all select boxes:*/
  document.addEventListener("click", closeAllSelect);
}

async function setupVideoPlayer(elements, config) {
  const videoPlayer = new VideoPlayer(elements, config);
  await videoPlayer.setupConnection();

  videoPlayer.ondisconnect = onDisconnect;
  videoPlayer.onaddtrackfinish = onAddTrackFinish;
  registerKeyboardEvents(videoPlayer);

  elements.forEach(element=>{
    registerMouseEvents(videoPlayer, element);
  });

  return videoPlayer;
}

function onDisconnect() {
  const playerDiv = document.getElementById('player')
  clearChildren(playerDiv);
  videoPlayer = null;
  showPlayButton();
}

function onAddTrackFinish(mediaStreams) {

  let options = ["Select a media"];
  for (let i=0; i<mediaStreams.length; ++i)
  {
    options.push(mediaStreams[i].id);
  }
  setupMediaSelector(options);
  console.log("onAddTrackFinish");
}

function clearChildren(element) {
  while (element.firstChild) {
    element.removeChild(element.firstChild);
  }
}
