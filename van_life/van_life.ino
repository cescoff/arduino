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
const int batteryVoltageSensor = A0;

const int switch1 = 2;
const int switch2 = 3;
const int switch3 = 4;
const int switch4 = 5;
const int switch5 = 6;

// OUTPUTS
const int outputRelay1 = 7;
const int outputRelay2 = 8;
const int outputRelay3 = 9;
const int outputRelay4 = 10;
const int outputRelay5 = 11;
const int outputRelay6 = 12;

// States
const int ON = 1;
const int OFF = 2;

int SWITCH1_STATE = OFF;
int SWITCH2_STATE = OFF;
int SWITCH3_STATE = OFF;
int SWITCH4_STATE = OFF;
int SWITCH5_STATE = OFF;

int SWITCH_STATES[5] = {SWITCH1_STATE, SWITCH2_STATE, SWITCH3_STATE, SWITCH4_STATE, SWITCH5_STATE};

int RELAY1_STATE = OFF;
int RELAY2_STATE = OFF;
int RELAY3_STATE = OFF;
int RELAY4_STATE = OFF;
int RELAY5_STATE = OFF;
int RELAY6_STATE = OFF;

String names[6] = {"GLOBAL", "YELLOW_LIGHTS", "BLUE_LIGHTS", "SOUND_SYSTEM", "WATER", "FRIDGE"};

int switchValues[][2] = {{switch1, HIGH}, {switch2, HIGH}, {switch3, HIGH}, {switch4, HIGH}, {switch5, HIGH}};
// [SWITCH_ID, OUTPUT_ID, OUTPUT_STATE, STATE_LESS, NAME_POINTER, VOLTAGE_THRESHOLD]
int switchBindings[][6] = {{switch1, outputRelay1, RELAY1_STATE, 1, 1, 10}, {switch2, outputRelay2, RELAY2_STATE, 1, 2, 11}, {switch3, outputRelay3, RELAY3_STATE, 0, 3, 11}, {switch5, outputRelay4, RELAY4_STATE, 0, 4, 12}, {switch4, outputRelay5, RELAY5_STATE, 0, 5, 12}};

// Voltage handling

// Sensor voltage divider resistor values
const float R1 = 30000.0; //  
const float R2 = 7500.0; // 

// Current window sum
float voltageAvgSum = 0;
int voltageReadCount = 0;
float voltageValuesPerMinute = -1;

// Minute average pointer
int voltageAvgPointer = 0;
float voltages[3] = {-1, -1, -1};

// LOOOOOOOOP
int loopCount = 0;

const long wait = 5;

void setup(){
   pinMode(batteryVoltageSensor, INPUT);
   pinMode(switch1, INPUT);
   pinMode(switch2, INPUT);
   pinMode(switch3, INPUT);
   pinMode(switch4, INPUT);
   pinMode(switch5, INPUT);

// Turn on pull up resistors
   digitalWrite(switch1, HIGH);
   digitalWrite(switch2, HIGH);
   digitalWrite(switch3, HIGH);
   digitalWrite(switch4, HIGH);
   digitalWrite(switch5, HIGH);

   pinMode(outputRelay1, OUTPUT);
   pinMode(outputRelay2, OUTPUT);
   pinMode(outputRelay3, OUTPUT);
   pinMode(outputRelay4, OUTPUT);
   pinMode(outputRelay5, OUTPUT);
   pinMode(outputRelay6, OUTPUT);

   delay(1000);

   digitalWrite(outputRelay1, LOW);
   digitalWrite(outputRelay2, LOW);
   digitalWrite(outputRelay3, LOW);
   digitalWrite(outputRelay4, LOW);
   digitalWrite(outputRelay5, LOW);
   digitalWrite(outputRelay6, LOW);

   voltageValuesPerMinute = (60 * 1000) / wait;

   Serial.begin(9600);
   Serial.println("VAN LIFE V2");
}

void loop() {
  voltageAvgSum += readVoltage();
  voltageReadCount++;

  if (voltageReadCount > voltageValuesPerMinute) {
    voltages[voltageAvgPointer] = voltageAvgSum / voltageReadCount;
    voltageAvgPointer = (voltageAvgPointer + 1) % 3;
    voltageReadCount = 0;
  }
  
  switchValues[0][1] = digitalRead(switchValues[0][0]);
  switchValues[1][1] = digitalRead(switchValues[1][0]);
  switchValues[2][1] = digitalRead(switchValues[2][0]);
  switchValues[3][1] = digitalRead(switchValues[3][0]);
  switchValues[4][1] = digitalRead(switchValues[4][0]);
  
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

float readVoltage() {
   if (!handleVoltage) {
      return 13;
   }
   float voltageValue = analogRead(batteryVoltageSensor);
   float vout = (voltageValue * 5.0) / 1024.0; // see text
   return (vout / (R2/(R1+R2))); 
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

int isOutputOn(int switchId) {
  for (int i=0; i<sizeof switchBindings/sizeof switchBindings[0]; i++) {
    if (switchBindings[i][0] == switchId) {
      if (switchBindings[i][2] == ON) {
        return true;        
      }
    }
  }  
  return false;
}

bool isVoltageDisabled(int switchId) {
  int voltageMinutesCount = 0;
  float voltageSum = 0;
  for (int i=0; i<sizeof voltages/sizeof voltages[0]; i++) {
    if (voltages[i] >= 0) {
      voltageSum += voltages[i];
      voltageMinutesCount++;
    }
  }

  if (voltageMinutesCount <= 0) {
    return false;
  }

  float avgVoltage = voltageSum / voltageMinutesCount;
  
  for (int i=0; i<sizeof switchBindings/sizeof switchBindings[0]; i++) {
    if (switchBindings[i][0] == switchId) {
      if (avgVoltage < switchBindings[i][5]) {
        Serial.print("Switch ");
        Serial.print(names[switchBindings[i][4]]);
        Serial.print(" is disabled because voltage is ");
        Serial.print(avgVoltage);
        Serial.println("V");
        return true;
      }
    }
  }  
  return false;
}
