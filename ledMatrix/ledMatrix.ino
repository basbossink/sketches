int ledAnode = 13;
int ledCathode = 12;
int led2Cathode = 8;
int led3Cathode = 7;
int led4Anode = 4;

void setup() {                
  pinMode(ledAnode, OUTPUT);     
  pinMode(ledCathode, OUTPUT);
  pinMode(led2Cathode, OUTPUT);
  pinMode(led3Cathode, OUTPUT);
  pinMode(led4Anode, OUTPUT);
  digitalWrite(ledAnode, HIGH); 
  digitalWrite(ledCathode, HIGH);
  digitalWrite(led2Cathode, HIGH);
  digitalWrite(led3Cathode, LOW);
  digitalWrite(led4Anode, HIGH);
}

void loop() {
}
