#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

// CONSTANTS
const bool debug = true;

const bool LIGHT_SWITCHES_STATELESS = true;

const long OPEN_DOORS_MESSAGES[] = {16630658, 8346754, 7674210, 7685618};
const long CLOSE_DOORS_MESSAGES[] = {16630664, 8346760, 7674209, 7685617};
const long OPEN_BACK_DOOR_MESSAGES[] = {7674212, 7685620};
const int REMOTE_CONTROL_PROTOCOL = 1;

const int OPENED = 1;
const int CLOSED = 2;

const int ON = 3;
const int OFF = 4;

// DELAYS
const int DOORS_COMMAND_DELAY = 200;
const int COMMAND_SPACER_DELAY = 300;

const int WAIT = 5;

// INPUTS 2, 3, 4, 5, 8
const int IGNITE_INPUT = 4;
const int FRONT_DOORS = 5;
const int FRONT_LIGHTS_SWITCH = 3;
const int BACK_LIGHT_SWITCH = 8;

// 433 RADIO
const int RADIO_DATA = 2;

// OUTPUTS
const int OPEN_DOORS = 11;
const int CLOSE_DOORS = 12;
const int FRONT_LIGHTS = 6;
const int BACK_LIGHT = 7;
const int BACK_DOOR_1 = 9;
const int REAR_DOOR_2 = 10;

// RADIO 2
// FRONT SWITCH TEST 3
// IGNITE TEST 4
// DOORS TESTS 5
// FRONT LIGHT RELAY 6
// BACK LIGHT RELAY 7
// BACK SWITCH TEST 8
// BACK DOOR NR1 9
// BACK DOOR NR2 10
// OPEN DOORS 11
// CLOSE DOORS 12

// STATES
int igniteState = OFF;
int frontDoorsState = CLOSED;

int frontLightState = OFF;
int backLightState = OFF;

void setup() {
// INPUTS SETUP
   pinMode(IGNITE_INPUT, INPUT);
   pinMode(FRONT_DOORS, INPUT);
   pinMode(FRONT_LIGHTS_SWITCH, INPUT);
   pinMode(BACK_LIGHT_SWITCH, INPUT);

// Turn on pull up resistors
   digitalWrite(IGNITE_INPUT, HIGH);
   digitalWrite(FRONT_DOORS, HIGH);
   digitalWrite(FRONT_LIGHTS_SWITCH, HIGH);
   digitalWrite(BACK_LIGHT_SWITCH, HIGH);

// OUTPUTS SETUP
   pinMode(OPEN_DOORS, OUTPUT);
   pinMode(CLOSE_DOORS, OUTPUT);
   pinMode(FRONT_LIGHTS, OUTPUT);
   pinMode(BACK_LIGHT, OUTPUT);
   pinMode(BACK_DOOR_1, OUTPUT);
   pinMode(REAR_DOOR_2, OUTPUT);
   

   digitalWrite(OPEN_DOORS, LOW);
   digitalWrite(CLOSE_DOORS, LOW);
   digitalWrite(FRONT_LIGHTS, LOW);
   digitalWrite(BACK_LIGHT, LOW);
   digitalWrite(BACK_DOOR_1, LOW);
   digitalWrite(REAR_DOOR_2, LOW);

   mySwitch.enableReceive(0);
   
   Serial.begin(9600);
   Serial.println("CAR MAIN STARTED");
}

