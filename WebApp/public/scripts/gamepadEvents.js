const _e = 0.09;
const _gameloopInterval = 16.67; //in milliseconds, 60 times a second
var gameloop = null;
var gamepadsPreviousButtonsStates = {}
var gamepadsPreviousAxesStates = {}
const _axisOffset = 100;
const _axisMultiplier = 1;
const _axisYInverted = -1;
const GamepadButtonsKeyCode = 
{
  button0: 0,
  button1: 1,
  button2: 2,
  button3: 3,
  button4: 4,
  button5: 5,
  button6: 6,
  button7: 7,
  button8: 8,
  button9: 9,
  axis0button: 10,
  axis1button: 11,
  dpadup : 12,
  dpaddown: 13,
  dpadleft: 14,
  dpadright: 15,
  axis0: _axisOffset,
  axis1: 101
};

class GamepadButtonEvent extends Event
{
    constructor()
    {
        super(...arguments);
        this.index = arguments[1].index;
        this.id = arguments[1].id;
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
          
          event = new GamepadAxisEvent('gamepadAxis', {id: gamepad.index, index: i/2 + _axisOffset, x: gamepad.axes[i] * _axisMultiplier, y: gamepad.axes[i+1] * _axisMultiplier * _axisYInverted })
          document.dispatchEvent(event);
        }
        else{
          var previousAbsX = Math.abs(previousGamePad[i]);
          var previousAbsY = Math.abs(previousGamePad[i+1]);
         
          //have to send if previously was moved
          if ((previousAbsX > _e) || 
            (previousAbsY > _e))
            {
              event = new GamepadAxisEvent('gamepadAxis', {id: gamepad.index, index: i/2 + _axisOffset, x: 0.0, y: 0.0})
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
                  event = new GamepadButtonEvent('gamepadButtonDown', {id: gamepad.index, index: index})
              else
                  event = new GamepadButtonEvent('gamepadButtonUp', {id: gamepad.index, index: index})

              document.dispatchEvent(event);
          }
          else if(buttonStatus)
          {
              var event = new GamepadButtonEvent('gamepadButtonPressed',  {id: gamepad.index, index: index})
              document.dispatchEvent(event);
          }
        })
        checkAxes(gamepad, gamepadsPreviousAxesStates[gamepadIndex]);
        storePreviousState(gamepad);        
    })
  };

  function gamepadHandler(event, connecting) {
    var gamepad = event.gamepad;
  
    if (connecting) {
      storePreviousState(gamepad); 
      if(Object.keys(gamepadsPreviousAxesStates).length == 1)
      {
        gameloop = setInterval(gameLoop, _gameloopInterval);
      }
      console.log("connected: " + gamepad.id)
   
    } else {
      //delete gamepadsPreviousStates[gamepad.index];
      if(Object.keys(gamepadsPreviousAxesStates).length == 0)
      {
        clearInterval(gameloop);
        gameloop = null;
      }
      console.log("disconnected: " + gamepad.id)
    }
  }
  
  export function startGamepadDetection() {
    window.addEventListener("gamepadconnected", function(e) { gamepadHandler(e, true); }, false);
    window.addEventListener("gamepaddisconnected", function(e) { gamepadHandler(e, false); }, false);
  }