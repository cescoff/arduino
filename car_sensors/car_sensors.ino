#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 24, 4);

#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

 float ExtCelcius=0;
 float MotCelcius=0;
 unsigned long lastIceAlertTime=0;
 
 const unsigned long alertThreshold=4*60000;
 const float iceAlertValueThreshold = 4;

void setup(void)
{
  
  Serial.begin(9600);
  sensors.begin();

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("PEUGEOT 305 GT");

  for (int index = 0; index < 20; index++) {
    lcd.scrollDisplayRight();
    delay(1000);
  }
  lcd.clear();
}

void loop(void)
{ 
  sensors.requestTemperatures(); 
  ExtCelcius=sensors.getTempCByIndex(0);
  Serial.print(" C  ");
  Serial.println(ExtCelcius);

  if (ExtCelcius <= iceAlertValueThreshold || MotCelcius <= iceAlertValueThreshold) {
    unsigned long currentTime=millis();
    unsigned long lastAlertDelta = currentTime - lastIceAlertTime;
    Serial.print("Current Time: ");
    Serial.println(currentTime);
    Serial.print("Last Ice Alert Time: ");
    Serial.println(lastIceAlertTime);
    Serial.print("Last Ice Alert Delta: ");
    Serial.print(lastAlertDelta);
    Serial.print(" <-> ");
    Serial.println(alertThreshold);
    if (lastIceAlertTime == 0 || lastAlertDelta > alertThreshold) {
      displayIceAlert(4, ExtCelcius);
      lastIceAlertTime=millis();
    }
  }
  
  displayExtAndMot(ExtCelcius, MotCelcius);

  delay(1000);
}

void displayExtAndMot(float extCel, float motCel) {
  lcd.setCursor(0,0);
  lcd.print("Ext: ");
  lcd.setCursor(5,0);
  lcd.print(extCel);
  if (extCel <= 10 && extCel >= 0) {
    lcd.setCursor(9,0);
  } else {
    lcd.setCursor(10,0);
  }
  lcd.print(" C");
  
  lcd.setCursor(0,1);
  lcd.print("Mot: ");
  lcd.setCursor(5,1);
  lcd.print(motCel);
  if (motCel <= 10 && motCel >= 0) {
    lcd.setCursor(9,1);
  } else {
    lcd.setCursor(10,1);
  }
  lcd.print(" C");
}

void displayIceAlert(int repeat, float extCel) {
  lcd.clear();
  for (int index = 0; index < repeat; index++) {
    lcd.setCursor(1,0);
    lcd.print("***   ICE   ***");
    lcd.setCursor(1,1);
    lcd.print("***  ");
    lcd.setCursor(6,1);
    lcd.print(extCel);
    lcd.setCursor(10,1);
    if (extCel > 10 || extCel < 0) {
      lcd.print(" C ***");
    } else if (extCel >= 0 && extCel < 10) {
      lcd.print(" C  ***");
    }
    delay(1500);
    lcd.clear();
    lcd.noBacklight(); // turn off backlight
    delay(500);
    lcd.backlight();
  }
  lcd.clear();
}
