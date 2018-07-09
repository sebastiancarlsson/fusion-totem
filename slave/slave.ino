#include <TimerOne.h>
#include <ClickEncoder.h>
#include <FastLED.h>
#include <Wire.h>

// I2C input data
byte input[8];

// LEDs
#define LED_PIN 8
#define NUM_LEDS 280
#define NUM_SPOKES 8
int SPOKE_LENGTH = NUM_LEDS/NUM_SPOKES;
#define BRIGHTNESS 128
CRGB leds[NUM_LEDS];

// BPM in milliseconds
#define BPM_LOW 521 // 115 BPM
#define BPM_HIGH 461 // 130 BPM

// Settings
#define NUM_SETTINGS 10
byte setting = 0;

// Controls
int wheelVal = 0;
int maxWheelVal = 4;

// Potentiometer
#define POT_PIN A6
int potVal = 0;

unsigned long startMs;
unsigned long currentMs;

void setup() {
  Serial.begin(115200);
  Serial.println("Slave is here");

  pinMode(13, OUTPUT);

  // Setup LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  FastLED.setBrightness(BRIGHTNESS);

  // Setup data receiving/sending
  Wire.begin(1);
  Wire.onReceive(onData);

  // Setup rotary encoder
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  // Start timing
  startMs = millis();
}

void onData(int numBytes) {
  int i = 0;
  while(Wire.available()) {
    input[i] = Wire.read();
    i++;
  }
}

void loop() {
  if(input[1] > 8) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  
  // Read control inputs
  readInputs();

  // Animate
  switch(setting) {
    case 0:
      Color();
      break;
    case 1:
      Rain();
      break;
    case 2:
      Spaceship();
      break;
    case 3:
      Snake(2);
      break;
    case 4:
      Snake(4);
      break;
    case 5:
      Snake(8);
      break;
    case 6:
      WheelAuto();
      break;
    case 7:
      WheelManual();
      break;
      /*
    case 10:
      Bass();
      break;
      */
    case 8:
      Twinkle();
      break;
    case 9:
      EQ();
      break;
  }

  // Update LEDs
  FastLED.show();
}

CRGB getColor(int i) {
  switch(i) {
    case 0:
      return CRGB(0, 0, 255);
      break;
    case 1:
      return CRGB(255, 0, 255);
      break;
    case 2:
      return CRGB(255, 0, 0);
      break;
    case 3:
      return CRGB(0, 255, 0);
      break;
    case 4:
      return CRGB(0, 255, 255);
      break;
  }
}

void Color() {
  FastLED.clear();
  int x, y, start;
   
  if(potVal > 1000) {
    start = 30;
  } else {
    start = map(potVal, 0, 1023, 0, 30);
  }

  CRGB color = getColor(wheelVal);
  
  for(x = 0; x < NUM_SPOKES; x++) {
    for(y = start; y < start+5; y++) {
      leds[x*SPOKE_LENGTH + y] = color;
    }
  }
}

int Wheel_i = 0;
int Wheel_high = false;

void WheelAuto() {
  int threshold = map(potVal, 0, 1023, 4, 8);
  fadeToBlackBy( leds, NUM_LEDS, 16);

  CRGB color = getColor(wheelVal);

  currentMs = millis();
  if(input[1] > threshold && Wheel_high == false && currentMs - startMs >= 300) {
    Wheel_high == true;

    //Wheel_i = (Wheel_i + 1) % NUM_SPOKES;
    Wheel_i = random8(0, 8);
    uint8_t x;
    for(x = 0; x < SPOKE_LENGTH; x++) {
      leds[Wheel_i*SPOKE_LENGTH + x] = color;
    }
    startMs = currentMs;
  }
  
  if (input[1] < 4 && Wheel_high == true) {
    Wheel_high == false;
    
  }
}

void WheelManual() {
  int period = map(potVal, 0, 1023, 600, 25);
  int fade = map(potVal, 0, 1023, 16, 127);

  fadeToBlackBy( leds, NUM_LEDS, fade);

  CRGB color = getColor(wheelVal);
  
  currentMs = millis();
  if (currentMs - startMs >= period) {
    Wheel_i = (Wheel_i + 1) % NUM_SPOKES;

    uint8_t x;
    for(x = 0; x < SPOKE_LENGTH; x++) {
      leds[Wheel_i*SPOKE_LENGTH + x] = color;
    }

    // Serial.println(Wheel_i);
    // Serial.println(Wheel_i*SPOKE_LENGTH);
    
    startMs = currentMs;
  }
}

