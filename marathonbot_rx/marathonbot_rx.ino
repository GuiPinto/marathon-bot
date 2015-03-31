/**
  ROCKET CAR!!
  Receiver
**/


#include <EasyTransfer.h>
#include <Servo.h> 

#define ENGINE1  10

#define escArmVal 89

Servo throttle;
Servo stearing;

//create object
EasyTransfer ET; 

struct RECEIVE_DATA_STRUCTURE{
  int throttle;
  int breaks;
  int stearing;
  boolean armESC;
  boolean breaking;
  boolean headlights;
  boolean engine1;
  boolean engine2;  
};

boolean engine1Fireing = false;
unsigned long engine1FireingTime = 2000;
unsigned long engine1FiredTime = 0;

//give a name to the group of data
RECEIVE_DATA_STRUCTURE rocketcontrols;

void setup(){
  
  Serial.begin(9600);
  
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 
  ET.begin(details(rocketcontrols), &Serial);
  
  stearing.attach(9);
  throttle.attach(8, 1000, 2000);
  
  pinMode(13, OUTPUT);
  
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT); 
  pinMode(ENGINE1,OUTPUT);  

  // Arm ESC
  throttle.write(89);
  delay(3000);

}

int motorInit = 0;

void loop(){
  
/*
I have all values i need: 
- arm with a value between 89 to 97 during 2s
- one direction:99 to 138 and the other 87 to 48
- brake but is too bad for the motor, i don´t use them in my project:0 and 254.
*/
  

  if(ET.receiveData()) {
    
      // Arming Electronic Speed Controller
      if (rocketcontrols.armESC) {
        throttle.write( escArmVal );
        delay(2000);
      }

      // Stearing
      stearing.write( map(rocketcontrols.stearing, 0, 255, 25, 180) );

      // Throttle
      if (rocketcontrols.breaking) {
       //throttle.write(30);
      } else if (rocketcontrols.breaks > 3) {
        throttle.write( map(rocketcontrols.throttle, 0, 255, 48, 87) );
      } else if (rocketcontrols.throttle > 3) {
        throttle.write( map(rocketcontrols.throttle, 0, 255, 99, 138) );
      } else {
         throttle.write(90);
      }
      
      
      
      if (rocketcontrols.engine1 && !engine1Fireing) {
        engine1Fireing = true;
        engine1FiredTime = millis();
      }

      if (engine1Fireing) {
        if (millis() > engine1FiredTime + engine1FireingTime) {
          digitalWrite(ENGINE1,LOW);
          engine1Fireing = false;
        } else {
          digitalWrite(ENGINE1,HIGH);
        }
      }

  }
  
  delay(18);

}

