enum TRAIN_STATE {
  OFF,
  RUNNING,
  DOUBLE_SPEED,
  QUADRUPLE_SPEED,
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
  void (*write)(uint32_t, uint32_t);
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
void loopDoubleSpeedRunningState();
void loopQuadrupleSpeedRunningState();

const int numberOfStates = 12;
const struct state STATES[] = {
  { OFF, RUNNING, &switchOffAll, &nop },
  { RUNNING, DOUBLE_SPEED, 0, &loopRunningState },
  { DOUBLE_SPEED, QUADRUPLE_SPEED, 0, &loopDoubleSpeedRunningState },
  { QUADRUPLE_SPEED, STOPPING, 0, &loopQuadrupleSpeedRunningState },
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
const int numberOfLeds = 20;
struct led LEDS[] = {
  { DIGITAL(23) },
  { DIGITAL(25) },
  { DIGITAL(27) },
  { DIGITAL(29) },
  { DIGITAL(31) },
  { DIGITAL(33) },
  { DIGITAL(35) },
  { DIGITAL(37) },
  { DIGITAL(22) },
  { DIGITAL(24) },
  { DIGITAL(26) },
  { DIGITAL(28) },
  { DIGITAL(30) },
  { DIGITAL(39) },
  { DIGITAL(43) },
  { DIGITAL(45) },
  { DIGITAL(47) },
  { DIGITAL(32) },
  { DIGITAL(34) },
  { DIGITAL(40) },
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
const long blinkInterval = 1 << 8;
const long decelerationMultiplier = 3;
const int numberOfStoppingBlinks = 6;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

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

void loopRunningInADirectionState(
  void(*swithToNextLight)(), 
  long blinkInterval) {
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
  loopRunningInADirectionState(&switchOnNextLed, blinkInterval);
}

void loopDoubleSpeedRunningState() {
  loopRunningInADirectionState(&switchOnNextLed, blinkInterval >> 2);
}
void loopQuadrupleSpeedRunningState() {
  loopRunningInADirectionState(&switchOnNextLed, blinkInterval >> 4);
}
void loopRunningBackwardState() {
  loopRunningInADirectionState(&switchOnPreviousLed, blinkInterval);
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
  pinMode(buttonPin, INPUT);
  forEachLed(&setPinModeToOutput);
  switchOffAll();
} 

void loop() {
  int reading = digitalRead(buttonPin);
  if(reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > debounceDelay) { 
    if(reading != buttonState) {
      buttonState = reading;
      if(buttonState == HIGH) {
        Serial.println("Button pushed, button state HIGH");
        buttonPushCounter++;
        currentTrainState = nextState(currentTrainState);
        if(currentTrainState.onEnterState) {
          currentTrainState.onEnterState();
        }
      }
    }
  }
  lastButtonState = reading;
  currentTrainState.loopFunction();
}
