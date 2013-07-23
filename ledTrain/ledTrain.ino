enum TRAIN_STATE {
  OFF,
  KNIGHT_RIDER,
  CYLON_COMMANDER,
  FILLING_UP,
  FILLING_UP_DRYING_UP,
  RANDOM_FILL,
  RUNNING,
  HIGH_SPEED,
  ULTRA_HIGH_SPEED,
  STOPPING,
  STOPPED_BEFORE_BACKWARDS,
  BACKWARDS,
  BACKWARDS_STOPPING,
  STOPPED,
  ALL_BLINKING,
  EVEN_BLINKING,
  ODD_BLINKING,
}; 

enum DIRECTION {
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT,
};

struct led {
  int pin;
  bool isOn;
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
void loopHighSpeedRunningState();
void loopUltraHighSpeedRunningState();
void loopKnightRiderState();
void loopCylonCommanderState();
void switchOffAllAndResetLedIndex();
void loopFillingUp();
void loopFillingUpDryingUp();
void loopRandomFill();

const struct state STATES[] = {
  { OFF, KNIGHT_RIDER, &switchOffAll, &nop },
  { KNIGHT_RIDER, CYLON_COMMANDER, &switchOffAllAndResetLedIndex, &loopKnightRiderState },
  { CYLON_COMMANDER, FILLING_UP, &switchOffAllAndResetLedIndex, &loopCylonCommanderState },
  { FILLING_UP, FILLING_UP_DRYING_UP, &switchOffAllAndResetLedIndex, &loopFillingUp },
  { FILLING_UP_DRYING_UP, RANDOM_FILL, &switchOffAllAndResetLedIndex, &loopFillingUpDryingUp },
  { RANDOM_FILL, RUNNING, &switchOffAllAndResetLedIndex, &loopRandomFill },
  { RUNNING, HIGH_SPEED, &switchOffAllAndResetLedIndex, &loopRunningState },
  { HIGH_SPEED, ULTRA_HIGH_SPEED, 0, &loopHighSpeedRunningState },
  { ULTRA_HIGH_SPEED, STOPPING, 0, &loopUltraHighSpeedRunningState },
  { STOPPING, STOPPED_BEFORE_BACKWARDS, 0, &loopStoppingState },
  { STOPPED_BEFORE_BACKWARDS, BACKWARDS, 0, &nop },
  { BACKWARDS, BACKWARDS_STOPPING, 0, &loopRunningBackwardState },
  { BACKWARDS_STOPPING, STOPPED,  0, &loopStoppingBackwardState },
  { STOPPED, ALL_BLINKING, 0, &nop },
  { ALL_BLINKING, EVEN_BLINKING, &switchOffAll, &loopAllBlinkingState },
  { EVEN_BLINKING, ODD_BLINKING, &switchOffAll, &loopEvenBlinkingState },
  { ODD_BLINKING, OFF, &switchOffAll, &loopOddBlinkingState }
};

#define DIGITAL(pin) pin, false

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

#undef DIGITAL

const int BUTTON_PIN = 2;
const int NUMBER_OF_LEDS = sizeof(LEDS)/sizeof(struct led);
const int CYLON_LEFT_START_INDEX = (NUMBER_OF_LEDS/2)-1;
const int CYLON_RIGHT_START_INDEX = CYLON_LEFT_START_INDEX + 1;
const int NUMBER_OF_STOPPING_BLINKS = 6;
const int NUMBER_STATES = sizeof(STATES)/sizeof(struct state);
const unsigned long BLINK_INTERVAL = 1 << 8;
const unsigned long CYLON_BLINK_INTERVAL = (BLINK_INTERVAL >> 1) - 10;
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long DECELERATION_MULTIPLIER = 3;
const unsigned long HIGH_SPEED_BLINK_INTERVAL = BLINK_INTERVAL >> 2;
const unsigned long ULTRA_HIGH_SPEED_BLINK_INTERVAL = BLINK_INTERVAL >> 4;

enum DIRECTION cylonLeftDirection = RIGHT_TO_LEFT;
enum DIRECTION cylonRightDirection = LEFT_TO_RIGHT;
enum DIRECTION knightRiderDirection = LEFT_TO_RIGHT;
enum DIRECTION fillingDirection = LEFT_TO_RIGHT;

int buttonPushCounter = 0;
int buttonState = 0;
int currentOnLedIndex = 0;
int cylonLeftIndex = 0;
int cylonRightIndex = 0;
int lastButtonState = 0;
int stoppingBlinksElapsed = 0;
int targetIndex = 0;

struct state currentTrainState = STATES[0];
unsigned long currentMillis = 0;
unsigned long lastDebounceTime = 0;
unsigned long previousMillis = 0;

void switchOff(struct led *l) {
  digitalWrite(l->pin, LOW);
  l->isOn = false;
}

void switchOn(struct led *l) {
  digitalWrite(l->pin, HIGH);
  l->isOn = true;
}

void forEachLed(void(*fn)(struct led*)) {
  for(int i = 0; i < NUMBER_OF_LEDS; i++) {
    fn(LEDS+i);
  }
}

void switchOffAll() {
  forEachLed(&switchOff);
}

void resetIndex(int& index) {
  index = 0;
}

void switchOffAllAndResetLedIndex() {
  switchOffAll();
  resetIndex(currentOnLedIndex);
}

void incrementIndex(int& index) {
  index++;
  if(index == NUMBER_OF_LEDS) {
    resetIndex(index);
  }
}

void decrementIndex(int& index) {
  index--;
  if(index == -1) {
    index = NUMBER_OF_LEDS - 1;
  }
}

void switchOnNextLed(int& index) {
  switchOff(LEDS + index);
  incrementIndex(index);
  switchOn(LEDS + index);
}

void switchOnPreviousLed(int& index) {
  switchOff(LEDS + index);
  decrementIndex(index);
  switchOn(LEDS + index);
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
  void(*swithToNextLight)(int&),
  int& index,
  long blinkInterval) {
  if(interValElapsed(blinkInterval)) {
    swithToNextLight(index);
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
  if(interValElapsed(BLINK_INTERVAL)) {
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
  if(interValElapsed(BLINK_INTERVAL)) {
     for(int i = 0; i < NUMBER_OF_LEDS; i++) {
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
  loopRunningInADirectionState(
    &switchOnNextLed,
    currentOnLedIndex,
    BLINK_INTERVAL);
}

void loopHighSpeedRunningState() {
  loopRunningInADirectionState(
    &switchOnNextLed, 
    currentOnLedIndex,
    HIGH_SPEED_BLINK_INTERVAL);
}

void loopUltraHighSpeedRunningState() {
  loopRunningInADirectionState(
    &switchOnNextLed, 
    currentOnLedIndex,
    ULTRA_HIGH_SPEED_BLINK_INTERVAL);
}

void loopRunningBackwardState() {
  loopRunningInADirectionState(
    &switchOnPreviousLed,
    currentOnLedIndex,
    BLINK_INTERVAL);
}

void loopHighSpeedRunningBackwardState() {
  loopRunningInADirectionState(
    &switchOnPreviousLed, 
    currentOnLedIndex,
    HIGH_SPEED_BLINK_INTERVAL);
}

void loopUltraHighSpeedRunningBackwardState() {
  loopRunningInADirectionState(
    &switchOnPreviousLed, 
    currentOnLedIndex,
    ULTRA_HIGH_SPEED_BLINK_INTERVAL);
}

void loopStoppingInADirectionState(void(*switchToNextLight)(int&), int& index) {
  if(interValElapsed(BLINK_INTERVAL + (stoppingBlinksElapsed * BLINK_INTERVAL))) {
    stoppingBlinksElapsed++;
    switchToNextLight(index);
    if (stoppingBlinksElapsed >= NUMBER_OF_STOPPING_BLINKS) {
      stoppingBlinksElapsed = 0;
      currentTrainState = nextState(currentTrainState);
    }
  }
}

void loopStoppingState() {
  loopStoppingInADirectionState(&switchOnNextLed, currentOnLedIndex);
}

void loopStoppingBackwardState() {
  loopStoppingInADirectionState(&switchOnPreviousLed, currentOnLedIndex);
}

void loopKnightRiderState() {
  if(currentOnLedIndex == 0) {
    knightRiderDirection = LEFT_TO_RIGHT;
  }
  if(currentOnLedIndex == NUMBER_OF_LEDS -1) {
    knightRiderDirection = RIGHT_TO_LEFT;
  }
  if(knightRiderDirection == LEFT_TO_RIGHT) {
    loopHighSpeedRunningState();
  }
  if(knightRiderDirection == RIGHT_TO_LEFT) {
    loopHighSpeedRunningBackwardState();
  }
}

void advanceLeftLed() {
  if(cylonLeftIndex == 0) {
    cylonLeftDirection = LEFT_TO_RIGHT;  
  }
  if(cylonLeftIndex == CYLON_LEFT_START_INDEX) {
    cylonLeftDirection = RIGHT_TO_LEFT;
  }
  if(cylonLeftDirection == LEFT_TO_RIGHT) {
    switchOnNextLed(cylonLeftIndex);
  }
  if(cylonLeftDirection == RIGHT_TO_LEFT) {
    switchOnPreviousLed(cylonLeftIndex);
  }
}

void advanceRightLed() {
  if(cylonRightIndex == CYLON_RIGHT_START_INDEX) {
    cylonRightDirection = LEFT_TO_RIGHT;  
  }
  if(cylonRightIndex == NUMBER_OF_LEDS -1) {
    cylonRightDirection = RIGHT_TO_LEFT;
  }
  if(cylonRightDirection == LEFT_TO_RIGHT) {
    switchOnNextLed(cylonRightIndex);
  }
  if(cylonRightDirection == RIGHT_TO_LEFT) {
    switchOnPreviousLed(cylonRightIndex);
  }
}

bool allLedsAreOff() {
  bool returnValue = true;
  for(int i = 0; i < NUMBER_OF_LEDS && returnValue; i++) {
    if(LEDS[i].isOn) {
      returnValue = false;
    }
  }
  return returnValue;
}

void loopCylonCommanderState() {
  if(allLedsAreOff()) {
    cylonLeftIndex = CYLON_LEFT_START_INDEX;    
    cylonRightIndex = CYLON_RIGHT_START_INDEX;
  }
  if(interValElapsed(CYLON_BLINK_INTERVAL)) {
    advanceLeftLed();
    advanceRightLed();
  }
}

void addLed() {
  switchOn(LEDS + currentOnLedIndex);
}

void loopFillingUp() {
  if(interValElapsed(HIGH_SPEED_BLINK_INTERVAL)) {
    addLed();
    if(currentOnLedIndex == NUMBER_OF_LEDS) {
      switchOffAllAndResetLedIndex();
    } else {
      currentOnLedIndex++;
    }
  }
}

void removeLed() {
  switchOff(LEDS + currentOnLedIndex);
}

void loopFillingUpDryingUp() {
  if(currentOnLedIndex == NUMBER_OF_LEDS) {
    fillingDirection = RIGHT_TO_LEFT;
  } 
  if(currentOnLedIndex == -1) {
    fillingDirection = LEFT_TO_RIGHT;
    currentOnLedIndex++;
  } 
  if(interValElapsed(CYLON_BLINK_INTERVAL)) {
    if(fillingDirection == LEFT_TO_RIGHT) {
      addLed();
      currentOnLedIndex++;
    }
    if(fillingDirection == RIGHT_TO_LEFT) {
      removeLed();
      currentOnLedIndex--;
    }
  }
}

void loopRandomFill() {
  if(currentOnLedIndex > targetIndex) {
    fillingDirection = RIGHT_TO_LEFT;
  } 
  if(currentOnLedIndex < targetIndex) {
    fillingDirection = LEFT_TO_RIGHT;
  } 
  if(currentOnLedIndex == targetIndex) {
    if(fillingDirection == LEFT_TO_RIGHT) {
      switchOn(LEDS + targetIndex);
    }
    if(fillingDirection == RIGHT_TO_LEFT) {
      switchOff(LEDS + targetIndex);
    }
    targetIndex = random(NUMBER_OF_LEDS);
  }
  if(interValElapsed(HIGH_SPEED_BLINK_INTERVAL)) {
    if(fillingDirection == LEFT_TO_RIGHT) {
      addLed();
      currentOnLedIndex++;
      currentOnLedIndex = min(currentOnLedIndex, NUMBER_OF_LEDS -1);
    }
    if(fillingDirection == RIGHT_TO_LEFT) {
      removeLed();
      currentOnLedIndex--;
      currentOnLedIndex = max(0,currentOnLedIndex);
    }
  }
}

struct state nextState(struct state currentState) {
  for(int i = 0; i < NUMBER_STATES; i++) {
    if(currentState.state == STATES[i].state) {
      for(int j = 0; j < NUMBER_STATES; j++) {
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

void setPinModeToOutput(struct led* l) {
  pinMode(l->pin, OUTPUT);
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  randomSeed(analogRead(0));
  forEachLed(&setPinModeToOutput);
  switchOffAll();
} 

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  if(reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > DEBOUNCE_DELAY) { 
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
