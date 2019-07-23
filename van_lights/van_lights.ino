#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

// INPUTS
int contactInput = A5;
int soundSystemInput = A0;
int frontDoorInput = A3;
int rearDoorInput = A4;
int yellowLightSwitch = A1;
int blueLightSwitch = A2;

// OUTPUTS
int yellowLightOutput = 4;
int blueLightOutput = 5;
int soundSystemPower = 7;
int soundSystemIgnite = 6;

const int debug = 1;

const int bindBlueWithYellow = 1;

const long WAIT = 50;

const int CONTACT_ON = 1;
const int CONTACT_OFF = 2;
const int SOUND_SYSTEM_SWITCH_ON = 3;
const int SOUND_SYSTEM_SWITCH_OFF = 4;
const int FRONT_DOOR_OPEN = 5;
const int FRONT_DOOR_CLOSE = 6;
const int REAR_DOOR_OPEN = 7;
const int REAR_DOOR_CLOSE = 8;
const int YELLOW_SWITCH_ON = 9;
const int YELLOW_SWITCH_OFF = 10;
const int BLUE_SWITCH_ON = 11;
const int BLUE_SWITCH_OFF = 12;
const int REMOTE_DOOR_OPEN = 13;
const int REMOTE_DOOR_CLOSE = 14;
const int REMOTE_LIGHT = 15;
const int CHANGE_YELLOW_LIGHT_STATE = 16;
const int CHANGE_BLUE_LIGHT_STATE = 17;
const int CHANGE_LIGHTS_STATE = 18;
const int LIGHTS_ON = 19;
const int LIGHTS_OFF = 20;

const int ON = 1;
const int OFF = 2;
const int OPEN = 3;
const int CLOSE = 4;
const int OPENED = 5;
const int CLOSED = 6;

long remoteControlOpenValue[] = {7674210, 7685618};
long remoteControlCloseValue[] = {7674209, 7685617};
long remoteControlLightValue[] = {7674212, 7685620};
int remoteControlProtocol = 1;
int remoteControlBitLength = 24;

int minVoltage = 11;

float R1 = 30000.0; //  
float R2 = 7500.0; // 

float vout = 0.0;

int contactValue = 0;
float vinContact = 0.0;
int contactState = OFF;

int soundSystemValue = 0;
float vinSoundSystem = 0.0;
int soundSystemState = OFF;

int soundSystemSwitchState = OFF;

int frontDoorValue = 0;
float vinFrontDoor = 0.0;
int frontDoorState = CLOSED;

int rearDoorValue = 0;
float vinRearDoor = 0.0;
int rearDoorState = CLOSED;

int yellowLightSwitchValue = 0;
float vinYellowLightSwitch = 0.0;
int yellowLightSwicthState = OFF;
int yellowLightState = OFF;


int blueLightSwitchValue = 0;
float vinBlueLightSwitch = 0.0;
int blueLightSwicthState = OFF;
int blueLightState = OFF;

float loopCount = 0;

float contactOffMaxCount;
int contactOffCount = -1;

