import * as Logger from "./logger.js";

const _e = 0.09;
const _gameloopInterval = 16.67; //in milliseconds, 60 times a second
var gameloop = null;
var gamepadsPreviousButtonsStates = {}
var gamepadsPreviousAxesStates = {}
var gamepadsConnectedTimeStamp = {}
const _axisOffset = 100;
const _axisMultiplier = 1;
const _axisYInverted = -1;

class GamepadButtonEvent extends Event
{
    constructor()
    {
        super(...arguments);
        this.index = arguments[1].index;
        this.id = arguments[1].id;
        this.value = arguments[1].value;
    }
}

class GamepadAxisEvent extends Event
{
    constructor()
    {
        super(...arguments);
        this.index = arguments[1].index;
        this.x = arguments[1].x;
        this.y = arguments[1].y;
        this.id = arguments[1].id;
    }
}

function storePreviousState(gamepad)
{
  gamepadsPreviousButtonsStates[gamepad.index] = {};
  gamepad.buttons.forEach(function(button, index) {
    gamepadsPreviousButtonsStates[gamepad.index] [index] =  {value : button.value, pressed : button.pressed} 
  })

  gamepadsPreviousAxesStates[gamepad.index] = [gamepad.axes.length]
  for(var index = 0; index < gamepad.axes.length; index++)
    gamepadsPreviousAxesStates[gamepad.index][index] = gamepad.axes[index] 
}

function checkAxes(gamepad, previousGamePad)
{
    for(var i=0; i < gamepad.axes.length; i+=2)
    {
        var absX = Math.abs(gamepad.axes[i]);
        var absY = Math.abs(gamepad.axes[i+1]);
        var event = null;
        if (( absX > _e) || 
            ( absY > _e))
        {
          
          event = new GamepadAxisEvent('gamepadAxis', {id: gamepadsConnectedTimeStamp[gamepad.index], index: i/2 + _axisOffset, x: gamepad.axes[i] * _axisMultiplier, y: gamepad.axes[i+1] * _axisMultiplier * _axisYInverted })
          document.dispatchEvent(event);
        }
        else{
          var previousAbsX = Math.abs(previousGamePad[i]);
          var previousAbsY = Math.abs(previousGamePad[i+1]);
         
          //have to send if previously was moved
          if ((previousAbsX > _e) || 
            (previousAbsY > _e))
            {
              event = new GamepadAxisEvent('gamepadAxis', {id:  gamepadsConnectedTimeStamp[gamepad.index], index: i/2 + _axisOffset, x: 0.0, y: 0.0})
              document.dispatchEvent(event);
            }
        }
    }
}

function gameLoop() { 
  Object.keys(gamepadsPreviousAxesStates).forEach(function(gamepadIndex) {
      var gamepad = navigator.webkitGetGamepads ? navigator.webkitGetGamepads()[gamepadIndex] : navigator.getGamepads()[gamepadIndex];
      var previousButtons = gamepadsPreviousButtonsStates[gamepadIndex];
      gamepad.buttons.forEach(function(button, index)
      {
        var buttonStatus = navigator.webkitGetGamepads ? button == 1 : (button.value > 0 || button.pressed == true);
        var previousButtonStatus =  navigator.webkitGetGamepads ? previousButtons[index].value == 1 : ( previousButtons[index].value > 0 ||  previousButtons[index].pressed == true)
        if( buttonStatus != previousButtonStatus)
        {
            var event;
            if(buttonStatus)
                event = new GamepadButtonEvent('gamepadButtonDown', {id: gamepadsConnectedTimeStamp[gamepad.index], index: index, value: button.value})
            else
                event = new GamepadButtonEvent('gamepadButtonUp', {id: gamepadsConnectedTimeStamp[gamepad.index], index: index, value: 0})

            document.dispatchEvent(event);
        }
        else if(buttonStatus)
        {
            var event = new GamepadButtonEvent('gamepadButtonPressed',  {id: gamepadsConnectedTimeStamp[gamepad.index], index: index, value: button.value})
            document.dispatchEvent(event);
        }
      })
      checkAxes(gamepad, gamepadsPreviousAxesStates[gamepadIndex]);
      storePreviousState(gamepad);        
  })
};

function getCookie(cname) {
  var name = cname + "=";
  var decodedCookie = decodeURIComponent(document.cookie);
  var ca = decodedCookie.split(';');
  for(var i = 0; i < ca.length; i++) {
    var c = ca[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
    }
  }
  return "";
}

export function gamepadHandler(event, connecting) {
  var gamepad = event.gamepad;

  var key = gamepad.id.replace(/\s/g,'');
  var cookieTimeStamp =  getCookie(key);

  if (connecting) {
    storePreviousState(gamepad); 
    if(Object.keys(gamepadsPreviousAxesStates).length == 1)
    {
      gameloop = setInterval(gameLoop, _gameloopInterval);
    }

    //try to find the timestamp
    //need to strip the : from the id

    if(cookieTimeStamp == "")
    {
        document.cookie = key + "=" + gamepad.timestamp;
        gamepadsConnectedTimeStamp[gamepad.index] = gamepad.timestamp;
    }
    else
    {
      gamepadsConnectedTimeStamp[gamepad.index] = cookieTimeStamp;
    }

    Logger.log("connected: " + gamepadsConnectedTimeStamp[gamepad.index])
  
  } else {
    delete gamepadsPreviousAxesStates[gamepad.index];
    delete gamepadsPreviousButtonsStates[gamepad.index];
    if(Object.keys(gamepadsPreviousAxesStates).length == 0)
    {
      clearInterval(gameloop);
      gameloop = null;
    }
    Logger.log("disconnected: " + gamepad.id)
  }
}
