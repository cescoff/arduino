#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


// OLED display TWI address
#define OLED_ADDR   0x3C
#define SSD1306_128_64

// reset pin not used on 4-pin OLED module
Adafruit_SSD1306 display(-1);  // -1 = no reset pin

// 128 x 64 pixel display
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#define ONE_WIRE_BUS_EXT 5
#define ONE_WIRE_BUS_MOT 5

static const unsigned char PROGMEM cloud_logo[] =
{ 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x1f, 0x80, 0x3f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfe, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfe, 0x3f, 0xfc, 0x09, 0x24, 0x12, 0x48, 0x24, 0x90 };

// 'Lion_Peugeot_1980', 64x32px
const unsigned char Peugeot_Logo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x80, 0x30, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x38, 0x00, 0x18, 0x00, 0x00, 
  0x00, 0x38, 0x60, 0x30, 0x00, 0x0d, 0xf0, 0x00, 0x00, 0x38, 0x38, 0x1f, 0xe0, 0x0f, 0x70, 0x00, 
  0x00, 0x38, 0x0c, 0x06, 0x70, 0x0f, 0x70, 0x00, 0x00, 0x1e, 0x03, 0x0f, 0x70, 0x07, 0x30, 0x00, 
  0x00, 0x07, 0xf9, 0xcf, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x78, 0x00, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x20, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x0d, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0xc0, 0x00, 0x1c, 0xc0, 0x00, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x18, 0x60, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x30, 0x60, 0x00, 0x00, 0x30, 0x3f, 0xfc, 0x00, 0xe0, 0x30, 0x00, 
  0x00, 0x3f, 0x30, 0x07, 0x00, 0x70, 0x38, 0x00, 0x00, 0x18, 0xe0, 0x03, 0x00, 0x18, 0x38, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x07, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x18, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x3e, 0x01, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x60, 0x63, 0x01, 0x80, 0x00, 0x00, 0x00, 0xfc, 0x20, 0xc1, 0x80, 0xf0, 0x00, 
  0x00, 0x00, 0xc3, 0x63, 0x80, 0xe0, 0x30, 0x00, 0x00, 0x00, 0xc1, 0x86, 0x00, 0x3c, 0x30, 0x00, 
  0x00, 0x00, 0xc0, 0x0c, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0xfe, 0x30, 0x00, 
  0x00, 0x00, 0x03, 0x00, 0x01, 0x80, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x30, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00
};

// 'SnowFlake', 64x32px
static const unsigned char Snowflake [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7, 0xef, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x7f, 0xfe, 0x0f, 0x80, 0x00, 
  0x00, 0x31, 0xf8, 0x1f, 0xf8, 0x1f, 0x8c, 0x00, 0x00, 0x7e, 0xf8, 0x0f, 0xf0, 0x1f, 0x7e, 0x00, 
  0x00, 0xff, 0xfc, 0x07, 0xe0, 0x3f, 0xff, 0x00, 0x00, 0x1f, 0xfe, 0x07, 0xe0, 0x7f, 0xf8, 0x00, 
  0x00, 0x1f, 0xff, 0x87, 0xe1, 0xff, 0xf8, 0x00, 0x00, 0x7f, 0xff, 0xf7, 0xef, 0xff, 0xfe, 0x00, 
  0x00, 0x3f, 0xc7, 0xff, 0xff, 0xe3, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0xff, 0xff, 0xe1, 0xfc, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xc7, 0xe3, 0xff, 0xfc, 0x00, 
  0x00, 0x1f, 0xfe, 0x07, 0xe0, 0x7f, 0xf8, 0x00, 0x00, 0xff, 0xfc, 0x07, 0xe0, 0x3f, 0xff, 0x00, 
  0x00, 0x7e, 0xfc, 0x07, 0xe0, 0x1f, 0x7e, 0x00, 0x00, 0x30, 0xf8, 0x1f, 0xf8, 0x1f, 0x0c, 0x00, 
  0x00, 0x01, 0xf0, 0x7f, 0xfe, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf7, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe2, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


OneWire oneWire1(ONE_WIRE_BUS_EXT);
OneWire oneWire2(ONE_WIRE_BUS_MOT);

DallasTemperature extSensors(&oneWire1);
DallasTemperature motSensors(&oneWire2);

 float ExtCelcius=0;
 float MotCelcius=0;
 unsigned long lastIceAlertTime=0;
 
 const unsigned long alertThreshold=1*60000;
 const float iceAlertValueThreshold = 23;

void setup(void)
{
  
  Serial.begin(9600);
  extSensors.begin();
  motSensors.begin();

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.drawBitmap(32, 0, Peugeot_Logo, 64, 32, WHITE);
  display.display();
  delay(20000);
  display.clearDisplay();
}

void loop(void)
{ 
  extSensors.requestTemperatures(); 
  ExtCelcius=extSensors.getTempCByIndex(0);
  motSensors.requestTemperatures(); 
  MotCelcius=motSensors.getTempCByIndex(0);
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
      displayIceAlert(10, ExtCelcius);
      lastIceAlertTime=millis();
    }
  }
  
  displayExtAndMot(ExtCelcius, MotCelcius);

  delay(1000);
}

void displayExtAndMot(float extCel, float motCel) {
  display.clearDisplay();


  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Ext: ");
  display.println(printFloat(extCel));
  

  display.print("Mot: ");
  display.print(printFloat(motCel));

  display.display();

}

void displayIceAlert(int repeat, float extCel) {
  for (int index = 0; index < repeat; index++) {
    display.clearDisplay();
    display.drawBitmap(32, 0, Snowflake, 64, 32, WHITE);
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(printFloat(extCel));
    display.display();
    delay(1000);
    display.clearDisplay();
  }
}


String printFloat(float floatvalue) {
  char str[4];
  dtostrf(floatvalue, 3, 1, str );
  return str;
}