void setup(){
   pinMode(contactInput, INPUT);
   pinMode(soundSystemInput, INPUT);
   pinMode(frontDoorInput, INPUT);
   pinMode(rearDoorInput, INPUT);
   pinMode(yellowLightSwitch, INPUT);
   pinMode(blueLightSwitch, INPUT);

   pinMode(yellowLightOutput, OUTPUT);
   pinMode(blueLightOutput, OUTPUT);
   pinMode(soundSystemPower, OUTPUT);
   pinMode(soundSystemIgnite, OUTPUT);

   if (soundSystemState == OFF) {
    digitalWrite(soundSystemPower, LOW);
    digitalWrite(soundSystemIgnite, LOW);
   }
   if (yellowLightState == OFF) {
    digitalWrite(yellowLightOutput, LOW);
   }
   if (yellowLightState == OFF) {
    digitalWrite(blueLightOutput, LOW);
   }

   contactOffMaxCount = (5 * 60 * 1000) / WAIT;
   contactOffCount = -1;
   
   mySwitch.enableReceive(0);

   Serial.begin(9600);
   Serial.print("VAN LIGHT V2");
}
void loop(){
   // read the value at analog input
   contactValue = analogRead(contactInput);
   vout = (contactValue * 5.0) / 1024.0; // see text
   vinContact = vout / (R2/(R1+R2)); 

   soundSystemValue = analogRead(soundSystemInput);
   vout = (soundSystemValue * 5.0) / 1024.0; // see text
   vinSoundSystem = vout / (R2/(R1+R2)); 

   frontDoorValue = analogRead(frontDoorInput);
   vout = (frontDoorValue * 5.0) / 1024.0; // see text
   vinFrontDoor = vout / (R2/(R1+R2)); 

   rearDoorValue = analogRead(rearDoorInput);
   vout = (rearDoorValue * 5.0) / 1024.0; // see text
   vinRearDoor = vout / (R2/(R1+R2)); 

   yellowLightSwitchValue = analogRead(yellowLightSwitch);
   vout = (yellowLightSwitchValue * 5.0) / 1024.0; // see text
   vinYellowLightSwitch = vout / (R2/(R1+R2)); 
   
   blueLightSwitchValue = analogRead(blueLightSwitch);
   vout = (blueLightSwitchValue * 5.0) / 1024.0; // see text
   vinBlueLightSwitch = vout / (R2/(R1+R2)); 

   if (debug == 1 && loopCount == 1000) {
      printVoltage("Contact", vinContact);
      printVoltage("Sound system", vinSoundSystem);
      printVoltage("Front door", vinFrontDoor);
      printVoltage("Rear door", vinRearDoor);
      printVoltage("Yellow light switch", vinYellowLightSwitch);
      printVoltage("Blue light switch", vinBlueLightSwitch);
      Serial.println("___________________________________________________________");
      loopCount = 0;
   } else if (debug != 1) {
      loopCount = 0;
   }

   loopCount++;

   if (vinContact >= minVoltage) {
      handleContact(ON);
   } else {
      handleContact(OFF);
   }

   if (vinSoundSystem >= minVoltage) {
      handleSoundSystemSwitch(ON);
   } else {
      handleSoundSystemSwitch(OFF);
   }

   if (vinFrontDoor >= minVoltage) {
    handleFrontDoor(OPEN);
   } else {
    handleFrontDoor(CLOSE);
   }

   if (vinRearDoor >= minVoltage) {
    handleRearDoor(OPEN);
   } else {
    handleRearDoor(CLOSE);
   }

   if (vinYellowLightSwitch  >= minVoltage) {
    handleYellowSwicth(ON);
   } else {
    handleYellowSwicth(OFF);
   }

   if (vinBlueLightSwitch >= minVoltage) {
    handleBlueSwicth(ON);
   } else {
    handleBlueSwicth(OFF);
   }

  if (mySwitch.available()) {
    
    long value = mySwitch.getReceivedValue();
    int protocol = mySwitch.getReceivedProtocol();
    int bitLength = mySwitch.getReceivedBitlength();
//    if (protocol == remoteControlProtocol && bitLength == remoteControlBitLength) {
    if (protocol == remoteControlProtocol) {
       if (isRemoteOpen(value)) {
        handleRemote(OPEN);
       } else if (isRemoteClose(value)) {
        handleRemote(CLOSE);        
       } else if (isRemoteLight(value)) {
        handleRemote(ON);        
       }
    }
    if (debug == 1) {
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );
    }

    mySwitch.resetAvailable();
  }
    
   delay(WAIT);
}

