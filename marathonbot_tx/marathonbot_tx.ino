#include <PS3USB.h>
#include <EasyTransfer.h>

#define DEBUG_PRINT false

#define ARM_MODE_DRIVING 0
#define ARM_MODE_CUP 1

#define MAX_RESET 7 //MAX3421E pin 12
#define MAX_GPX   8 //MAX3421E pin 17

#define TRIM_MAX        100
#define TRIM_DEFAULT    0
#define TRIM_AMOUNT     5

#define THROTTLE_IDLE     127

#define ARM_BASE_MIN   0
#define ARM_BASE_MAX   180
#define ARM_SHOLDER_MIN   5
#define ARM_SHOLDER_MAX   90
#define ARM_FOREARM_MIN   0
#define ARM_FOREARM_MAX   180
#define ARM_WRIST_MIN   0
#define ARM_WRIST_MAX   180
#define ARM_CLAW_MIN   5
#define ARM_CLAW_MAX   150

#define LIGHT_MODE_IDLE        0
#define LIGHT_MODE_TRIM_LEFT   2
#define LIGHT_MODE_TRIM_RIGHT  3

#define CONTROL_DEADZONE 15

// Initialize USB Hooks
USB Usb;
PS3USB PS3(&Usb); // This will just create the instance
//PS3USB PS3(&Usb,0x00,0x15,0x83,0x3D,0x0A,0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

// Initialize EasyTransfer Object
EasyTransfer ET; 

struct SEND_DATA_STRUCTURE{
    int throttle = 0;
    int reverse = 0;
    int steering = 128;
    boolean breaking = false;
    int armBase = 90;
    int armSholder = 50;
    int armForearm = 90;
    int armWrist = 90; 
    int armClaw = 80; 
};

SEND_DATA_STRUCTURE rocketcontrols;

volatile int armMode = ARM_MODE_DRIVING;

volatile int trim = TRIM_DEFAULT;
volatile int controllerLightMode = LIGHT_MODE_IDLE;
bool idleReset = false;

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
    if (DEBUG_PRINT) Serial.println("Serial is ready.");
    
    // Initialize Easy-Transfer Lib
    ET.begin(details(rocketcontrols), &Serial);
    
    // Initialize USB
    if (Usb.Init() == -1) { // Halt!
        if (DEBUG_PRINT) Serial.println("USB.Init() did not start..");
        while(Usb.Init() == -1) { 
            if (DEBUG_PRINT) Serial.println("Waiting on USB.init()...");
            delay(500);
        }
    }
    
    if (DEBUG_PRINT) Serial.println("USB Initialized!");
    
    randomSeed(analogRead(0));
    
    setArmMode(ARM_MODE_DRIVING);
    
    delay(200);     
}

void loop() {

    Usb.Task();
    
    
    // steering
    rocketcontrols.steering = processSteering();
    
    // Steering Trim
    if (PS3.getButtonClick(RIGHT)) {
        adjustTrim(TRIM_AMOUNT);
    } else if (PS3.getButtonClick(LEFT)) {
        adjustTrim(-TRIM_AMOUNT);
    }
    
    // Throttle
    if(PS3.getAnalogButton(R2)) {
        rocketcontrols.throttle = int( PS3.getAnalogButton(R2) );
    } else {
        rocketcontrols.throttle = 0;
    }

    // Mini-Throttle
    if(PS3.getAnalogButton(R1)) {
      rocketcontrols.throttle = 40;
    }
    
    // Reverse
    if(PS3.getAnalogButton(L2)) {
        rocketcontrols.reverse = int( PS3.getAnalogButton(L2) );
    } else {
        rocketcontrols.reverse = 0;
    }
        
    int deadzone = 15;      
    
    if(PS3.getAnalogButton(TRIANGLE)) {
        rocketcontrols.armClaw = rocketcontrols.armClaw + 1;
    } else if (PS3.getAnalogButton(CROSS)) {
        rocketcontrols.armClaw = rocketcontrols.armClaw - 1;
    }
    
    if (PS3.getAnalogButton(SQUARE)) {
      setArmMode(ARM_MODE_DRIVING);
    } else if (PS3.getAnalogButton(CIRCLE)) {
      setArmMode(ARM_MODE_CUP); 
    }
    
    
    if (PS3.getAnalogButton(UP)) {
      rocketcontrols.armForearm = 80;
    } else if (PS3.getAnalogButton(DOWN)) {
      rocketcontrols.armForearm = 100;
    } else {
      rocketcontrols.armForearm = 90;
    }
    
    
    rocketcontrols.armBase = controlDelta( rocketcontrols.armBase, int(PS3.getAnalogHat(RightHatX)), CONTROL_DEADZONE, 5);
    //rocketcontrols.armSholder = controlDelta( rocketcontrols.armSholder, int(PS3.getAnalogHat(LeftHatY)), CONTROL_DEADZONE, 5);
    //rocketcontrols.armForearm = controlDelta( rocketcontrols.armForearm, int(PS3.getAnalogHat(RightHatY)), CONTROL_DEADZONE, 5);
    //rocketcontrols.armWrist = controlDelta( rocketcontrols.armWrist, int(PS3.getAnalogHat(RightHatX)), CONTROL_DEADZONE, 5);

  
    
    // Apply Boundaries
    rocketcontrols.armBase = constrain(rocketcontrols.armBase, ARM_BASE_MIN, ARM_BASE_MAX);
    rocketcontrols.armSholder = constrain(rocketcontrols.armSholder, ARM_SHOLDER_MIN, ARM_SHOLDER_MAX);
    //rocketcontrols.armForearm = constrain(rocketcontrols.armForearm, ARM_FOREARM_MIN, ARM_FOREARM_MAX);
    //rocketcontrols.armWrist = constrain(rocketcontrols.armWrist, ARM_WRIST_MIN, ARM_WRIST_MAX);
    rocketcontrols.armClaw = constrain(rocketcontrols.armClaw, ARM_CLAW_MIN, ARM_CLAW_MAX);


    if (DEBUG_PRINT) {
        
        Serial.print("throttle=");
        Serial.print(rocketcontrols.throttle);
        Serial.print(", steering=");
        Serial.print(rocketcontrols.steering);
        Serial.print(", reverse=");
        Serial.print(rocketcontrols.reverse);
        Serial.print(", breaking=");
        Serial.print(rocketcontrols.breaking);
        Serial.print(", armBase=");
        Serial.print(rocketcontrols.armBase);
        Serial.print(", armSholder=");
        Serial.print(rocketcontrols.armSholder);
        Serial.print(", armForearm=");
        Serial.print(rocketcontrols.armForearm);
        Serial.print(", armWrist=");
        Serial.print(rocketcontrols.armWrist);
        Serial.print(", armClaw=");
        Serial.print(rocketcontrols.armClaw);
        Serial.println("");
        
    } else {
        
      ET.sendData();
        
    }
    
    processControllerLights();

    delay(30);
}


