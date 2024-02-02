#include <Adafruit_AHT10.h> // Sensor Chip
#include <U8glib.h> // Preferred Library

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);
Adafruit_AHT10 aht;
sensors_event_t humidity, temp;

/**
* Documentation & Notes
* Author: Jasmine Montrichard
* Comments: I DID THIS AT 14???????? JWQIJEOQJWEADL AND THIS IS MY FIRST TIME TOO??? AND I GOT PAID $50???
* ^^ With some help, obviously...
* 
* Notes:
* 
* S1 - A0 - Digital
* S2 - A1 - Digital
* S3 - A2 - Digital
* S4 - A3 - Digital
* S5 - A5 - Analog
* S6 - A6 - Analog
 */

// Analog Pins
int pinA6 = A6;
int readValA6;
int V1 = 0;

int pinA7 = A7;
int readValA7;
int V2 = 0;

int analogToDigital(int arrInfo[]) {
  // pin, readVal, finalVal
  // 0,   1,       2,
  arrInfo[1] = analogRead(arrInfo[0]);
  arrInfo[2] = (5./1023.)*arrInfo[1];
  return arrInfo[2];
}

void systemDraw_analogRead(int arrInfo[], int pin, int d1, int d2, int d3, int p1, int p2, String s) {
  int finalVal = analogToDigital(arrInfo);
  
  if (finalVal >= 3) {
    u8g.drawDisc(d1, d2, d3);
    u8g.setPrintPos(p1, p2);
    u8g.print(s);
  }
}

void systemDraw_digitalRead(int pinStatus, int d1, int d2, int d3, int p1, int p2, String s) {
  if (pinStatus == HIGH) {
    u8g.drawDisc(d1, d2, d3);
    u8g.setPrintPos(p1, p2);
    u8g.print(s);
  }
}

void u8g_setup() {
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  u8g.setFont(u8g_font_6x10);

  // Sensors
  u8g.setPrintPos(2, 10);
  u8g.print("AirTemp: "); u8g.print(temp.temperature, 0); u8g.print(char(176)); u8g.print("C");
  u8g.setPrintPos(2, 20);
  u8g.print("Humidity: "); u8g.print(humidity.relative_humidity, 0); u8g.print("%");
  u8g.setPrintPos(2, 30);
  u8g.print("WaterTemp: "); u8g.print("XX"); u8g.print(char(176)); u8g.print("C");

  // Indicators
  int digitalPins[] = { digitalRead(A0), digitalRead(A1), digitalRead(A2), digitalRead(A3) };

  /* pinStatus array, 1st draw, 2nd draw, 3rd draw, 1st printPos, 2nd printPos, system num */
  systemDraw_digitalRead(digitalPins[0], 8, 40, 2, 16, 44, "S1");
  systemDraw_digitalRead(digitalPins[1], 63, 40, 2, 70, 44, "S2");
  systemDraw_digitalRead(digitalPins[2], 8, 50, 2, 16, 54, "S3");
  systemDraw_digitalRead(digitalPins[3], 63, 50, 2, 70, 54, "S4");
  
  /* pin, readVal, finalVal */
  int A6Info[] = { pinA6, readValA6, V1 };
  int A7Info[] = { pinA7, readValA7, V2 }; 

  systemDraw_analogRead(A6Info, A6Info[0], 8, 60, 2, 16, 64, "S5");
  systemDraw_analogRead(A7Info, A7Info[0], 63, 60, 2, 70, 64, "S6");
}

void setup() {
  pinMode(readValA6, INPUT); 
  pinMode(readValA7, INPUT);
  Serial.begin(115200);
  
  Serial.println("Starting program...");

  if (!aht.begin()) {
    Serial.println("Could not find AHT10");
    while (1) delay(10); // Sleep mode
  }
  Serial.println("AHT10 found");
}

void loop() {
  u8g.firstPage();  
  do {
    u8g_setup();
  } while( u8g.nextPage() );
}