void handleEvent(int event) {
  if (event == CONTACT_ON) {
    Serial.println(" /|\\ [ON] Contact");
    if (isSoundSystemOff()) {
       turnOnSoundSystem();
    }
  } else if (event == CONTACT_OFF) {
    Serial.println(" /|\\ [OFF] Contact");
    if (isSoundSystemOn() && soundSystemSwitchState == OFF) {
       turnOffSoundSystem();
    }
  }

  if (event == SOUND_SYSTEM_SWITCH_ON || event == SOUND_SYSTEM_SWITCH_OFF) {
    Serial.println(" /|\\ [STATE] Sound system switch");
    if (contactState == OFF) {
      if (isSoundSystemOff()) {
         turnOnSoundSystem();
      } else {
         turnOffSoundSystem();
      }
    }
  }

  if (event == YELLOW_SWITCH_ON || event == YELLOW_SWITCH_OFF) {
    Serial.println(" /|\\ [STATE] Yellow light switch");
    handleEvent(CHANGE_YELLOW_LIGHT_STATE);
  }
    
  if (event == BLUE_SWITCH_ON || event == BLUE_SWITCH_OFF) {
    Serial.println(" /|\\ [STATE] Blue light switch");
    handleEvent(CHANGE_BLUE_LIGHT_STATE);
  }

  if (event == FRONT_DOOR_OPEN) {
    Serial.println(" /|\\ [OPEN] Front door");
    if (isRearDoorClosed()) {
      handleEvent(LIGHTS_ON);
    }
  } else if (event == FRONT_DOOR_CLOSE) {
    Serial.println(" /|\\ [CLOSE] Front door");
    if (isRearDoorClosed()) {
       handleEvent(LIGHTS_OFF);
    }
  }
    
  if (event == REAR_DOOR_OPEN) {
    Serial.println(" /|\\ [OPEN] Rear door");
    handleEvent(LIGHTS_ON);
  } else if (event == REAR_DOOR_CLOSE) {
    Serial.println(" /|\\ [CLOSE] Rear door");
    if (isFrontDoorClosed() && !isBaseCampMode()) {
      handleEvent(LIGHTS_OFF);
    }
  }

  if (event == REMOTE_DOOR_OPEN) {
    Serial.println(" /|\\ [OPEN] Remote");
//    handleEvent(LIGHTS_ON);
  } else if (event == REMOTE_DOOR_CLOSE) {
    Serial.println(" /|\\ [CLOSE] Remote");
/*    handleEvent(LIGHTS_OFF);
    if (bindBlueWithYellow != 1) {
      if (isBlueLightsOn()) {
        turnOffBlueLights();
      }
    }*/
  } else if (event == REMOTE_LIGHT) {
    Serial.println(" /|\\ [ON] Remote");
    handleEvent(CHANGE_LIGHTS_STATE);
    if (bindBlueWithYellow != 1) {
      if (isBlueLightsOff()) {
        turnOnBlueLights();
      }
    }
  }

  if (event == LIGHTS_ON) {
    Serial.println(" /|\\ [ON] Lights");
    if (isYellowLightsOff()) {
      turnOnYellowLights();
    }
    if (bindBlueWithYellow == 1) {
      if (isBlueLightsOff()) {
        turnOnBlueLights();
      }
    }
  } else if (event == LIGHTS_OFF) {
    Serial.println(" /|\\ [OFF] Lights");
    if (isYellowLightsOn()) {
      turnOffYellowLights();
    }
    if (bindBlueWithYellow == 1) {
      if (isBlueLightsOn()) {
        turnOffBlueLights();
      }
    }
  }

  if (event == CHANGE_LIGHTS_STATE) {
    Serial.println(" /|\\ [STATE] Lights");
    if (isYellowLightsOff() && isBlueLightsOff()) {
      handleEvent(LIGHTS_ON);
    } else {
      handleEvent(LIGHTS_OFF);
    }
  }

  if (event == CHANGE_YELLOW_LIGHT_STATE) {
    Serial.println(" /|\\ [STATE] Yellow lights");
      if (isYellowLightsOff()) {
        turnOnYellowLights();
      } else {
        turnOffYellowLights();
      }
  }
  
  if (event == CHANGE_BLUE_LIGHT_STATE) {
    Serial.println(" /|\\ [STATE] Blue lights");
      if (isBlueLightsOff()) {
        turnOnBlueLights();
      } else {
        turnOffBlueLights();
      }
  }
  
}

void handleContact(int action) {
  if (action == ON && contactState == OFF) {
    contactOffCount = 0;
    contactState = ON;
    handleEvent(CONTACT_ON);
  }
  if (action == OFF && contactState == ON) {
    contactState = OFF;
    handleEvent(CONTACT_OFF);
  }
  if (action == OFF) {
      if (contactOffCount <= contactOffMaxCount) {
        contactOffCount++;
      }
  }
}

void handleSoundSystemSwitch(int action) {
  if (action == ON && soundSystemSwitchState == OFF) {
    soundSystemSwitchState = ON;
    handleEvent(SOUND_SYSTEM_SWITCH_ON);
  }
  if (action == OFF && soundSystemSwitchState == ON) {
    soundSystemSwitchState = OFF;
    handleEvent(SOUND_SYSTEM_SWITCH_OFF);
  }
}

void handleYellowSwicth(int action) {
  if (action == ON && yellowLightSwicthState == OFF) {
    yellowLightSwicthState = ON;
    handleEvent(YELLOW_SWITCH_ON);
  }
  if (action == OFF && yellowLightSwicthState == ON) {
    yellowLightSwicthState = OFF;
    handleEvent(YELLOW_SWITCH_OFF);
  }
}

void handleBlueSwicth(int action) {
  if (action == ON && blueLightSwicthState == OFF) {
    blueLightSwicthState = ON;
    handleEvent(BLUE_SWITCH_ON);
  }
  if (action == OFF && blueLightSwicthState == ON) {
    blueLightSwicthState = OFF;
    handleEvent(BLUE_SWITCH_OFF);
  }
}

