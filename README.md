# Arduino-Boat-Project
###### This project is discontinued.
### Background
I did this project alongside my dad. I was responsible for the software part of it, while he was responsible for the hardware. This was one of my other personal projects after taking my first Java (Dual Credit) course, which was meant for displaying wind speed, ground speed, etc. while he would be on the lake.

### My Experience
* I experimented with the [u8glib](https://github.com/olikraus/u8glib) and [u8g2](https://github.com/olikraus/u8g2) libraries created by [olikraus](https://github.com/olikraus).
  + To get familiar with the software side of Arduino, I created a program that would display a status for each sensor, which would display a solid circle if it was *activated*, or would  display an outlined circle if it was *not activated*.
  + I also created a UI that displayed wind speed, ground speed, speed on ground (SOD) using sensors my dad hooked up to the breadboard.
* I used code from what my dad could find as a template off of the internet, and I simply tweaked it so that it fits our project's expectations and needs.

In the end, I learned that there are other factors to look at; not just the software side of it. I spent a few hours with my dad per day during my freshman year of high school in troubleshooting and making sure our version of each program worked. In addition, I also learned that documentation is important and I should've added it near the end of this project.

### Notes
* The programs I created in whole are: [BoatProject_Option1](https://github.com/JasmineMontrichard/Arduino-Sensors-Project-2022/blob/main/BoatProject_Option1.ino) and [BoatProject_Option2](https://github.com/JasmineMontrichard/Arduino-Sensors-Project-2022/blob/main/BoatProject_Option2.ino)
* No I don't know C++, but Java is *very* similar
* My setup using the u8glib lib: `U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);`
* Library for sensor chip my dad used: `Adafruit_AHT10.h` 
