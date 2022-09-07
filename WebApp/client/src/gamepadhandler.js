export class GamepadHandler extends EventTarget {
  constructor() {
    super();
    this._controllers = {};
    window.requestAnimationFrame(this._updateStatus.bind(this));
  }

  /**
   * @param {Gamepad} gamepad 
   */
  addGamepad(gamepad) {
    this._controllers[gamepad.index] = gamepad;
  }

  /**
   * @param {Gamepad} gamepad 
   */
  removeGamepad(gamepad) {
    delete this._controllers[gamepad.index];
  }

  _updateStatus() {
    this._scanGamepad();
    for(let i in this._controllers) {
      const controller = this._controllers[i];

      // gamepadupdated event type is own definition
      this.dispatchEvent(new GamepadEvent('gamepadupdated', {
        gamepad: controller
      }));
    }
    window.requestAnimationFrame(this._updateStatus.bind(this));
  }

  _scanGamepad() {
    let gamepads = navigator.getGamepads();
    for (let i = 0; i < gamepads.length; i++) {
      if (gamepads[i] && (gamepads[i].index in this._controllers)) {
        this._controllers[gamepads[i].index] = gamepads[i];
      }
    }
  }    
}