void handleFrontDoor(int action) {
  if (action == OPEN && frontDoorState == CLOSED) {
    frontDoorState = OPENED;
    handleEvent(FRONT_DOOR_OPEN);
  }
  if (action == CLOSE && frontDoorState == OPENED) {
    frontDoorState = CLOSED;
    handleEvent(FRONT_DOOR_CLOSE);
  }
}

void handleRearDoor(int action) {
  if (action == OPEN && rearDoorState == CLOSED) {
    rearDoorState = OPENED;
    handleEvent(REAR_DOOR_OPEN);
  }
  if (action == CLOSE && rearDoorState == OPENED) {
    rearDoorState = CLOSED;
    handleEvent(REAR_DOOR_CLOSE);
  }
}

void handleRemote(int action) {
  if (action == OPEN) {
    handleEvent(REMOTE_DOOR_OPEN);
  }
  if (action == CLOSE) {
    handleEvent(REMOTE_DOOR_CLOSE);
  }
  if (action == ON) {
    handleEvent(REMOTE_LIGHT);
  }
}

bool isSoundSystemOn() {
  return soundSystemState == ON;
}

bool isSoundSystemOff() {
  return soundSystemState == OFF;
}

bool isYellowLightsOn() {
  return yellowLightState == ON;
}

bool isYellowLightsOff() {
  return yellowLightState == OFF;
}

bool isBlueLightsOn() {
  return blueLightState == ON;
}

bool isBlueLightsOff() {
  return blueLightState == OFF;
}

bool isFrontDoorOpened() {
  return frontDoorState == OPENED;
}

bool isFrontDoorClosed() {
  return frontDoorState == CLOSED;
}

bool isRearDoorOpened() {
  return rearDoorState == OPENED;
}

bool isRearDoorClosed() {
  return rearDoorState == CLOSED;
}

bool isBaseCampMode() {
  return contactOffCount >= (contactOffMaxCount - 1);
}

bool isRemoteOpen(long test) {
  for (int i=0; i<sizeof remoteControlOpenValue/sizeof remoteControlOpenValue[0]; i++) {
    if (remoteControlOpenValue[i] == test) {
      return true;
    }
  }
  return false;
}

bool isRemoteClose(long test) {
  for (int i=0; i<sizeof remoteControlCloseValue/sizeof remoteControlCloseValue[0]; i++) {
    if (remoteControlCloseValue[i] == test) {
      return true;
    }
  }
  return false;
}

bool isRemoteLight(long test) {
  for (int i=0; i<sizeof remoteControlLightValue/sizeof remoteControlLightValue[0]; i++) {
    if (remoteControlLightValue[i] == test) {
      return true;
    }
  }
  return false;
}


void printVoltage(char Category[], float value) {
      Serial.print(Category);
      Serial.print(" voltage ");
      Serial.print(value);
      Serial.println("V");
}

void turnOnSoundSystem() {
     if (soundSystemState == OFF) {
       Serial.println(" ++ SoundSystem turned on");
       digitalWrite(soundSystemPower, HIGH);
       delay(500);
       digitalWrite(soundSystemIgnite, HIGH);
     }
     soundSystemState = ON; 
}

void turnOffSoundSystem() {
     if (soundSystemState == ON) {
       Serial.println(" -- SoundSystem turned off");
       digitalWrite(soundSystemIgnite, LOW);
       delay(500);
       digitalWrite(soundSystemPower, LOW);
     }
     soundSystemState = OFF;
}

void changeYellowLightState() {
     if (yellowLightState == ON) {
        turnOnYellowLights();
     } else {
        turnOffYellowLights();
     }
}

void turnOnYellowLights() {
        if (yellowLightState == OFF) {
          Serial.println(" ++ Yellow lights turned on");
          digitalWrite(yellowLightOutput, HIGH);
        }
        yellowLightState = ON;
}

void turnOffYellowLights() {
     if (yellowLightState == ON) {
       Serial.println(" -- Yellow lights turned off");
          digitalWrite(yellowLightOutput, LOW);
     }
     yellowLightState = OFF;
}

void changeBlueLightState() {
     if (blueLightState == OFF) {
        turnOnBlueLights();
     } else {
        turnOffBlueLights();
     }
}

void turnOnBlueLights() {
        if (blueLightState == OFF) {
          Serial.println(" ++ Blue lights turned on");
          digitalWrite(blueLightOutput, HIGH);
        }
        blueLightState = ON;
}

void turnOffBlueLights() {
     if (blueLightState == ON) {
       Serial.println(" -- Blue lights turned off");
          digitalWrite(blueLightOutput, LOW);
     }
     blueLightState = OFF;
}
