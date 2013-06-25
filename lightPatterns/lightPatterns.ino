/**
Copyright 2013 Bas Bossink <bas.bossink@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
enum TRAIN_STATE {
  OFF,
  RUNNING,
  STOPPING,
  STOPPED_BEFORE_BACKWARDS,
  BACKWARDS,
  BACKWARDS_STOPPING,
  STOPPED,
  ALL_BLINKING,
  EVEN_BLINKING,
  ODD_BLINKING,
}; 

struct led {
  int pin;
  int highValue;
  int isOn;
  void (*write)(uint8_t, uint8_t);
};

struct state { 
  enum TRAIN_STATE state;
  enum TRAIN_STATE nextState;
  void (*onEnterState)();
  void (*loopFunction)();
};

void loopStoppedState();
void loopRunningState();
void loopStoppingState();
void loopStoppingBackwardState();
void loopOffState();
void loopRunningBackwardState();
void loopAllBlinkingState();
void loopEvenBlinkingState();
void loopOddBlinkingState();
void switchOffAll();

const int numberOfStates = 10;
const struct state STATES[] = {
  { OFF, RUNNING, &switchOffAll, &nop },
  { RUNNING, STOPPING, 0, &loopRunningState },
  { STOPPING, STOPPED_BEFORE_BACKWARDS, 0, &loopStoppingState },
  { STOPPED_BEFORE_BACKWARDS, BACKWARDS, 0, &nop },
  { BACKWARDS, BACKWARDS_STOPPING, 0, &loopRunningBackwardState },
  { BACKWARDS_STOPPING, STOPPED,  0, &loopStoppingBackwardState },
  { STOPPED, ALL_BLINKING, 0, &nop },
  { ALL_BLINKING, EVEN_BLINKING, &switchOffAll, &loopAllBlinkingState },
  { EVEN_BLINKING, ODD_BLINKING, &switchOffAll, &loopEvenBlinkingState },
  { ODD_BLINKING, OFF, &switchOffAll, &loopOddBlinkingState }
};

struct state nextState(struct state currentState) {
  for(int i = 0; i < numberOfStates; i++) {
    if(currentState.state == STATES[i].state) {
      for(int j = 0; j < numberOfStates; j++) {
        if(STATES[i].nextState == STATES[j].state) {
          Serial.print("\nchanging from state: ");
          Serial.print(currentState.state);
          Serial.print(", to state: ");
          Serial.println(currentState.nextState);
          return STATES[j];
        }
      }
    }
  }
}

void myAnalogWrite(uint8_t pin, uint8_t value) {
  analogWrite(pin,value);
}

#define ANALOG(pin) pin, analogHighValue, 0, &myAnalogWrite
#define DIGITAL(pin) pin, HIGH, 0, &digitalWrite

const int analogHighValue = 255;
const int numberOfLeds = 8;
struct led LEDS[] = {
  { ANALOG(6) },
  { DIGITAL(7) },
  { DIGITAL(8) },
  { ANALOG(9) },
  { ANALOG(10) },
  { ANALOG(11) },
  { DIGITAL(12) },
  { DIGITAL(13) }
};


void switchOff(struct led *l) {
  l->write(l->pin, 0);
  l->isOn = 0;
}

void switchOn(struct led *l) {
  l->write(l->pin, l->highValue);
  l->isOn = 1;
}

void forEachLed(void(*fn)(struct led*)) {
  for(int i = 0; i < numberOfLeds; i++) {
    fn(LEDS+i);
  }
}

void switchOffAll() {
  forEachLed(&switchOff);
}

const int buttonPin = 2;
const long blinkInterval = 300;
const long decelerationMultiplier = 3;
const int numberOfStoppingBlinks = 6;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;
int currentOnLedIndex = 0;
struct state currentTrainState = STATES[0];
int stoppingBlinksElapsed = 0;

void incrementCurrentLedIndex() {
    currentOnLedIndex++;
    if(currentOnLedIndex == numberOfLeds) {
      currentOnLedIndex = 0;
    }
}

void decrementCurrentLedIndex() {
    currentOnLedIndex--;
    if(currentOnLedIndex == -1) {
      currentOnLedIndex = numberOfLeds - 1;
    }
}

void switchOnNextLed() {
    switchOff(LEDS + currentOnLedIndex);
    incrementCurrentLedIndex();
    switchOn(LEDS + currentOnLedIndex);
}

void switchOnPreviousLed() {
    switchOff(LEDS + currentOnLedIndex);
    decrementCurrentLedIndex();
    switchOn(LEDS + currentOnLedIndex);
}

int interValElapsed(int interval) {
  currentMillis = millis();
  int returnValue = currentMillis - previousMillis > interval;
  if (returnValue) {
    previousMillis = currentMillis;
  }
  return returnValue;
}

void loopRunningInADirectionState(void(*swithToNextLight)()) {
  if(interValElapsed(blinkInterval)) {
    swithToNextLight();
  }
}

void nop() {}

void toggleLed(struct led* l) {
  if(l->isOn) {
    switchOff(l);
  } else {
    switchOn(l); 
  }
}

void loopAllBlinkingState() {
  if(interValElapsed(blinkInterval)) {
    forEachLed(&toggleLed);
  }  
}

int isOdd(int integer) {
  return integer % 2 == 1;
}

int isEven(int integer) {
  return integer % 2 == 0;
}

void loopBlinkingState(int(*predicate)(int)) {
  if(interValElapsed(blinkInterval)) {
     for(int i = 0; i < numberOfLeds; i++) {
       if(predicate(i)) {
         toggleLed(LEDS + i);
       } else {
         switchOff(LEDS + i);
       }
     }
  }
}

void loopOddBlinkingState() {
  loopBlinkingState(&isOdd);
}

void loopEvenBlinkingState() {
  loopBlinkingState(&isEven);
}

void loopRunningState() {
  loopRunningInADirectionState(&switchOnNextLed);
}

void loopRunningBackwardState() {
  loopRunningInADirectionState(&switchOnPreviousLed);
}

void loopStoppingInADirectionState(void(*switchToNextLight)()) {
  if(interValElapsed(blinkInterval + (stoppingBlinksElapsed * blinkInterval))) {
    stoppingBlinksElapsed++;
    switchToNextLight();
    if (stoppingBlinksElapsed >= numberOfStoppingBlinks) {
      stoppingBlinksElapsed = 0;
      currentTrainState = nextState(currentTrainState);
    }
  }
}

void loopStoppingState() {
    loopStoppingInADirectionState(&switchOnNextLed);
}

void loopStoppingBackwardState() {
    loopStoppingInADirectionState(&switchOnPreviousLed);
}

void setPinModeToOutput(struct led* l) {
  pinMode(l->pin, OUTPUT);
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  forEachLed(&setPinModeToOutput);
  switchOffAll();
} 

void loop() {
  buttonState = digitalRead(buttonPin);
  if(buttonState != lastButtonState) {
    if(buttonState == HIGH) {
      Serial.println("Button pushed, button state HIGH");
      buttonPushCounter++;
      currentTrainState = nextState(currentTrainState);
      if(currentTrainState.onEnterState) {
        currentTrainState.onEnterState();
      }
    }
  }
  lastButtonState = buttonState;
  currentTrainState.loopFunction();
}