uint8_t drops[8] = {35, 35, 35, 35, 35, 35, 35, 35};

void Rain() {
  int x;
  int period = map(potVal, 0, 1023, 10, 50);
  int color = map(potVal, 0, 1023, 0, 255);
  
  fadeToBlackBy( leds, NUM_LEDS, 32);
  
  currentMs = millis();
  if (currentMs - startMs >= period) {
    for(x = 0; x < NUM_SPOKES; x++) {
      if(drops[x] < 35) {
        leds[x*SPOKE_LENGTH + drops[x]] = wheel(color);
        drops[x]++;
      }
    }
    
    int spokeIndex = random8(NUM_SPOKES);
    if(drops[spokeIndex] == 35) {
      drops[spokeIndex] = random8(SPOKE_LENGTH);
    }
    
    startMs = currentMs;
  }
}

void EQ() {
  uint8_t x, y, i;

  fadeToBlackBy( leds, NUM_LEDS, 64);

  double m = map_double(potVal, 0, 1023, 0.1, 1.0);
  
  CRGB color = getColor(wheelVal);
  
  for(x = 0; x < 8; x++) {
    switch(x) {
      case 0:
        y = map(input[0]*0.2*m, 0, 10, 0, SPOKE_LENGTH);
        break;
      case 1: 
        y = map(input[1]*0.9*m, 0, 10, 0, SPOKE_LENGTH);
        break;
      case 2:
        y = map(min(input[2]*5*m, 10), 0, 10, 0, SPOKE_LENGTH);
        break;
      case 3:
        y = map(min(input[3]*3*m, 10), 0, 10, 0, SPOKE_LENGTH);
        break;
      case 4:
        y = map(min(input[4]*3*m, 10), 0, 10, 0, SPOKE_LENGTH);
        break;
      case 5:
        y = map(min(input[5]*3*m, 10), 0, 10, 0, SPOKE_LENGTH);
        break;
      case 6:
        y = map(min(input[6]*3*m, 10), 0, 10, 0, SPOKE_LENGTH);
        break;
      case 7:
        y = map(min(input[7]*4*m, 10), 0, 10, 0, SPOKE_LENGTH);
        break;
      case 8:
        y = map(input[x]*m, 0, 10, 0, SPOKE_LENGTH);
        break;
    }
    
    for(i = 0; i < y; i++) {
      leds[i + x * SPOKE_LENGTH] = color;
    }
  }
}
/*
uint8_t setting1index = 17;
uint8_t setting1counter = 0;

void Bass() {
  //Serial.println(input[1]);
  int threshold = map(potVal, 0, 1023, 4, 8);
  
  fadeToBlackBy( leds, NUM_LEDS, 32);

  // for(int i = 0; i < 5; i++) {
    if(input[1] > threshold) {
      setting1counter = 0;
      leds[setting1index].setRGB(0, 0, 255);
      //setting1index = random16(NUM_LEDS);
    }
    
    if(setting1counter < 17) {
      setting1counter++;
      leds[getIndex(setting1index, NUM_LEDS, -setting1counter)].setRGB(0, 0, 255);
      leds[getIndex(setting1index, NUM_LEDS, +setting1counter)].setRGB(0, 0, 255);
    }
  // }
}
*/

uint16_t s2i = 0;
void Twinkle() {
  int threshold = map(potVal, 0, 1023, 4, 8);
  fadeToBlackBy( leds, NUM_LEDS, 32);
  CRGB color = getColor(wheelVal);
  
  currentMs = millis();
  if (currentMs - startMs >= 2500) {
    s2i = random8(0, 8);
    startMs = currentMs;
  }
  
  if(input[1] > threshold) {
    for(int i = s2i; i < NUM_LEDS; i = i+8) {
      leds[i] = color;
    }
  }
}

uint16_t Snake_i = 0;

