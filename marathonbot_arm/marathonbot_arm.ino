#include <Wire.h>
#include <EasyTransferI2C.h>
#include <AFMotor.h>

EasyTransferI2C ET; 

struct ARM_DATA_STRUCTURE{
	int armM1;
	int armM2;
	int armM3;
	int armM4;
};

ARM_DATA_STRUCTURE arm;

#define I2C_SLAVE_ADDRESS 9

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

void setup(){
	
  	Serial.begin(9600);
  
	Wire.begin(I2C_SLAVE_ADDRESS);
	
	ET.begin(details(arm), &Wire);

	Wire.onReceive(receive);
	
	pinMode(13, OUTPUT);
	
	motor1.run(RELEASE);
	motor2.run(RELEASE);
	motor3.run(RELEASE);
	motor4.run(RELEASE);
  
}

void loop() {

	if(ET.receiveData()){
  
      
		if (arm.armM1 > 0) {
			motor1.run(FORWARD);
			motor1.setSpeed(arm.armM1);
		} else if (arm.armM1 < 0) {
			motor1.run(BACKWARD);
			motor1.setSpeed(abs(arm.armM1));
		} else {
			motor1.run(RELEASE);
		}
		
		if (arm.armM2 > 0) {
			motor2.run(FORWARD);
			motor2.setSpeed(arm.armM2);
		} else if (arm.armM2 < 0) {
			motor2.run(BACKWARD);
			motor2.setSpeed(abs(arm.armM2));
		} else {
			motor2.run(RELEASE);
		}	
		
				
		if (arm.armM3 > 0) {
			motor3.run(FORWARD);
			motor3.setSpeed(arm.armM3);
		} else if (arm.armM3 < 0) {
			motor3.run(BACKWARD);
			motor3.setSpeed(abs(arm.armM3));
		} else {
			motor3.run(RELEASE);
		}		
		
		if (arm.armM4 > 0) {
			motor4.run(FORWARD);
			motor4.setSpeed(arm.armM4);
		} else if (arm.armM4 < 0) {
			motor4.run(BACKWARD);
			motor4.setSpeed(abs(arm.armM4));
		} else {
			motor4.run(RELEASE);
		}		
		
/*
		Serial.print("armM1=");
		Serial.print(arm.armM1);
		Serial.print(", armM2=");
		Serial.print(arm.armM2);
		Serial.print(", armM3=");
		Serial.print(arm.armM3);
		Serial.print(", armM4=");
		Serial.print(arm.armM4);
		Serial.println("");
*/

	}
	
}

void receive(int numBytes) {
	
	
}
