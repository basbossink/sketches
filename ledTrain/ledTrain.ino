enum TRAIN_STATE {
  OFF,
  KNIGHT_RIDER,
  CYLON_COMMANDER,
  FILLING_UP,
  FILLING_UP_DRYING_UP,
  RANDOM_FILL,
  CROSSING_LEDS,
  RUNNING,
  HIGH_SPEED,
  ULTRA_HIGH_SPEED,
  STOPPING,
  STOPPED_BEFORE_BACKWARDS,
  BACKWARDS,
  BACKWARDS_STOPPING,
  STOPPED,
  RUNNING_TRAIN,
  ALL_BLINKING,
  EVEN_BLINKING,
  ODD_BLINKING,
  BINARY_COUNTER,
  BINARY_COUNTER_GREY_CODE,
  BINARY_COUNTER_FIBONACCI,
  BINARY_COUNTER_FACTORIAL,
  BINARY_RANDOM,
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
void loopCrossingLeds();
void switchOffAllAndResetCylonIndices();
void loopRunningTrain();
void loopBinaryCounter();
void loopBinaryCounterGrayCode();
void loopBinaryCounterFibonacci();
void loopBinaryCounterFactorial();
void loopBinaryRandom();

const struct state STATES[] = {
  { OFF, KNIGHT_RIDER, &switchOffAll, &nop },
  { KNIGHT_RIDER, CYLON_COMMANDER, &switchOffAllAndResetLedIndex, &loopKnightRiderState },
  { CYLON_COMMANDER, FILLING_UP, &switchOffAllAndResetLedIndex, &loopCylonCommanderState },
  { FILLING_UP, FILLING_UP_DRYING_UP, &switchOffAllAndResetLedIndex, &loopFillingUp },
  { FILLING_UP_DRYING_UP, RANDOM_FILL, &switchOffAllAndResetLedIndex, &loopFillingUpDryingUp },
  { RANDOM_FILL, CROSSING_LEDS, &switchOffAllAndResetLedIndex, &loopRandomFill },
  { CROSSING_LEDS, RUNNING, &switchOffAllAndResetCylonIndices, &loopCrossingLeds },
  { RUNNING, HIGH_SPEED, &switchOffAllAndResetLedIndex, &loopRunningState },
  { HIGH_SPEED, ULTRA_HIGH_SPEED, 0, &loopHighSpeedRunningState },
  { ULTRA_HIGH_SPEED, STOPPING, 0, &loopUltraHighSpeedRunningState },
  { STOPPING, STOPPED_BEFORE_BACKWARDS, 0, &loopStoppingState },
  { STOPPED_BEFORE_BACKWARDS, BACKWARDS, 0, &nop },
  { BACKWARDS, BACKWARDS_STOPPING, 0, &loopRunningBackwardState },
  { BACKWARDS_STOPPING, STOPPED,  0, &loopStoppingBackwardState },
  { STOPPED, RUNNING_TRAIN, 0, &nop },
  { RUNNING_TRAIN, ALL_BLINKING, &switchOffAllAndResetLedIndex, &loopRunningTrain },
  { ALL_BLINKING, EVEN_BLINKING, &switchOffAll, &loopAllBlinkingState },
  { EVEN_BLINKING, ODD_BLINKING, &switchOffAll, &loopEvenBlinkingState },
  { ODD_BLINKING, BINARY_COUNTER, &switchOffAll, &loopOddBlinkingState },
  { BINARY_COUNTER, BINARY_COUNTER_GREY_CODE, &switchOffAll, &loopBinaryCounter },
  { BINARY_COUNTER_GREY_CODE, BINARY_COUNTER_FIBONACCI, &switchOffAll, &loopBinaryCounterGrayCode },
  { BINARY_COUNTER_FIBONACCI, BINARY_COUNTER_FACTORIAL, &switchOffAll, &loopBinaryCounterFibonacci },
  { BINARY_COUNTER_FACTORIAL, BINARY_RANDOM, &switchOffAll, &loopBinaryCounterFactorial },
  { BINARY_RANDOM, OFF, &switchOffAll, &loopBinaryRandom },
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
const int TRAIN_SIZE = 5;
const unsigned long BLINK_INTERVAL = 1 << 8;
const unsigned long CYLON_BLINK_INTERVAL = (BLINK_INTERVAL >> 1) - 10;
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long DECELERATION_MULTIPLIER = 3;
const unsigned long HIGH_SPEED_BLINK_INTERVAL = BLINK_INTERVAL >> 2;
const unsigned long ULTRA_HIGH_SPEED_BLINK_INTERVAL = BLINK_INTERVAL >> 4;
const unsigned long RIGHT_LED_LAG = 100;
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
unsigned int counter = 0;
unsigned int fibNMinus2 = 0;
unsigned int fibNMinus1 = 1;
unsigned int factorialNMinus1 = 1;

struct state currentTrainState = STATES[0];
unsigned long currentMillis = 0;
unsigned long currentRightLedMillis = 0;
unsigned long lastDebounceTime = 0;
unsigned long previousMillis = 0;
unsigned long previousRightLedMillis = 0;

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

void switchOffAllAndResetCylonIndices() {
  switchOffAll();
  resetIndex(cylonLeftIndex);
  cylonRightIndex = NUMBER_OF_LEDS -1;
}

template <typename T>
void wrapAroundIncrementIndex(
  T& index, 
  const int upperBound = NUMBER_OF_LEDS,
  const int lowerBound = 0) {
  index++;
  if(index == upperBound) {
    index = lowerBound;
  }
}

void wrapAroundDecrementIndex(int& index) {
  index--;
  if(index == -1) {
    index = NUMBER_OF_LEDS - 1;
  }
}

void safeIncrementIndex(int& index) {
  index++;
  index = min(index, NUMBER_OF_LEDS -1);
}

void safeDecrementIndex(int& index) {
  index--;
  index = max(0, index);
}

void switchOnNextLed(int& index) {
  switchOff(LEDS + index);
  wrapAroundIncrementIndex<int>(index);
  switchOn(LEDS + index);
}

void switchOnPreviousLed(int& index) {
  switchOff(LEDS + index);
  wrapAroundDecrementIndex(index);
  switchOn(LEDS + index);
}

bool interValElapsed(int interval) {
  return interValElapsed(interval, currentMillis, previousMillis);
}

bool interValElapsed(
  int interval, 
  unsigned long& current, 
  unsigned long& previous) {
  current = millis();
  bool returnValue = current - previous > interval;
  if (returnValue) {
    previous = current;
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
      safeIncrementIndex(currentOnLedIndex);
    }
    if(fillingDirection == RIGHT_TO_LEFT) {
      removeLed();
      safeDecrementIndex(currentOnLedIndex);
    }
  }
}

void loopCrossingLeds() {
  if(interValElapsed(HIGH_SPEED_BLINK_INTERVAL)) {
    switchOff(LEDS + cylonLeftIndex);
    wrapAroundIncrementIndex<int>(cylonLeftIndex);
    switchOn(LEDS + cylonLeftIndex);
  }  
  if(interValElapsed(
    HIGH_SPEED_BLINK_INTERVAL + RIGHT_LED_LAG, 
    currentRightLedMillis, 
    previousRightLedMillis)) {
    switchOff(LEDS + cylonRightIndex);
    wrapAroundDecrementIndex(cylonRightIndex);
    switchOn(LEDS + cylonRightIndex);
  }
}

void loopRunningTrain() {
  if(interValElapsed(HIGH_SPEED_BLINK_INTERVAL)) {
    int index = currentOnLedIndex - TRAIN_SIZE;
    if(0 <= index && index < NUMBER_OF_LEDS) {
      switchOff(LEDS + index);
    }
    for(int onIndex = currentOnLedIndex;
      0 <= onIndex && 
      onIndex < NUMBER_OF_LEDS && 
      onIndex > currentOnLedIndex - TRAIN_SIZE;
      onIndex--) {
      switchOn(LEDS + onIndex);
    }
    wrapAroundIncrementIndex<int>(
      currentOnLedIndex, 
      NUMBER_OF_LEDS + TRAIN_SIZE, 
      -TRAIN_SIZE);
  }
}

void setLedsCorrespondingToBits(unsigned int number) {
    for(int bit = 0; bit < NUMBER_OF_LEDS; bit++) {
      if(bitRead(number,bit)) {
        switchOn(LEDS + bit);
      } else {
        switchOff(LEDS + bit);
      }
    }
}

void loopBinaryCounter() {
  if(interValElapsed(ULTRA_HIGH_SPEED_BLINK_INTERVAL)) {
    setLedsCorrespondingToBits(counter);
    wrapAroundIncrementIndex<unsigned int>(counter, 1 << NUMBER_OF_LEDS);
  }
}

unsigned int binaryToGray(unsigned int number) {
  return (number >> 1) ^ number;
}

void loopBinaryCounterGrayCode() {
  if(interValElapsed(ULTRA_HIGH_SPEED_BLINK_INTERVAL)) {
    unsigned int gray = binaryToGray(counter);
    setLedsCorrespondingToBits(gray);
    wrapAroundIncrementIndex<unsigned int>(counter, 1 << NUMBER_OF_LEDS);
  }
} 

unsigned int fibonacci(
  unsigned int& fibonacciNMinus2, 
  unsigned int& fibonacciNMinus1) {
  unsigned int returnValue = fibonacciNMinus2 + fibonacciNMinus1;
  fibonacciNMinus2 = fibonacciNMinus1;
  fibonacciNMinus1 = returnValue;
  return returnValue;
}

void loopBinaryCounterFibonacci() {
  if(interValElapsed(BLINK_INTERVAL)) {
    setLedsCorrespondingToBits(counter);
    counter = fibonacci(fibNMinus2, fibNMinus1);
    if(counter >= 1 << NUMBER_OF_LEDS) {
      fibNMinus2 = 0;
      fibNMinus1 = 1;
    }
  }
}

unsigned int factorial(unsigned int n, unsigned int& factorialNMinus1) {
  unsigned int returnValue = n * factorialNMinus1;
  factorialNMinus1 = returnValue;
  return returnValue;
}

void loopBinaryCounterFactorial() {
  if(interValElapsed(BLINK_INTERVAL)) {
    unsigned int fact = factorial(counter, factorialNMinus1);
    if(fact >= 1 << NUMBER_OF_LEDS) {
      fact = 1;
      factorialNMinus1 = 1;
      counter = 0;
    }
    setLedsCorrespondingToBits(fact);
    counter++;
  }
}

void loopBinaryRandom() {
  if(interValElapsed(BLINK_INTERVAL)) {
    setLedsCorrespondingToBits(random((1 << NUMBER_OF_LEDS) -1));
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
