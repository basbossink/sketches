#include <Servo.h> 
 
Servo servo1;
Servo servo2; 
int servo1Pos = 0;
int servo2Pos = 0;
const int servo1Control = 9;
const int servo2Control = 10;
const int movementDelay = 25;
void setup() 
{ 
  servo1.attach(servo1Control);  
  servo2.attach(servo2Control);
} 
 
 
void loop() 
{ 
  for(servo1Pos = 0, servo2Pos =0; 
      servo1Pos < 90; 
      servo1Pos += 1, servo2Pos +=1)
  {                              
    servo1.write(servo1Pos);     
    servo2.write(servo2Pos);
    delay(movementDelay);        
  } 
 
  for(servo1Pos = 90, servo2Pos = 90; 
      servo1Pos>=1; 
      servo1Pos-=1,servo2Pos-=1)
  {                                
    servo1.write(servo1Pos);
    servo2.write(servo2Pos);
    delay(movementDelay);       
  } 
}
