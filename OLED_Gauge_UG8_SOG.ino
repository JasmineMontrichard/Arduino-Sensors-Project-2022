/*
SOG GAUGE

 Analog gauge v1.02
 - Shows half circle gauge with scale labels
 - Shows digital value, 00 - 100 aligned
 - Shows gauge label
 21/12/2015 - Rudi Imbrechts
 http://arduinows.blogspot.com
*/

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include "U8glib.h"


// Init hardware
Adafruit_GPS GPS(&Serial1);                      // Hardware Serial using Rx0
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Res = 12

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO false


                                                // For OLED display with SH1106 driver. If you use another display,

                                                // then please check the u8glib documentation and website at

                                                // https://github.com/olikraus/u8glib



int xmax=128;                                   // max length x-axis

int ymax=62;                                    // max length y-axis

int xcenter=xmax/2;                             // center of x-axis

int ycenter=ymax/2+10;                          // center of y-axis

int arc=ymax/2;                           

int angle=0;                                 

char* label[] = {"WFAS","COOLANT"," INTAKE","VOLT"};    // some custom gauge labels

int labelXpos[] = {53, 45, 49, 53};                     // predefined x-position of a gauge label

#define potmeterPin A1                                  // simulate analogue value with potentiometer

int p, w, m,a=10;

u8g_uint_t xx=0;



// ------------------------------------------------- void gauge() ------------------------------------------

void gauge(uint8_t angle) {



  // draw border of the gauge

  u8g.drawCircle(xcenter,ycenter,arc+6, U8G_DRAW_UPPER_RIGHT);

  u8g.drawCircle(xcenter,ycenter,arc+4, U8G_DRAW_UPPER_RIGHT);

  u8g.drawCircle(xcenter,ycenter,arc+6, U8G_DRAW_UPPER_LEFT);

  u8g.drawCircle(xcenter,ycenter,arc+4, U8G_DRAW_UPPER_LEFT);



  // draw the needle

  float x1=sin(2*angle*2*3.14/360);              // needle position

  float y1=cos(2*angle*2*3.14/360);

  u8g.drawLine(xcenter, ycenter, xcenter+arc*x1, ycenter-arc*y1);

  u8g.drawDisc(xcenter, ycenter, 5, U8G_DRAW_UPPER_LEFT);

  u8g.drawDisc(xcenter, ycenter, 5, U8G_DRAW_UPPER_RIGHT);

  u8g.setFont(u8g_font_chikita);



  // show scale labels

  u8g.drawStr( 20, 42, "0");                 

  u8g.drawStr( 25, 18, "2.5");

  u8g.drawStr( 60, 14, "5");

  u8g.drawStr( 95, 18, "7.5");

  u8g.drawStr( 105, 42, "10");



  // show gauge label

  u8g.setPrintPos(labelXpos[3],32);         // Change position of Label

  u8g.print(label[0]);                     // 0-3 Change the label name (check line#37)



  // show digital value and align its position

  u8g.setFont(u8g_font_profont22);
  
  u8g.setPrintPos(3,60);                        // Mod 
  u8g.print("SOG");                             // Mod           

  u8g.setPrintPos(54,60);

  if (w<10){                                    // leading 0 when value less than 10

    u8g.print("0");

  }

  if (w>99) {                                   // position at 100%

    u8g.setPrintPos(47,60);

  }

  u8g.print(w);
  u8g.setPrintPos(90,60);                       // Mod 
  u8g.print("Kts");                             // Mod

}

// ------------------------------------------------- void setup() ------------------------------------------

void setup(void) {
  Serial.begin(9600);
  GPS.begin(9600);
    

  u8g.setFont(u8g_font_chikita);

  u8g.setColorIndex(1);                         // Instructs the display to draw with a pixel on.



  // assign default color value

  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {

    u8g.setColorIndex(255);                     // white

  }

 /* else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {

    u8g.setColorIndex(3);                       // max intensity

  }

  else if ( u8g.getMode() == U8G_MODE_BW ) {

    u8g.setColorIndex(1);                       // pixel on

  }

  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {

    u8g.setHiColorByRGB(255,255,255);

  }*/
    
u8g.firstPage();  
  do {
  u8g.setFont(u8g_font_helvR08);
  u8g.drawStr(55, 22, "SOG");
  u8g.drawStr(53, 42, "WFAS");
  u8g.drawStr(35, 60, "MARINE ONE");
  } while( u8g.nextPage() );
  delay(2000);                // Wait 2 seconds

}

uint32_t timer = millis();

// ------------------------------------------------- void loop() ------------------------------------------


void loop(void)
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

  }

   p+=a;     



   if( p > 1023) a= -10;

else if( p < 0 ) a = 10;



 w = map(p,0,1023,0,10);                 // map it between 0 and 10

 m = map(p,0,1023,0,90);                // map needle movement  
    
 

    
    
  // show needle and dial

  xx = m;                               // 135 = zero position, 180 = just before middle, 0 = middle, 45 = max

  if (xx<45){                          // positie correctie

    xx=xx+135;

  }

  else {

    xx=xx-45;

  }



  // picture loop

  {

    u8g.firstPage(); 

    do {             

      gauge(xx);

    }

    while( u8g.nextPage() );

  }

}      