void Snake(int numSnakes) {
  int i, j, s, sb;
  currentMs = millis();
  int period = map(potVal, 0, 1023, 100, 5);
  CRGB color = getColor(wheelVal);
  
  if (currentMs - startMs >= period) {
    FastLED.clear();
    
    if(Snake_i == NUM_LEDS) {
      Snake_i = 0;
    }
    
    for(i = Snake_i; i < Snake_i + 15; i++) {
      s = floor((i % NUM_LEDS)/SPOKE_LENGTH);
      sb = s*SPOKE_LENGTH;
      if(s % 2 == 0) {
        for(j = 0; j < numSnakes; j++) {
         leds[((sb+j*NUM_LEDS/numSnakes) % NUM_LEDS)+SPOKE_LENGTH - (i + j*NUM_LEDS/numSnakes)%35 - 1] = color;
        }
      } else {
        for(j = 0; j < numSnakes; j++) {
          leds[(i + j*NUM_LEDS/numSnakes) % NUM_LEDS] = color;
        }
      }
      
    }
  
    Snake_i++;

    startMs = currentMs;
  }
}

uint8_t Spaceship_i = 0;
int Spaceship_startMs = millis();
int Spaceship_currentMs = 0;
uint8_t Spaceship_color = 0;
void Spaceship() {
  fadeToBlackBy( leds, NUM_LEDS, 16);
  
  uint8_t pos, spoke;
  currentMs = millis();
  int period = map(potVal, 0, 1023, 25, 4);

  CRGB color;
  switch(wheelVal) {
     case 1:
      color = CRGB(0, 0, 255);
      break;
     case 2:
      color = CRGB(0, 255, 0);
      break;
     case 3:
      color = CRGB(255, 0, 0);
      break;
     case 4:
      color = CRGB(127, 0, 255);
      break;
     default:
      Spaceship_currentMs = millis();
      if(Spaceship_currentMs - Spaceship_startMs >= 400) {
        Spaceship_color++;
        if(Spaceship_color > 255) Spaceship_color = 0;
        Spaceship_startMs = Spaceship_currentMs;
      }
      color = wheel(Spaceship_color);
      break;
  }

  if (period < 5 || currentMs - startMs >= period) {
    //FastLED.clear();
    
    pos = map(sin8(Spaceship_i), 0, 255, 0, 34);

    for(spoke = 0; spoke < NUM_SPOKES; spoke++) {
      leds[spoke * SPOKE_LENGTH + pos] = color;
    }
    
    Spaceship_i++;
    startMs = currentMs;
  }
}

byte encVal1 = -1;
byte encVal2 = -1;
byte buttonVal = 1;

void readInputs() {
  byte newEncVal1 = digitalRead(A0);
  byte newEncVal2 = digitalRead(A1);
  byte newButtonVal = digitalRead(A2);
  
  potVal = analogRead(POT_PIN);
  
  if(newButtonVal != buttonVal) {
    if(newButtonVal == HIGH && buttonVal == LOW) {
      setting = (setting + 1) % NUM_SETTINGS;
      
      // Reset rotary encoder
      wheelVal = 0;
      // Reset LEDs
      resetLEDs();

      //Serial.print("Setting");
      //Serial.println(setting);
    }
    buttonVal = newButtonVal;
  }

  // Super simple rotary encoder code
  if(newEncVal1 != encVal1 || newEncVal2 != encVal2) {
    if(encVal1 == 1 && newEncVal1 == 1 && encVal2 == 1 && newEncVal2 == 0) {
      wheelVal--;
      if(wheelVal < 0) wheelVal = maxWheelVal;
      
      //Serial.print("Backward ");
      //Serial.println(wheelVal);
    }

    if(encVal1 == 1 && newEncVal1 == 0 && encVal2 == 1 && newEncVal2 == 1) {
      wheelVal = (wheelVal + 1) % maxWheelVal;
      
      //Serial.print("Forward ");
      //Serial.println(wheelVal);
    }
    
    encVal1 = newEncVal1;
    encVal2 = newEncVal2; 
  }
}

void resetLEDs() {
  FastLED.clear();
  FastLED.show();
}

// Utility function to wrap array index
int getIndex(int current, int maximum, int add) {
  /*
  Serial.print(current);
  Serial.print(" ");
  Serial.print(maximum);
  Serial.print(" ");
  Serial.println(add);
  */
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

float map_double(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
