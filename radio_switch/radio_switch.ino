const int radioAntennaPin = 7;
const int blackPin =  8;
const int redPin =  9;

int switchState = 1;

void setup() {
  pinMode(radioAntennaPin, INPUT);      
  pinMode(blackPin, OUTPUT);     
  pinMode(redPin, OUTPUT);     

// Turn on pull up resistors
   digitalWrite(radioAntennaPin, HIGH);

   Serial.begin(9600);
   Serial.println("AUTO RADIO SWITCH");
}

void loop(){
	int radioAntennaState = digitalRead(radioAntennaPin);
	if (radioAntennaState == LOW) {
    if (switchState == 1) {
      switchState = 2;
      digitalWrite(blackPin, HIGH);
      digitalWrite(redPin, HIGH);   
      Serial.println("Switch mode 2");
    }
	} else if (switchState == 2) {
		switchState = 1;
		digitalWrite(blackPin, LOW);
		digitalWrite(redPin, LOW);		
    Serial.println("Switch mode 1");
	}
  delay(100);
}
