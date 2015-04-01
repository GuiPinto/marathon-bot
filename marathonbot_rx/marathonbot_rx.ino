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
#define I2C_SLAVE_ADDRESS 9

#define escArmVal 89

Servo throttle;
Servo stearing;

EasyTransfer ET; 
EasyTransferI2C ETArm; 

struct CONTROLLER_DATA_STRUCTURE{
	int throttle;
	int breaks;
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

void setup(){
  
	Serial.begin(9600);
	
	Wire.begin();
	
	ET.begin(details(controller), &Serial);
	ETArm.begin(details(arm), &Wire);
	
	stearing.attach(STEARING_PIN);
	throttle.attach(THROTTLE_PIN, 1000, 2000);
	
	pinMode(13, OUTPUT);
	
	pinMode(THROTTLE_PIN, OUTPUT);
	pinMode(STEARING_PIN, OUTPUT); 
	pinMode(ENGINE1_PIN, OUTPUT);  
	
	randomSeed(analogRead(0));

	// Arm ESC
	//throttle.write(89);
	//delay(3000);

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
		if (controller.armESC) {
			throttle.write( escArmVal );
			delay(2000);
		}
		
		// Stearing
		stearing.write( map(controller.stearing, 0, 255, 25, 180) );
		
		// Throttle
		if (controller.breaking) {
			//throttle.write(30);
		} else if (controller.breaks > 3) {
			throttle.write( map(controller.throttle, 0, 255, 48, 87) );
		} else if (controller.throttle > 3) {
			throttle.write( map(controller.throttle, 0, 255, 99, 138) );
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

