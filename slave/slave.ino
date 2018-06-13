#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#include <Wire.h>

// I2C input data
byte input[8];

// LEDs
#define LED_PIN 8
#define NUM_LEDS 300
#define BRIGHTNESS 255

CRGB leds[NUM_LEDS];

// Settings
#define NUM_SETTINGS 3
#define NUM_MODES 3

// Controls
#define SETTING_PIN 7
bool settingBtnPressed = false;
int setting = 0;

unsigned long sStartMillis;  //some global variables available anywhere in the program
unsigned long sCurrentMillis;
unsigned int period = 5000;

void setup() {
  Serial.begin(9600);
  Serial.println("Slave is here");

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  FastLED.setBrightness(BRIGHTNESS);

  // cStartMillis = millis();  //initial start time
  sStartMillis = millis();
  
  Wire.begin(1);
  Wire.onReceive(onData);
}

void onData(int numBytes) {
  int i = 0;
  while(Wire.available()) {
    input[i] = Wire.read();
    i++;
  }
}

void loop() {
  // Read control inputs
  readInputs();
  
  if(input[1] > 8) {
    digitalWrite(13, HIGH); 
  } else {
    digitalWrite(13, LOW);
  }
  
  int j = NUM_LEDS;
  while(j--) {
    leds[j].fadeToBlackBy( 32 );
  }

  switch(setting) {
    case 0:
      setting1();
      break;
    case 1:
      setting2();
      break;
  }
}

int getIndex(int current, int maximum, int add) {
  int ret = current+add;
  if(ret < 0) ret += maximum;
  if(ret > maximum) ret -= maximum;
  return ret;
}

int setting1index = 20;
int setting1counter = 0;
int setting1color = 0;
int s1r = 0;
int s1g = 0;
int s1b = 255;

void setting1() {
  /*
  sCurrentMillis = millis();
  if (sCurrentMillis - sStartMillis >= period) {
    // setting1index = random16(NUM_LEDS);
    setting1color = (setting1color + 1) % 3;

    switch(setting1color) {
      case 0:
        s1r = 0;
        s1g = 0;
        s1b = 255;
        break;
      case 1:
        s1r = 255;
        s1g = 0;
        s1b = 0;
        break;
      case 2:
        s1r = 0;
        s1g = 255;
        s1b = 0;
        break;
    }
    
    Serial.println(setting1index);
    sStartMillis = sCurrentMillis;
  }
  */

  if(input[1] > 8) {
    setting1counter = 0;
    leds[setting1index].setRGB(s1r, s1g, s1b);
    setting1index = random16(NUM_LEDS);
  }

  if(setting1counter < 21) {
    setting1counter++;
  }

  if(setting1counter <= 20) {
    leds[getIndex(setting1index, NUM_LEDS, -setting1counter)].setRGB(s1r, s1g, s1b);
    leds[getIndex(setting1index, NUM_LEDS, +setting1counter)].setRGB(s1r, s1g, s1b);
  }

  FastLED.show();
}

void setting2() {
  for(int i = 0; i < input[1]; i++) {
    leds[i].setRGB(255, 0, 0);
  }
  
  FastLED.show();
}

void readInputs() {
  if(digitalRead(SETTING_PIN) == HIGH && settingBtnPressed == true) {
    settingBtnPressed = false;
    setting = (setting + 1) % NUM_SETTINGS;
    Serial.println("Setting: " + String(setting));
    resetLEDs();
  } else if(digitalRead(SETTING_PIN) == LOW && settingBtnPressed == false) {
    settingBtnPressed = true;
  }
}

void resetLEDs() {
  FastLED.clear();
  FastLED.show();
  // FastLED.delay(250);
}

