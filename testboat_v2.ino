// GPS_Display
// This program is based on the Test code for Adafruit GPS modules using MTK3329/MTK3339 driver.
// Also using the graphic labrary made by Oliver Kraus.
// Modified for using with an OLED Display and added QRA locator information.
// The program displays LOCAL and 12/24 hour time 
//
// Hardware:
// - Arduino Nano with 64 x 128 OLED Display (I2C)
// - NEO-6M-0-001 chipset
//
// Connections between the Nano and the OLED and GPS module:
// - I2C OLED: A4-SDA, A5-SCL, 3.3Volt-Vcc
// - GPS: 3.3Volt-Vcc, RX0-TX.  !!!>>  Disconnect during programming <<!!!
//
// Modifications:
// - 26-02-2016   V.1.0   First release
// - 01-11-2016   V.1.1   Time display in LOCAL modified in original Adafruit_GPS.cpp
// - 17-04-2021   V.2.0   Added Timezone and 12/24 hour setup. Get it from clock_sevenseg_gps.ino


#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR  true

// Offset the hours from UTC (universal time) to your local time by changing
// this value.
#define HOUR_OFFSET   -5

#define OLED_RESET    -1

// Init hardware
Adafruit_GPS GPS(&Serial1);                      // Hardware Serial using Rx0
U8G2_SH1106_128X64_NONAME_F_HW_I2C display1(U8G2_R0, 5,4,U8X8_PIN_NONE);  // Display which does not send ACK
U8G2_SH1106_128X64_NONAME_F_HW_I2C display2(U8G2_R0, 5,4,U8X8_PIN_NONE);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO true


void setup()  
{
  display1.setI2CAddress(0x78);
  display1.begin();
  display2.setI2CAddress(0x7A);
  display2.begin();
// connect at 9600 so we can read the GPS fast enough and echo without dropping chars
// also spit it out
  Serial.begin(9600);
// 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  display1.firstPage();  
  do {
  display1.setFont(u8g2_font_helvR08_tr);
  display1.drawStr(35, 22, "GPS CLOCK");
  display1.drawStr(53, 42, "WFAS");
  display1.drawStr(35, 60, "MARINE ONE");
  } while( display1.nextPage() );
  delay(0);                // Wait 10 seconds / 10000 ms

}

uint32_t timer = millis();

//************************** L O O P  **************************************************
void loop()                     // run over and over again
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) return;
  }  

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 1 seconds or so, display the current stats
  if (millis() - timer > 1000)
    timer = millis();           // reset the timer

  if (GPSECHO)
    Serial.println (GPS.seconds, DEC);  

// Picture loop
do {
  display1.setI2CAddress(0x78);
      displaytime();
      displaydate();
      displaylocation();
      displaysats();
      displayQRA();
      displaySOG();
      display1.sendBuffer();
      delay(500);
     display2.setI2CAddress(0x7A);
      display2.print("Hello!");
      display2.sendBuffer();
      display1.clearBuffer();
      display2.clearBuffer();
  } while (true);

}

/******************************************************************
 *   D I S P L A Y
 *   
 *   Displays Time, Date and Location data
 *   Time corrected with Timezone offset and 12/24 hour clock
 ******************************************************************/

void displaytime()
{
  display1.setFont(u8g2_font_ncenR18_tf);
  display1.setCursor(10,47);

// Do Time Zone offset
  int hours = GPS.hour + HOUR_OFFSET;    // Add hour offset to convert from UTC
                                        // to local time.
  // Handle when UTC + offset wraps around to a negative or > 23 value.
  if (GPS.hour < 0) {
    hours = 24 + GPS.hour;
  }
  if (GPS.hour > 23) {
    hours = 24 - GPS.hour;
  }

// Do 24 hour to 12 hour format conversion when required.
  if (!TIME_24_HOUR) {
    // Handle when hours are past 12 by subtracting 12 hours (1200 value).
    if (GPS.hour > 12) {
      hours -= 12;
    }
    // Handle hour 0 (midnight) being shown as 12.
    else if (GPS.hour == 0) {
      hours += 12;
    }
  }

  if (hours < 10) display1.print ("0");
  display1.print(hours, DEC);
  display1.print(":");
  if (GPS.minute < 10) display1.print("0");
  display1.print(GPS.minute, DEC);
  display1.print(":");
  if (GPS.seconds < 10) display1.print("0");
  display1.print(GPS.seconds, DEC);
  display1.print(" "); 
}

void displaydate()
{
//  if (GPS.satellites > 3) return;
  display1.setFont(u8g2_font_helvR08_tr);
  display1.setCursor(2,64);
  display1.print(GPS.day, DEC); display1.print('-');
  display1.print(GPS.month, DEC); display1.print("-20");
  display1.print(GPS.year, DEC);
}

void displaylocation()
{
//  if (GPS.satellites > 3) return;
  display1.setFont(u8g2_font_helvR08_tr);
  display1.setCursor(2,10);
  display1.print(GPS.latitudeDegrees, 4); display1.print(GPS.lat);
  display1.setCursor(2,20);
  display1.print(GPS.longitudeDegrees, 4); display1.print(GPS.lon);
      
}

void displayQRA()
{
//  if (GPS.satellites > 3) return;

  float loclong = GPS.longitudeDegrees * 1000000;
  float loclat  = GPS.latitudeDegrees * 1000000;
  float scrap;
  char char_string [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char num_string[]= "0123456789";

  if (GPS.lon == 'E') 
  {
    loclong = (loclong) + 180000000;
  }
  if (GPS.lon == 'W') 
  {
    loclong = 180000000 - (loclong);
  }

  if (GPS.lat == 'N') 
  {
    loclat = loclat + 90000000;
  }
  if (GPS.lat == 'S') 
  {
    loclat = 90000000 - loclat;
  }

  display1.setFont(u8g2_font_helvR08_tr);
  display1.setCursor(83,10);  
  // First Character - longitude based (every 20° = 1 gridsq)
  display1.print(char_string[int(loclong / 20000000)]);

  // Second Character - latitude based (every 10° = 1 gridsq)
  display1.print(char_string[int(loclat / 10000000)]);

  // Third Character - longitude based (every 2° = 1 gridsq)
  scrap = loclong - (20000000 * int(loclong / 20000000));
  display1.print(num_string[int(scrap * 10 / 20 / 1000000)]);

  // Fourth Character - latitude based (every 1° = 1 gridsq)
  scrap = loclat - (10000000 * int(loclat / 10000000));
  display1.print(num_string[int(scrap / 1000000)]);

  // Fifth Character - longitude based (every 5' = 1 gridsq)
  scrap = (loclong / 2000000) - (int(loclong / 2000000));
  display1.print(char_string[int(scrap * 24)]);

  // Sixth Character - longitude based (every 2.5' = 1 gridsq)
  scrap = (loclat / 1000000) - (int(loclat / 1000000));
  display1.print(char_string[int(scrap * 24)]);  
}  

void displaysats()
{
  display1.setFont(u8g2_font_helvR08_tr);
  display1.setCursor(85,64);  
  if (GPS.satellites >=0 && GPS.satellites <20)  {
    display1.print("Sats: "); display1.print((int)GPS.satellites);
  }  
}

void displaySOG() {
  display1.setCursor(67, 20);
  display1.print("SOG: "); display1.print(GPS.speed, 1); display1.println(" kts");
}
