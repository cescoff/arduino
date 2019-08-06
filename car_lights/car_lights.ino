// OUTPUTS
const int FRONT_LIGHT_OUTPUT = 2;
const int BACK_LIGHT_OUTPUT = 3;
const int BACK_DOOR_COMMAND_1 = 4;
const int BACK_DOOR_COMMAND_2 = 5;
const int FRONT_DOORS_OPENED_LED = 6;
const int REAR_DOORS_OPENED_LED = 7;
const int BACK_DOOR_OPENED_LED = 8;

// CONSTANTS
const bool debug = false;

const int OPENED = 1;
const int CLOSED = 2;

const int ON = 3;
const int OFF = 4;

// DOOR STATE MESSAGE CONSTANTS
const String MSG_PREFIX = "MSG://";
const String FRONT_DOORS_STATE_MSG = "FD";
const String REAR_DOORS_STATE_MSG = "RD";
const String BACK_DOOR_STATE_MSG = "BD";
const String IGNITE_STATE_MSG = "IGN";

const String FRONT_LIGHT_STATE_MSG = "FL";
const String BACK_LIGHT_STATE_MSG = "BL";

const String DOOR_OPENED_MSG = "O";
const String DOOR_CLOSED_MSG = "C";
const String ON_MSG = "ON";
const String OFF_MSG = "OFF";

const char SEPARATOR = ',';
const char END_OF_MSG_CHAR = '\n';

const String PARSE_ERROR = "PARSE_ERROR";


String inputMessageString = "";
bool messageComplete = false;

int frontLightState = OFF;
int backLightState = OFF;



void setup() {
  pinMode(FRONT_LIGHT_OUTPUT, OUTPUT);
  pinMode(BACK_LIGHT_OUTPUT, OUTPUT);
  pinMode(BACK_DOOR_COMMAND_1, OUTPUT);
  pinMode(BACK_DOOR_COMMAND_2, OUTPUT);

  digitalWrite(FRONT_LIGHT_OUTPUT, LOW);
  digitalWrite(BACK_LIGHT_OUTPUT, LOW);
  digitalWrite(BACK_DOOR_COMMAND_1, LOW);
  digitalWrite(BACK_DOOR_COMMAND_2, LOW);

  pinMode(FRONT_DOORS_OPENED_LED, OUTPUT);
  pinMode(REAR_DOORS_OPENED_LED, OUTPUT);
  pinMode(BACK_DOOR_OPENED_LED, OUTPUT);

  digitalWrite(FRONT_DOORS_OPENED_LED, LOW);
  digitalWrite(REAR_DOORS_OPENED_LED, LOW);
  digitalWrite(BACK_DOOR_OPENED_LED, LOW);

  Serial.begin(19200);
  inputMessageString.reserve(1024);
  Serial.println("CAR LIGHTS START");
}

void loop() {
  if (messageComplete) {
    String message = String(inputMessageString);
    parseInputMessage(message);
    
    inputMessageString = "";
    messageComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputMessageString+=inChar;
    if (inChar == END_OF_MSG_CHAR) {
      inputMessageString+=inChar;
      messageComplete = true;
    }
  }
}

void parseInputMessage(String message) {
    debugPrint("Received : '");
    debugPrint(inputMessageString);
    debugPrintln("'");
    
    if (!message.startsWith(MSG_PREFIX)) {
      Serial.println("Unknown message '" + message + "'");
      return;
    }

    String frontDoorsStateStr = getValue(message, FRONT_DOORS_STATE_MSG);
    String rearDoorsStateStr = getValue(message, REAR_DOORS_STATE_MSG);
    String backDoorStateStr = getValue(message, BACK_DOOR_STATE_MSG);

    debugPrint("READ FRONT DOORS STATE : ");
    debugPrintln(frontDoorsStateStr);
    
    debugPrint("READ REAR DOORS STATE : ");
    debugPrintln(rearDoorsStateStr);
    
    debugPrint("READ BACK DOOR STATE : ");
    debugPrintln(backDoorStateStr);

    if (DOOR_OPENED_MSG.equals(frontDoorsStateStr)) {
      digitalWrite(FRONT_DOORS_OPENED_LED, HIGH);
    } else if (DOOR_CLOSED_MSG.equals(frontDoorsStateStr)) {
      digitalWrite(FRONT_DOORS_OPENED_LED, LOW);
    }
    
    if (DOOR_OPENED_MSG.equals(rearDoorsStateStr)) {
      digitalWrite(REAR_DOORS_OPENED_LED, HIGH);
    } else if (DOOR_CLOSED_MSG.equals(rearDoorsStateStr)) {
      digitalWrite(REAR_DOORS_OPENED_LED, LOW);
    }
    
    if (DOOR_OPENED_MSG.equals(backDoorStateStr)) {
      digitalWrite(BACK_DOOR_OPENED_LED, HIGH);
    } else if (DOOR_CLOSED_MSG.equals(backDoorStateStr)) {
      digitalWrite(BACK_DOOR_OPENED_LED, LOW);
    }

    String frontLightStateStr = getValue(message, FRONT_LIGHT_STATE_MSG);
    String backLightStateStr = getValue(message, BACK_LIGHT_STATE_MSG);

    debugPrint("READ FRONT LIGHT STATE : ");
    debugPrintln(frontLightStateStr);
    
    debugPrint("READ REAR LIGHT STATE : ");
    debugPrintln(backLightStateStr);

    if (ON_MSG.equals(frontLightStateStr)) {
      digitalWrite(FRONT_LIGHT_OUTPUT, HIGH);
    } else if (OFF_MSG.equals(frontLightStateStr)) {
      digitalWrite(FRONT_LIGHT_OUTPUT, LOW);
    }

    if (ON_MSG.equals(backLightStateStr)) {
      digitalWrite(BACK_LIGHT_OUTPUT, HIGH);
    } else if (OFF_MSG.equals(backLightStateStr)) {
      digitalWrite(BACK_LIGHT_OUTPUT, LOW);
    }

}

String getValue(String messageToParse, String itemName) {
  int itemPosition = messageToParse.indexOf(itemName);
  if (itemPosition < 0) {
    return PARSE_ERROR;
  }
  
  int endOfItemValuePosition = messageToParse.indexOf(SEPARATOR, itemPosition);
  if (endOfItemValuePosition < 0) {
    endOfItemValuePosition = messageToParse.indexOf(END_OF_MSG_CHAR, itemPosition);
  }
  
  if (endOfItemValuePosition < 0) {
    return PARSE_ERROR;
  }

  return messageToParse.substring(itemPosition + itemName.length() + 1, endOfItemValuePosition);
}

void debugPrint(String message) {
  if (debug) {
    Serial.print("[DEBUG] : ");
    Serial.print(message);
  }
}

void debugPrintln(String message) {
  if (debug) {
    Serial.println(message);
  }
}
