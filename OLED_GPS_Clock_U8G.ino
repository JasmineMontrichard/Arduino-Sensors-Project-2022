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


#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <U8glib.h>

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR  true

// Offset the hours from UTC (universal time) to your local time by changing
// this value.
#define HOUR_OFFSET   -5

#define OLED_RESET    -1

// Init hardware
Adafruit_GPS GPS(&Serial1);                      // Hardware Serial using Rx0
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send ACK

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO true


void setup()  
{
// connect at 9600 so we can read the GPS fast enough and echo without dropping chars
// also spit it out
  Serial.begin(9600);
// 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  u8g.firstPage();  
  do {
  u8g.setFont(u8g_font_helvR08);
  u8g.drawStr(35, 22, "GPS CLOCK");
  u8g.drawStr(53, 42, "WFAS");
  u8g.drawStr(35, 60, "MARINE ONE");
  } while( u8g.nextPage() );
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
  if (millis() - timer > 1000) { 
    timer = millis();           // reset the timer

  if (GPSECHO)
    Serial.println (GPS.seconds, DEC);  

// Picture loop    
    u8g.firstPage();  
    do {
      displaytime();
      displaydate();
      displaylocation();
      displaysats();
      displayQRA();
      displaySOG();
    } while( u8g.nextPage() );
  }

}

/******************************************************************
 *   D I S P L A Y
 *   
 *   Displays Time, Date and Location data
 *   Time corrected with Timezone offset and 12/24 hour clock
 ******************************************************************/
void displaytime()
{
  u8g.setFont(u8g_font_gdr20);
  u8g.setPrintPos(10,47);

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

  if (hours < 10) u8g.print ("0");
  u8g.print (hours, DEC);
  u8g.print (":");
  if (GPS.minute < 10) u8g.print ("0");
  u8g.print (GPS.minute, DEC);
  u8g.print (":");
  if (GPS.seconds < 10) u8g.print ("0");
  u8g.print (GPS.seconds, DEC);
  u8g.print (" "); 
}

void displaydate()
{
//  if (GPS.satellites > 3) return;
  u8g.setFont(u8g_font_helvR08);
  u8g.setPrintPos(2,64);
  u8g.print(GPS.day, DEC); u8g.print('-');
  u8g.print(GPS.month, DEC); u8g.print("-20");
  u8g.print(GPS.year, DEC);
}

void displaylocation()
{
//  if (GPS.satellites > 3) return;
  u8g.setFont(u8g_font_helvR08);
  u8g.setPrintPos(2,10);
  u8g.print(GPS.latitudeDegrees, 4); u8g.print(GPS.lat);
  u8g.setPrintPos(2,20);
  u8g.print(GPS.longitudeDegrees, 4); u8g.print(GPS.lon);
      
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

  u8g.setFont(u8g_font_helvR08);
  u8g.setPrintPos(83,10);  
  // First Character - longitude based (every 20° = 1 gridsq)
  u8g.print(char_string[int(loclong / 20000000)]);

  // Second Character - latitude based (every 10° = 1 gridsq)
  u8g.print(char_string[int(loclat / 10000000)]);

  // Third Character - longitude based (every 2° = 1 gridsq)
  scrap = loclong - (20000000 * int(loclong / 20000000));
  u8g.print(num_string[int(scrap * 10 / 20 / 1000000)]);

  // Fourth Character - latitude based (every 1° = 1 gridsq)
  scrap = loclat - (10000000 * int(loclat / 10000000));
  u8g.print(num_string[int(scrap / 1000000)]);

  // Fifth Character - longitude based (every 5' = 1 gridsq)
  scrap = (loclong / 2000000) - (int(loclong / 2000000));
  u8g.print(char_string[int(scrap * 24)]);

  // Sixth Character - longitude based (every 2.5' = 1 gridsq)
  scrap = (loclat / 1000000) - (int(loclat / 1000000));
  u8g.print(char_string[int(scrap * 24)]);  
}  

void displaysats()
{
  u8g.setFont(u8g_font_helvR08);
  u8g.setPrintPos(85,64);  
  if (GPS.satellites >=0 && GPS.satellites <20)  {
    u8g.print("Sats: "); u8g.print((int)GPS.satellites);
  }  
}

void displaySOG() {
  u8g.setPrintPos(67, 20);
  u8g.print("SOG: "); u8g.print(GPS.speed, 1); u8g.println(" kts");
}
