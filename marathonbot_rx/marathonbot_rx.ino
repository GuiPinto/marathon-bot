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
#define STEERING_PIN 9
#define SHUTDOWN_PIN  5
#define I2C_SLAVE_ADDRESS 9

Servo throttle;
Servo steering;

EasyTransfer ET; 
EasyTransferI2C ETArm; 

struct CONTROLLER_DATA_STRUCTURE{
    int throttle;
    int reverse;
    int steering;
    boolean breaking;
    int armBase;
    int armSholder;
    int armForearm;
    int armWrist; 
    int armClaw; 
};

struct ARM_DATA_STRUCTURE{
    int armBase;
    int armSholder;
    int armForearm;
    int armWrist; 
    int armClaw;
};


ARM_DATA_STRUCTURE arm;
CONTROLLER_DATA_STRUCTURE controller;

volatile boolean shutdown = false;
int pos = 0; 
void setup(){
  
	Serial.begin(9600);
	
	Wire.begin();
	
	ET.begin(details(controller), &Serial);
	ETArm.begin(details(arm), &Wire);
	
	steering.attach(STEERING_PIN, 1000, 2000);
	throttle.attach(THROTTLE_PIN, 1000, 2000);
	
	pinMode(13, OUTPUT);
	
	pinMode(THROTTLE_PIN, OUTPUT);
	pinMode(STEERING_PIN, OUTPUT); 
	pinMode(ENGINE1_PIN, OUTPUT);  
	pinMode(SHUTDOWN_PIN, OUTPUT);
	
	randomSeed(analogRead(0));
  
}

int motorInit = 0;

void loop(){

    if (!shutdown && digitalRead(SHUTDOWN_PIN) == HIGH) {
      shutdown = true;
    }
    
    if (shutdown) {
    
        steering.write(127);
        throttle.write(90);
        
        arm.armBase = -1;
        arm.armSholder = -1;
        arm.armForearm = -1;
        arm.armWrist = -1;
        arm.armClaw = -1;
        
        // And send to arm
        ETArm.sendData(I2C_SLAVE_ADDRESS);
        
        delay(18);
        return; 
    }


    if(ET.receiveData()) {
    			
      // Steering
      int steeringVal = controller.steering;
      // Invert steering because servo is mounted backwards
      steering.write(map(steeringVal, 0, 255, 255, 0));

		  // Throttle
      if (controller.breaking) {
          throttle.write(0);
      } else if (controller.reverse > 5) {
          throttle.write( map(controller.reverse, 5, 255, 124, 0) );
      } else if (controller.throttle > 5) {  
          throttle.write( map(controller.throttle, 5, 255, 130, 255) );
      } else {
          throttle.write(127);
      }	
  		
  		// Pass on arm values
      arm.armBase = controller.armBase;
      arm.armSholder = controller.armSholder;
      arm.armForearm = controller.armForearm;
      arm.armWrist = controller.armWrist;
      arm.armClaw = controller.armClaw;
  	
  		// And send to arm
  		ETArm.sendData(I2C_SLAVE_ADDRESS);
  		
  		delay(18);	
	}

}

