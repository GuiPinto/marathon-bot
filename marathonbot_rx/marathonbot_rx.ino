/**
  ROCKET CAR!!
  Receiver
**/

#include <Wire.h>
#include <Servo.h> 
#include <EasyTransfer.h>
#include <EasyTransferI2C.h>

#define ENGINE1_PIN  10
#define THROTTLE_PIN 8
#define STEARING_PIN 9
#define SHUTDOWN_PIN  5
#define I2C_SLAVE_ADDRESS 9

#define escArmVal 89

Servo throttle;
Servo stearing;

EasyTransfer ET; 
EasyTransferI2C ETArm; 

struct CONTROLLER_DATA_STRUCTURE{
	int throttle;
	int reverse;
	int stearing;
	boolean armESC;
	boolean breaking;
	boolean headlights;
	boolean engine1;
	boolean engine2;
	int armM1;
	int armM2;
	int armM3;
	int armM4;
};

struct ARM_DATA_STRUCTURE{
	int armM1;
	int armM2;
	int armM3;
	int armM4;
};


ARM_DATA_STRUCTURE arm;
CONTROLLER_DATA_STRUCTURE controller;

boolean engine1Fireing = false;
unsigned long engine1FireingTime = 2000;
unsigned long engine1FiredTime = 0;

volatile boolean shutdown = false;

void setup(){
  
	Serial.begin(9600);
	
	Wire.begin();
	
	ET.begin(details(controller), &Serial);
	ETArm.begin(details(arm), &Wire);
	
	stearing.attach(STEARING_PIN, 1000, 2000);
	throttle.attach(THROTTLE_PIN, 1000, 2000);
	
	pinMode(13, OUTPUT);
	
	pinMode(THROTTLE_PIN, OUTPUT);
	pinMode(STEARING_PIN, OUTPUT); 
	pinMode(ENGINE1_PIN, OUTPUT);  
	pinMode(SHUTDOWN_PIN, OUTPUT);
	
	randomSeed(analogRead(0));

	// Arm ESC
	//throttle.write(89);
	//delay(3000);

}

int motorInit = 0;

void loop(){
	
      if (!shutdown && digitalRead(SHUTDOWN_PIN) == HIGH) {
          shutdown = true;
      }
      
      if (shutdown) {
        
        stearing.write(127);
        throttle.write(90);
        
        arm.armM1 = 0;
        arm.armM2 = 0;
        arm.armM3 = 0;
        arm.armM4 = 0;           
        
        // And send to arm
	ETArm.sendData(I2C_SLAVE_ADDRESS);
		
        delay(18);
         return; 
      }
    
  
  
	/*
	I have all values i need: 
	- arm with a value between 89 to 97 during 2s
	- one direction:99 to 138 and the other 87 to 48
	- brake but is too bad for the motor, i don´t use them in my project:0 and 254.
	*/
	
	if(ET.receiveData()) {
		
		// Arming Electronic Speed Controller
		if (controller.armESC) {
			throttle.write( escArmVal );
			delay(2000);
		}
		
		// Stearing
                int trim = 0;
		stearing.write( map(controller.stearing, 0, 255, 20 + trim, 175 - trim) );
		//stearing.write(controller.stearing);

//-----Foward: 95-255
//-----Reverse: 84-0

		// Throttle
		if (controller.breaking) {
			throttle.write(88);
		} else if (controller.reverse > 5) {
			throttle.write( map(controller.reverse, 5, 255, 0, 84) );
		} else if (controller.throttle > 5) {  
			throttle.write( map(controller.throttle, 5, 255, 95, 255) );
		} else {
			throttle.write(90);
		}	

		// Rocket Engine
		if (controller.engine1 && !engine1Fireing) {
			engine1Fireing = true;
			engine1FiredTime = millis();
		}
		if (engine1Fireing) {
			if (millis() > engine1FiredTime + engine1FireingTime) {
				digitalWrite(ENGINE1_PIN, LOW);
				engine1Fireing = false;
			} else {
				digitalWrite(ENGINE1_PIN, HIGH);
			}
		}


	
		
		// Pass on arm values
		arm.armM1 = controller.armM1;
		arm.armM2 = controller.armM2;
		arm.armM3 = controller.armM3;
		arm.armM4 = controller.armM4;
	
		// And send to arm
		ETArm.sendData(I2C_SLAVE_ADDRESS);
		
		delay(18);	
	}

}

