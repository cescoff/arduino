//#include <OneWire.h> 
//#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 8 on the Arduino 
//#define ONE_WIRE_BUS 2

//OneWire oneWire(ONE_WIRE_BUS); 
//DallasTemperature sensors(&oneWire);

const bool debug = false;
const bool handleVoltage = false;

// INPUTS

const int switch1 = 2;
const int switch2 = 3;

// OUTPUTS
const int outputRelay1 = 7;
const int outputRelay2 = 8;

// States
const int ON = 1;
const int OFF = 2;

int SWITCH1_STATE = OFF;
int SWITCH2_STATE = OFF;

int SWITCH_STATES[2] = {SWITCH1_STATE, SWITCH2_STATE};

int RELAY1_STATE = OFF;
int RELAY2_STATE = OFF;
int RELAY3_STATE = OFF;
int RELAY4_STATE = OFF;
int RELAY5_STATE = OFF;
int RELAY6_STATE = OFF;

String names[2] = {"YELLOW_LIGHTS", "BLUE_LIGHTS"};

int switchValues[][2] = {{switch1, HIGH}, {switch2, HIGH}};
// [SWITCH_ID, OUTPUT_ID, OUTPUT_STATE, STATE_LESS, NAME_POINTER, VOLTAGE_THRESHOLD]
int switchBindings[][6] = {{switch1, outputRelay1, RELAY1_STATE, 1, 1, 10}, {switch2, outputRelay2, RELAY2_STATE, 1, 2, 11}};

// LOOOOOOOOP
int loopCount = 0;

const long wait = 5;

void setup(){
   pinMode(batteryVoltageSensor, INPUT);
   pinMode(switch1, INPUT);
   pinMode(switch2, INPUT);

// Turn on pull up resistors
   digitalWrite(switch1, HIGH);
   digitalWrite(switch2, HIGH);

   pinMode(outputRelay1, OUTPUT);
   pinMode(outputRelay2, OUTPUT);

   delay(1000);

   digitalWrite(outputRelay1, LOW);
   digitalWrite(outputRelay2, LOW);

   Serial.begin(9600);
   Serial.println("VAN LIFE V2");
}

void loop() {

  switchValues[0][1] = digitalRead(switchValues[0][0]);
  switchValues[1][1] = digitalRead(switchValues[1][0]);
  
  for (int i=0; i<sizeof switchValues/sizeof switchValues[0]; i++) {
      if (debug) {
        int swId = i + 1;
        Serial.print("Switch");
        Serial.print(swId);
        Serial.print(" value is ");
        Serial.print(switchValues[i][1]);
        Serial.print(" (mapping=");
        Serial.print(switchValues[i][0]);
        Serial.println(")");
      }
      if (switchValues[i][1] == LOW) {
        if (SWITCH_STATES[i] == OFF) {
          SWITCH_STATES[i] = ON;
          changeStateForSwicth(switchValues[i][0], switchValues[i][1]);
        }
      } else {
        if (SWITCH_STATES[i] == ON) {
          SWITCH_STATES[i] = OFF;
          if (!isStateLessSwitch(switchValues[i][0])) {
            changeStateForSwicth(switchValues[i][0], switchValues[i][1]);
          }
        }
      }
      if (isOutputOn(switchValues[i][0]) && isVoltageDisabled(switchValues[i][0])) {
        changeStateForSwicth(switchValues[i][0], switchValues[i][1]);
      }
  }  

  

  if (debug) {
            Serial.println("_________________________________________");
  }

  loopCount++;
  delay(wait); 
}

bool isStateLessSwitch(int switchId) {
  for (int i=0; i<sizeof switchBindings/sizeof switchBindings[0]; i++) {
    if (switchBindings[i][0] == switchId) {
      return switchBindings[i][3] == 1;
    }
  }
  return false;  
}

void changeStateForSwicth(int switchId, int value) {
  for (int i=0; i<sizeof switchBindings/sizeof switchBindings[0]; i++) {
    if (switchBindings[i][0] == switchId) {
      if (switchBindings[i][2] == OFF) {
        if (!isVoltageDisabled(switchId)) {
          if (!debug) {
            digitalWrite(switchBindings[i][1], HIGH);
          }
          switchBindings[i][2] = ON;
          Serial.print("[ON] ");
        } else {
          Serial.print("[DISABLED] ");
        }
      } else if (switchBindings[i][2] == ON) {
        if (!debug) {
        digitalWrite(switchBindings[i][1], LOW);
        }
        switchBindings[i][2] = OFF;
        if (!isVoltageDisabled(switchId)) {
          Serial.print("[OFF] ");
        } else {
          Serial.print("[DISABLED] ");
        }
      }
        Serial.print(names[switchBindings[i][4]]);
        Serial.print(" (");
        Serial.print(value);
        Serial.println(")");
      return;
    }
  }
  Serial.println("NOT TURNED ON");
}

bool isOutputOn(int switchId) {
  for (int i=0; i<sizeof switchBindings/sizeof switchBindings[0]; i++) {
    if (switchBindings[i][0] == switchId) {
      if (switchBindings[i][2] == ON) {
        return true;        
      }
    }
  }  
  return false;
}
