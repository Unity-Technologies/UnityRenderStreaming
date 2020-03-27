# Browser Input Process

The following browser inputs are supported. 

- Mouse events
- Keyboard events
- Touch events
- Button events

The browser sends byte sequences, and Unity uses the `New Input System`.

### Mouse Events

|index|value|size|
|-------|-----|-----|
|0|0|1|
|1|position x|2|
|3|position y|2|
|5|button|1|

### Keyboard Events

|index|value|size|
|-------|-----|-----|
|0|1|1|
|1|key code|1|

### Touch Events

|index|value|size|
|-------|-----|-----|
|0|2|1|
|1|phase|1|
|2|length|1|
|3|position x|2|
|5|position y|2|
|7|force|4|

## HTML UI

HTML UI events only support button components.

### Button

|index|value|size|
|-------|-----|-----|
|0|16|1|
|1|id|1|
