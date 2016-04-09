#include <Wire.h>
#include <EasyTransferI2C.h>
#include <Servo.h>

EasyTransferI2C ET; 

struct ARM_DATA_STRUCTURE{
    int armBase;
    int armSholder;
    int armForearm;
    int armWrist; 
    int armClaw;
};

ARM_DATA_STRUCTURE arm;

#define I2C_SLAVE_ADDRESS 9

#define SERVO_BASE_PIN 2
#define SERVO_SHOLDER_A_PIN 3
#define SERVO_SHOLDER_B_PIN 4
#define SERVO_FOREARM_PIN 5
#define SERVO_WRIST_PIN 6
#define SERVO_CLAW_PIN 7

Servo servoBase;
Servo servoSholderA;
Servo servoSholderB;
Servo servoForearm;
Servo servoWrist;
Servo servoClaw;

void setup(){
	
  Serial.begin(9600);
  
  Wire.begin(I2C_SLAVE_ADDRESS);
  
  ET.begin(details(arm), &Wire);
  
  Wire.onReceive(receive);
  
  pinMode(13, OUTPUT);

  servoBase.attach(SERVO_BASE_PIN);
  servoSholderA.attach(SERVO_SHOLDER_A_PIN);
  servoSholderB.attach(SERVO_SHOLDER_B_PIN);
  servoForearm.attach(SERVO_FOREARM_PIN);
  servoWrist.attach(SERVO_WRIST_PIN);
  servoClaw.attach(SERVO_CLAW_PIN);
  
}

void loop() {

	if(ET.receiveData()) {

    int invertedSholder = 180 - arm.armSholder;

    servoBase.write(arm.armBase); 
    servoSholderA.write(arm.armSholder);
    servoSholderB.write(invertedSholder);
    servoForearm.write(arm.armForearm);
    servoWrist.write(arm.armWrist);
    servoClaw.write(arm.armClaw);
		
		Serial.print("armBase=");
		Serial.print(arm.armBase);
		Serial.print(", armSholder A=");
		Serial.print(arm.armSholder);
    Serial.print(", armSholder B=");
    Serial.print(invertedSholder);
		Serial.print(", armWrist=");
		Serial.print(arm.armWrist);
		Serial.print(", armClaw=");
		Serial.print(arm.armClaw);
		Serial.println("");

	}
	
}

void receive(int numBytes) {
	
	
}