void setArmMode(int armModeToSet) {
  
  if (armModeToSet == ARM_MODE_DRIVING) {
    rocketcontrols.armBase = 90;
    rocketcontrols.armSholder = 50;
  } else {
    rocketcontrols.armBase = 90;
    rocketcontrols.armSholder = 20;
  }
  
}

int controlDelta(int sourceVal, int sourceControl, int deadzone, int maxSteps) {
    int deltaVal = 0;
    int calculatedVal = 0;
    if (sourceControl >= 125 + deadzone) {
        deltaVal = map(sourceControl, 128, 0, 1, maxSteps);
        calculatedVal = sourceVal - deltaVal;
    } else if (sourceControl <= 125 - deadzone) {
        deltaVal = map(sourceControl, 128, 255, 1, maxSteps);
        calculatedVal = sourceVal + deltaVal;
      

    } else {
        calculatedVal = sourceVal;
    }

    return calculatedVal;
}


void adjustTrim(int direction) {
    trim = constrain(trim + direction, -(TRIM_MAX), TRIM_MAX);
    if (direction > 0) {
     controllerLightMode = LIGHT_MODE_TRIM_RIGHT;   
    } else {
     controllerLightMode = LIGHT_MODE_TRIM_LEFT;   
    }
}

int processSteering() { 
    int steering =  PS3.getAnalogHat(LeftHatX) + trim;      
    steering = constrain(steering, 0, 255);
    return steering;
}

int raceLightIndex = 0;
int raceLightCounter = 0;
int lightFireCounter = 0;
int lightTrimCounter = 0;
LEDEnum LEDs[4] = {LED4, LED3, LED2, LED1};
void processControllerLights() {
    
    switch(controllerLightMode) {
  case LIGHT_MODE_IDLE:

        
     // Race Lights
    raceLightCounter++;
    if (raceLightCounter != 5) return;
    raceLightCounter = 0;
    for (int led = 0; led < 4; led++) {
        if (led == raceLightIndex) {
            PS3.setLedOn(LEDs[led]);
        } else {
            PS3.setLedOff(LEDs[led]);
        }
    }  
    raceLightIndex++;
    if (raceLightIndex > 3) raceLightIndex = 0;
        
 break;
  case LIGHT_MODE_TRIM_LEFT:

    PS3.setLedOn(LED4);
    PS3.setLedOn(LED3);
    PS3.setLedOff(LED2);
    PS3.setLedOff(LED1);

    lightTrimCounter++;
    if (lightTrimCounter == 18) {
         lightTrimCounter = 0;   
        controllerLightMode = LIGHT_MODE_IDLE;
    }

 break;
  case LIGHT_MODE_TRIM_RIGHT:
  
      PS3.setLedOff(LED4);
      PS3.setLedOff(LED3);
      PS3.setLedOn(LED2);
      PS3.setLedOn(LED1);
  
    lightTrimCounter++;
    if (lightTrimCounter == 18) {
         lightTrimCounter = 0;   
        controllerLightMode = LIGHT_MODE_IDLE;
    }

 break;
    }
    
}

