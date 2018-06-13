#include <TimerOne.h>
#include <ClickEncoder.h>
#include <FastLED.h>
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
uint8_t setting = 1;

// Controls
ClickEncoder encoder(A1, A0, A2, 4);
int16_t encoderValLast = -1;
int16_t encoderVal;
uint8_t buttonState;

uint8_t wheelValue = 0;
#define WHEEL_DELTA 8

void timerIsr() {
  encoder.service();
}

unsigned long sStartMillis;  //some global variables available anywhere in the program
unsigned long sCurrentMillis;
unsigned int period = 100;

void setup() {
  Serial.begin(9600);
  Serial.println("Slave is here");

  // Setup LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  FastLED.setBrightness(BRIGHTNESS);

  // Setup data receiving
  Wire.begin(1);
  Wire.onReceive(onData);

  // Setup rotary encoder
  encoder.setAccelerationEnabled(false);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 

  // Start timing
  sStartMillis = millis();
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
  /*
  if(input[1] > 8) {
    digitalWrite(13, HIGH); 
  } else {
    digitalWrite(13, LOW);
  }
  */
  
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

uint8_t setting1index = 20;
uint8_t setting1counter = 0;
uint8_t setting1color = 0;
uint8_t s1r = 0;
uint8_t s1g = 0;
uint8_t s1b = 255;

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
    // leds[i].setRGB(255, 0, 0);
    leds[i] = wheel(wheelValue);
  }
  
  FastLED.show();
}

void readInputs() {
  encoderVal += encoder.getValue();
  
  if (encoderVal != encoderValLast) {
    if(encoderVal > encoderValLast) {
      wheelValue = getIndex(wheelValue, 255, WHEEL_DELTA);
    } else {
      wheelValue = getIndex(wheelValue, 255, -WHEEL_DELTA);
    }
    encoderValLast = encoderVal;
//    Serial.print("Encoder Value: ");
//    Serial.println(encoderVal);
    Serial.print("Wheel Value: ");
    Serial.println(wheelValue);
  }

  buttonState = encoder.getButton();

  if (buttonState != 0) {
    Serial.print("Button: "); Serial.println(buttonState);
    switch (buttonState) {
      case ClickEncoder::Open:          //0
        break;

      case ClickEncoder::Closed:        //1
        break;

      case ClickEncoder::Pressed:       //2
        break;

      case ClickEncoder::Held:          //3
        break;

      case ClickEncoder::Released:      //4
        break;

      case ClickEncoder::Clicked:       //5
        setting = (setting + 1) % NUM_SETTINGS;
        resetLEDs();
        break;

      case ClickEncoder::DoubleClicked: //6
        break;
    }
  }
}

void resetLEDs() {
  FastLED.clear();
  FastLED.show();
}

// Utility function to wrap array index
int getIndex(int current, int maximum, int add) {
  Serial.print(current);
  Serial.print(" ");
  Serial.print(maximum);
  Serial.print(" ");
  Serial.println(add);
  int ret = current+add;
  if(ret < 0) ret += maximum;
  if(ret > maximum) ret -= maximum;
  return ret;
}

CRGB wheel(int WheelPos) {
  CRGB color;
  if (85 > WheelPos) {
   color.r=0;
   color.g=WheelPos * 3;
   color.b=(255 - WheelPos * 3);
  } 
  else if (170 > WheelPos) {
   color.r=WheelPos * 3;
   color.g=(255 - WheelPos * 3);
   color.b=0;
  }
  else {
   color.r=(255 - WheelPos * 3);
   color.g=0;
   color.b=WheelPos * 3;
  }
  return color;
}