void loop() {
  readIgniteState();
  readDoorsStates();
  readLightSwitches();

  if (mySwitch.available()) {
debugPrint("Message available");    
    long value = mySwitch.getReceivedValue();
    int protocol = mySwitch.getReceivedProtocol();
    int bitLength = mySwitch.getReceivedBitlength();
//    if (protocol == remoteControlProtocol && bitLength == remoteControlBitLength) {
    if (protocol == REMOTE_CONTROL_PROTOCOL) {
       if (isRemoteOpen(value)) {
        debugPrint("Remote open detected");
        openDoors();
       } else if (isRemoteClose(value)) {
        debugPrint("Remote close detected");
        closeDoors();        
       } else if (isRemoteBackDoor(value)) {
        openBackDoor();
       } else {
        debugPrint("Unknown command");
       }
    }
    if (debug) {
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

void debugPrint(String msg) {
  if (debug) {
    Serial.println("[DEBUG] : " + msg);
  }
}

void openDoors() {
   if (isIgniteOn()) {
    debugPrint("Do not open doors once ignite is on");
    return;
   }
   debugPrint("OPEN DOORS");
   digitalWrite(OPEN_DOORS, HIGH);
   delay(DOORS_COMMAND_DELAY);
   digitalWrite(OPEN_DOORS, LOW);
   frontLightState = ON;
   backLightState = ON;

   turnOnFrontLight();
   turnOnBackLight();
   delay(COMMAND_SPACER_DELAY);
}

void closeDoors() {
   if (isIgniteOn()) {
    debugPrint("Do not close doors once ignite is on");
    return;
   }
   if (isDoorOpened()) {
    debugPrint("Do not close doors once doors are opened");
    return;
   }
   debugPrint("CLOSE DOORS");
   digitalWrite(CLOSE_DOORS, HIGH);
   delay(DOORS_COMMAND_DELAY);
   digitalWrite(CLOSE_DOORS, LOW);
   frontLightState = OFF;
   backLightState = OFF;

   turnOffFrontLight();
   turnOffBackLight();
   delay(COMMAND_SPACER_DELAY);
}

void openBackDoor() {
   if (isIgniteOn()) {
    debugPrint("Do not open back door once ignite is on");
    return;
   }
   debugPrint("OPEN BACK DOOR");
   backLightState = ON;
   turnOnBackLight();
   delay(COMMAND_SPACER_DELAY);
}

bool isIgniteOn() {
  return igniteState == ON;
}

void turnOnFrontLight() {
  digitalWrite(FRONT_LIGHTS, HIGH);
}

void turnOffFrontLight() {
  digitalWrite(FRONT_LIGHTS, LOW);
}

void turnOnBackLight() {
  digitalWrite(BACK_LIGHT, HIGH);
}

void turnOffBackLight() {
  digitalWrite(BACK_LIGHT, LOW);
}

bool readLightSwitches() {
  bool result = false;
  
  int frontLightValue = digitalRead(FRONT_LIGHTS_SWITCH);
  int backLightValue = digitalRead(BACK_LIGHT_SWITCH);

  if (frontLightValue == LOW) {
    if (LIGHT_SWITCHES_STATELESS) {
      result = true;
      if (frontLightState == ON) {
        frontLightState = OFF;
        turnOffFrontLight();
        debugPrint("FRONT LIGHT OFF");
      } else {
        frontLightState = ON;
        turnOnFrontLight();
        debugPrint("FRONT LIGHT ON");
      }
    } else {
      if (frontLightState == OFF) {
        frontLightState = ON;
        turnOnFrontLight();
        result = true;
        debugPrint("FRONT LIGHT ON");
      }
    }
  } else {
    if (LIGHT_SWITCHES_STATELESS) {
      // Nothing to do here
    } else {
      if (frontLightState == ON) {
        frontLightState = OFF;
        turnOffFrontLight();
        result = true;
        debugPrint("FRONT LIGHT OFF");
      }
    }
  }

  if (backLightValue == LOW) {
    if (LIGHT_SWITCHES_STATELESS) {
      result = true;
      if (backLightState == ON) {
        backLightState = OFF;
        turnOffBackLight();
        debugPrint("BACK LIGHT OFF");
      } else {
        backLightState = ON;
        turnOnBackLight();
        debugPrint("BACK LIGHT ON");
      }
    } else {
      if (backLightState == OFF) {
        backLightState = ON;
        turnOnBackLight();
        result = true;
        debugPrint("BACK LIGHT ON");
      }
    }
  } else {
    if (LIGHT_SWITCHES_STATELESS) {
      // Nothing to do here
    } else {
      if (backLightState == ON) {
        backLightState = OFF;
        turnOffBackLight();
        result = true;
        debugPrint("BACK LIGHT OFF");
      }
    }
  }

  if (result) {
    delay(COMMAND_SPACER_DELAY);
  }

  return result;
}

bool readIgniteState() {
  bool result = false;
  int igniteValue = digitalRead(IGNITE_INPUT);
  
  if (igniteValue == LOW) {
    if (igniteState == OFF) {
      if (!isDoorOpened()) {
        frontLightState = OFF;
        backLightState = OFF;
        turnOffFrontLight();
        turnOffBackLight();
      }
      igniteState = ON;
      result = true;
      debugPrint("IGNITE ON");
    }
  } else {
    if (igniteState == ON) {
      igniteState = OFF;
      result = true;
      debugPrint("IGNITE OFF");
    }
  }
  return result;  
}

bool readDoorsStates() {
  bool result = false;
  int frontDoorsValue = digitalRead(FRONT_DOORS);

  if (frontDoorsValue == LOW) {
    if (frontDoorsState == CLOSED) {
      frontLightState = ON;
      backLightState = ON;

      turnOnFrontLight();
      turnOnBackLight();

      frontDoorsState = OPENED;
      result = true;
      debugPrint("FRONT DOORS OPENED");
    }
  } else {
    if (frontDoorsState == OPENED) {
      frontLightState = OFF;
      backLightState = OFF;

      turnOffFrontLight();
      turnOffBackLight();

      frontDoorsState = CLOSED;
      result = true;
      debugPrint("FRONT DOORS CLOSED");
    }
  }
  
  return result;  
}

bool isDoorOpened() {
  if (frontDoorsState == OPENED) {
    return true;
  }
  return false;
}

bool isRemoteOpen(long test) {
  for (int i=0; i<sizeof OPEN_DOORS_MESSAGES/sizeof OPEN_DOORS_MESSAGES[0]; i++) {
    if (OPEN_DOORS_MESSAGES[i] == test) {
      return true;
    }
  }
  return false;
}

bool isRemoteClose(long test) {
  for (int i=0; i<sizeof CLOSE_DOORS_MESSAGES/sizeof CLOSE_DOORS_MESSAGES[0]; i++) {
    if (CLOSE_DOORS_MESSAGES[i] == test) {
      return true;
    }
  }
  return false;
}

bool isRemoteBackDoor(long test) {
  for (int i=0; i<sizeof OPEN_BACK_DOOR_MESSAGES/sizeof OPEN_BACK_DOOR_MESSAGES[0]; i++) {
    if (OPEN_BACK_DOOR_MESSAGES[i] == test) {
      return true;
    }
  }
  return false;
}
