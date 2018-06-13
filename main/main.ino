#include <FastLED.h>

#define LED_PIN 8
#define NUM_LEDS 300
#define BRIGHTNESS 255
#define NUM_SETTINGS 3
#define NUM_MODES 3

// Settings button
#define SETTING_PIN 7
bool settingBtnPrev = true;
bool settingBtnCurr = false;
int setting = 0;

// Mode button
#define MODE_PIN 6
bool modeBtnPrev = true;
bool modeBtnCurr = false;
int mode = 0;

// Potentiometer
#define POT_PIN 5
float potVal = 0;

// Mic
#define MIC_PIN = 0
const int micSampleInterval = 50;
unsigned int micSample;

CRGB leds[NUM_LEDS];

// "c" is for "controls"
unsigned long cStartMillis;  //some global variables available anywhere in the program
unsigned long cCurrentMillis;

// "s" is for "setting"
unsigned long sStartMillis;  //some global variables available anywhere in the program
unsigned long sCurrentMillis;
const unsigned int period = 1000;

void setup() {
  Serial.begin(9600);
  
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  FastLED.setBrightness(BRIGHTNESS);

  cStartMillis = millis();  //initial start time
  sStartMillis = millis();
}

void loop() {
   readInputs();
  
  // put your main code here, to run repeatedly:
  cCurrentMillis = millis();
  
  switch(setting) {
    case 0:
      setting1();
      break;
    case 1:
      setting2();
      break;
  }
}

void readInputs() {
  settingBtnCurr = digitalRead(SETTING_PIN);
  
  if (settingBtnCurr == LOW && settingBtnCurr != settingBtnPrev) {
    setting = (setting + 1) % NUM_SETTINGS;
    Serial.println("Setting: " + String(setting));
    resetLEDs();
  } else {
    settingBtnPrev = HIGH;
  }
  settingBtnPrev = settingBtnCurr;

  modeBtnCurr = digitalRead(MODE_PIN);

  if (modeBtnCurr == LOW && modeBtnCurr != modeBtnPrev) {
    mode = (mode + 1) % NUM_MODES;
    Serial.println("Mode: " + String(mode));
    resetLEDs();
  } else {
    modeBtnPrev = HIGH;
  }
  modeBtnPrev = modeBtnCurr;

  potVal = analogRead(POT_PIN);
}

void resetLEDs() {
  FastLED.clear();
  FastLED.show();
  FastLED.delay(500);
}

int s1index = 0;

void setting1() {
  sCurrentMillis = millis();

  int interval = 275 - map(potVal, 0, 1023, 25, 250);

  int r = 255;
  int g = 0;
  int b = 0;
  
  if (sCurrentMillis - sStartMillis >= interval)  //test whether the period has elapsed
  {
    int i;
    for(i = 0; i < 300; i++) {
      leds[i].setRGB(0, 0, 0);
    }
    
    for(i = s1index; i < 300; i = i+30) {
      leds[i].setRGB(255, 0, 0);
    }
    s1index++;
    if(s1index == 15) s1index = 0;
    
    FastLED.show();
    sStartMillis = sCurrentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

int s2arr [20][2] = {
  {16, 21},
  {30, 70},
  {212, 169},
  {166, 42},
  {6, 54},
  {255, 217},
  {295, 112},
  {124, 233},
  {180, 249},
  {1, 77},
  {243, 252},
  {22, 162},
  {23, 21},
  {185, 163},
  {174, 27},
  {163, 91},
  {211, 101},
  {86, 89},
  {287, 41},
  {48, 44}
};

void setting2() {
  sCurrentMillis = millis();
  
  if (sCurrentMillis - sStartMillis >= 15)  //test whether the period has elapsed
  {
    for(int i = 0; i < 20; i++) {
      int index = s2arr[i][0];
      int color = s2arr[i][1];
      leds[index].setRGB(0, 0, color);

      color -= 5;
      if(color < 0) {
        leds[index].setRGB(0, 0, 0);
        s2arr[i][0] = (index + 33) % NUM_LEDS;
        s2arr[i][1] = 255;
      } else {
        s2arr[i][1] = color;
      }
    }
    
    FastLED.show();
    sStartMillis = sCurrentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

