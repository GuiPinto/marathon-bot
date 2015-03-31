#include <PS3USB.h>
#include <EasyTransfer.h>

#define MODE_DRIVING  0
#define MODE_ARM  1

#define MAX_RESET 7 //MAX3421E pin 12
#define MAX_GPX   8 //MAX3421E pin 17

// Initialize USB Hooks
USB Usb;
PS3USB PS3(&Usb); // This will just create the instance
//PS3USB PS3(&Usb,0x00,0x15,0x83,0x3D,0x0A,0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

// Initialize EasyTransfer Object
EasyTransfer ET; 

struct SEND_DATA_STRUCTURE{
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
SEND_DATA_STRUCTURE rocketcontrols;

volatile int mode = MODE_DRIVING;

void setup() {
  
	// Stupid Sparkfun USB Shield..
	pinMode(MAX_GPX, INPUT);
	pinMode(MAX_RESET, OUTPUT);
	digitalWrite(MAX_RESET, LOW);
	delay(20);
	digitalWrite(MAX_RESET, HIGH);
	delay(20);
	
	pinMode(13, OUTPUT);
	
	// Initialize Serial and Easy Transfer
	Serial.begin(9600);
	delay(1000);
	Serial.println("Serial is ready.");
	
	// Initialize Easy-Transfer Lib
	//ET.begin(details(rocketcontrols), &Serial);
	
	// Initialize USB
	if (Usb.Init() == -1) { // Halt!
		Serial.println("USB.Init() did not start..");
		while(Usb.Init() == -1) { 
			Serial.println("Waiting on USB.init()...");
			delay(500);
		}
	}
	
	Serial.println("USB Initialized!");
	
	randomSeed(analogRead(0));
	
	modeChange(mode);
	delay(200);
		
}

void loop() {

	Usb.Task();
	
	// Mode selection
	if( PS3.getButtonClick(SELECT) ) {
		
		mode = mode == MODE_DRIVING ? MODE_ARM : MODE_DRIVING;
	
        	modeChange(mode);
		
		Serial.print("Switched to mode:");
		Serial.println(mode); 
	}
	
	switch(mode) {
	case MODE_DRIVING: 
		
		// Stearing
		rocketcontrols.stearing = int( PS3.getAnalogHat(LeftHatX) );
		
		// Throttle
		if(PS3.getAnalogButton(R2)) {
			rocketcontrols.throttle = int( PS3.getAnalogButton(R2) );
			rocketcontrols.breaking = false;
		} else {
			rocketcontrols.throttle = 0;
		}
		
		// Breaking
		if(PS3.getAnalogButton(L2)) {
			rocketcontrols.breaks = int( PS3.getAnalogButton(L2) );
			rocketcontrols.breaking = true;
		} else {
			rocketcontrols.breaks = 0;
			rocketcontrols.breaking = false;
		}
		
		// Breaking
		if(PS3.getButtonClick(CIRCLE)) {
			rocketcontrols.breaking = true;    
		}
		
		// Headlights
		if(PS3.getButtonClick(L3)) {
			rocketcontrols.headlights = !rocketcontrols.headlights;
		}
		
		// Arm ESC
		rocketcontrols.armESC = PS3.getButtonClick(START);
		
		// ENGINE 1  
		rocketcontrols.engine1 = PS3.getButtonClick(L1) && PS3.getButtonClick(R1);
		
		/*
		Serial.print("throttle=");
		Serial.print(rocketcontrols.throttle);
		Serial.print(", stearing=");
		Serial.print(rocketcontrols.stearing);
		Serial.print(", breaks=");
		Serial.print(rocketcontrols.breaks);
		Serial.print(", breaking=");
		Serial.print(rocketcontrols.breaking);
		Serial.println("");
		*/
		
	break;
	case MODE_ARM:
	
		int deadzone = 15;
		
		int sholderVal = armValue(int( PS3.getAnalogHat(LeftHatX) ), deadzone, false);
		int armVal = armValue(int( PS3.getAnalogHat(LeftHatY) ), deadzone, false);
		int forearmVal = armValue(int( PS3.getAnalogHat(RightHatX) ), deadzone, false);
		int handVal = armValue(int( PS3.getAnalogHat(RightHatY) ), deadzone, false);
		
		rocketcontrols.armM1 = sholderVal;
		rocketcontrols.armM2 = armVal;
		rocketcontrols.armM3 = forearmVal;
		rocketcontrols.armM4 = handVal;

		/*
		Serial.print("armM1=");
		Serial.print(rocketcontrols.armM1);
		Serial.print(", armM2=");
		Serial.print(rocketcontrols.armM2);
		Serial.print(", armM3=");
		Serial.print(rocketcontrols.armM3);
		Serial.print(", armM4=");
		Serial.print(rocketcontrols.armM4);
		Serial.println("");
		*/

	break;
	}
	
	Serial.print("throttle=");
	Serial.print(rocketcontrols.throttle);
	Serial.print(", stearing=");
	Serial.print(rocketcontrols.stearing);
	Serial.print(", breaks=");
	Serial.print(rocketcontrols.breaks);
	Serial.print(", armM1=");
	Serial.print(rocketcontrols.armM1);
	Serial.print(", armM2=");
	Serial.print(rocketcontrols.armM2);
	Serial.print(", armM3=");
	Serial.print(rocketcontrols.armM3);
	Serial.print(", armM4=");
	Serial.print(rocketcontrols.armM4);
	Serial.println("");
	

	//ET.sendData();
	
	delay(50);
}


void modeChange(int mode) {
	rocketcontrols.throttle = 0;
	rocketcontrols.breaks = 0;
	rocketcontrols.stearing = 125;
	rocketcontrols.armESC = false;
	rocketcontrols.breaking = false;
	rocketcontrols.headlights = false;
	rocketcontrols.engine1 = false;
	rocketcontrols.engine1 = false;

        PS3.setRumbleOn(200, 100, 0, 0);
	//PS3.setRumbleOn(RumbleHigh );
	
	if (mode == MODE_DRIVING) {
		PS3.setLedOn(LED1);
		PS3.setLedOff(LED2);
		PS3.setLedOff(LED3);
		PS3.setLedOn(LED4);
	} else {
		PS3.setLedOff(LED1);
		PS3.setLedOn(LED2);
		PS3.setLedOn(LED3);
		PS3.setLedOff(LED4);
	}
}

int armValue(int val, int deadzone, boolean reverse) {
 	if (val >= 125 + deadzone) {
        if (reverse) return -( map( val, 125 + deadzone, 255, 0, 255) );
		return map( val, 125 + deadzone, 255, 0, 255);
	} else if (val <= 125 - deadzone) {
		if (reverse) return map( val, 125 - deadzone, 0, 0, 255);
		return -( map( val, 125 - deadzone, 0, 0, 255) );
	} else {
		return 0;
	}
}

