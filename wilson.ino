#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "Wire.h" // This library allows you to communicate with I2C devices.
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
char tmp_str[7]; // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}
long randNumber;


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static unsigned char PROGMEM const logo16_glcd_bmp[] =
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfe, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xee, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x87, 0x9f,
  0xff, 0xff, 0xff, 0xfc, 0xc1, 0xe7, 0xff, 0xff, 0xff, 0xfe, 0x28, 0x01, 0xf7, 0xff, 0xff, 0xff,
  0x80, 0x00, 0x3e, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x0f, 0x7f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0xcf,
  0xff, 0xff, 0xfe, 0x00, 0x41, 0x1b, 0xff, 0xff, 0xff, 0x80, 0x12, 0xe2, 0xff, 0xff, 0xff, 0xe0,
  0x04, 0xd0, 0xff, 0xff, 0xff, 0xf8, 0x01, 0x30, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x68, 0x03, 0xff,
  0xff, 0xff, 0x80, 0x87, 0x23, 0xff, 0xff, 0xff, 0xf0, 0xf6, 0xdc, 0xff, 0xff, 0xff, 0xfc, 0x7c,
  0x1f, 0xbf, 0xff, 0xff, 0xff, 0x1f, 0x01, 0x3f, 0xff, 0xff, 0xff, 0xc6, 0x20, 0x66, 0xff, 0xff,
  0xff, 0xf1, 0x98, 0x1f, 0xbf, 0xff, 0xff, 0xf8, 0x66, 0x07, 0xef, 0xff, 0xff, 0xfe, 0x1f, 0x88,
  0xf9, 0xff, 0xff, 0xff, 0x83, 0xf0, 0x3c, 0x7f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xff,
  0xf8, 0x00, 0x60, 0x03, 0xff, 0xff, 0xfe, 0x00, 0x38, 0x00, 0xff, 0xff, 0xff, 0x00, 0x0e, 0x00,
  0x3f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfc,
  0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xc0, 0x00, 0x08, 0x1f,
  0xff, 0xff, 0xf0, 0x00, 0xff, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0x9f, 0x83, 0xff, 0xff, 0xff, 0x80,
  0xff, 0xc0, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xff,
  0xff, 0xff, 0xe0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0x00,
  0x03, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);


  Serial.begin(9600);
  delay(200);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // miniature bitmap display
  display.clearDisplay();
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(2000);


  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.setTextColor(WHITE);
  display.println (F(" Turn me over to get"));
  display.println (F("      started"));
  display.display();

}

void loop() {

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

  randomSeed(analogRead(0)); // nothing connected to 0 so read sees noise

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read() << 8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  if (accelerometer_z < 5000) {
    randNumber = random(1, 26); // generate random number between 1 & 5
    riddler();
    delay(2000);
  }
}

void riddler() {
  if (randNumber == 1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println (F("What is your favorite animal and why?"));
    display.display();
  }

  if (randNumber == 2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is your favorite cereal?"));
    display.display();
  }

  if (randNumber == 3) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Do you play any musical instruments? If not what would you like to play?"));
    display.display();

  } if (randNumber == 4) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What was your worst summer job?"));
    display.display();

  } if (randNumber == 5) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("If you could travel anywhere in the world where would it be and why?"));
    display.display();

  } if (randNumber == 6) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What was your favorite cartoon growing up?"));
    display.display();

  } if (randNumber == 7) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Name something you have done that you think no one else has."));
    display.display();

  } if (randNumber == 8) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Do you like Spring, Summer, Fall, or Winter and why?"));
    display.display();

  } if (randNumber == 9) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("If you could meet anyone dead or alive who would it be?"));
    display.display();

  } if (randNumber == 10) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is the weirdest food you have ever tried?"));
    display.display();

  } if (randNumber == 11) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What was your favorite toy growing up?"));
    display.display();

  } if (randNumber == 12) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Who is your favorite fictional character?"));
    display.display();

  } if (randNumber == 13) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Do you have any hidden talents?"));
    display.display();

  } if (randNumber == 14) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("If you could be invisible for a day where would you go?"));
    display.display();

  }  if (randNumber == 15) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What irritates you the most?"));
    display.display();

  }  if (randNumber == 16) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is your favorite movie or TV show?"));
    display.display();

  } if (randNumber == 17) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What technology innovation made the most impact in your life?"));
    display.display();

  } if (randNumber == 18) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is something fun you did this weekend?"));
    display.display();

  } if (randNumber == 19) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is your favorite outdoor activity?"));
    display.display();

  } if (randNumber == 20) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is your favorite holiday?"));
    display.display();

  } if (randNumber == 21) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("As a kid what did you want to be when you grew up?"));
    display.display();

  } if (randNumber == 22) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Did you have a nickname growing up? What was it?"));
    display.display();

  } if (randNumber == 23) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What is your favorite mythical creature?"));
    display.display();

  } if (randNumber == 24) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What kind of music did you listen to in highschool?"));
    display.display();

  } if (randNumber == 25) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("What’s your go-to restaurant when you don’t want to cook?"));
    display.display();

  }  if (randNumber == 26) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.println(F("Crunchy or smooth peanut butter?"));
    display.display();
  }

}


