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
#define NUM_SETTINGS 3
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
  Serial.println(SPOKE_LENGTH);

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
      //WheelAuto();
      //WheelManual();
      // Rain();
      //Snake();
      Bass();
      // setting2();
      // EQ();
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

int Wheel_i = 0;
int Wheel_high = false;

void WheelAuto() {
  fadeToBlackBy( leds, NUM_LEDS, 16);

  int threshold = map(potVal, 0, 1023, 1, 6);

  CRGB color = getColor(wheelVal);

  currentMs = millis();
  if(input[1] > 8 && Wheel_high == false && currentMs - startMs >= 200) {
    Wheel_high == true;

    Wheel_i = (Wheel_i + 1) % NUM_SPOKES;
    uint8_t x;
    for(x = 0; x < SPOKE_LENGTH; x++) {
      leds[Wheel_i*SPOKE_LENGTH + x] = color;
    }
    startMs = currentMs;
  } else if (input[1] < 4 && Wheel_high == true) {
    Wheel_high == false;
  }
  
  Serial.println(threshold);
}

void WheelManual() {
  int period = map(potVal, 0, 1023, 600, 50);
  int fade = map(potVal, 0, 1023, 16, 64);

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
  int period = map(potVal, 0, 1023, 15, 50);
  int fade = map(potVal, 0, 1023, 16, 64);
  
  fadeToBlackBy( leds, NUM_LEDS, fade);
  
  currentMs = millis();
  if (currentMs - startMs >= 15) {
    for(x = 0; x < NUM_SPOKES; x++) {
      if(drops[x] < 35) {
        leds[x*SPOKE_LENGTH + drops[x]] = CRGB(0, 0, 128);
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
  
  for(x = 0; x < 8; x++) {
    y = map(input[x], 0, 10, 0, SPOKE_LENGTH);
    // if(y > SPOKE_LENGTH) y = SPOKE_LENGTH;
    for(i = 0; i < y; i++) {
      leds[i + x * SPOKE_LENGTH].setRGB(0, 0, 255);
    }
  }
}

uint8_t setting1index = 17;
uint8_t setting1counter = 0;

void Bass() {
  Serial.println(input[1]);
  
  fadeToBlackBy( leds, NUM_LEDS, 32);

  // for(int i = 0; i < 5; i++) {
    if(input[1] > 8) {
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

uint16_t s2i = 0;

void setting2() {
  fadeToBlackBy( leds, NUM_LEDS, 32);
  
  currentMs = millis();
  if (currentMs - startMs >= 2500) {
    s2i = random8(0, 10);
    
    startMs = currentMs;
  }
  
  if(input[1] > 8) {
    for(int i = s2i; i < NUM_LEDS; i = i+10) {
      // leds[i].setRGB(255, 0, 0);
      leds[i] = wheel(wheelVal);
    }
  }
}

uint16_t Snake_i = 0;

void Snake() {
  currentMs = millis();
  int period = map(potVal, 0, 1023, 100, 10);
  CRGB color;
  switch(wheelVal) {
     case 0:
      color = CRGB(0, 0, 32);
      break;
     case 1:
      color = CRGB(0, 32, 0);
      break;
     case 2:
      color = CRGB(32, 0, 0);
      break;
  }
  
  if (currentMs - startMs >= period) {
    FastLED.clear();
    
    if(Snake_i == NUM_LEDS) {
      Snake_i = 0;
    }
    
    for(int i = Snake_i; i < Snake_i + NUM_LEDS/2; i++) {
      leds[i % NUM_LEDS] = color;
    }
  
    Snake_i++;

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

      switch(setting) {
        case 0:
          maxWheelVal = 4;
          break;
        default:
          maxWheelVal = 0;
      }
      
      // Reset rotary encoder
      wheelVal = 0;
      // Reset LEDs
      resetLEDs();

      Serial.print("Setting");
      Serial.println(setting);
    }
    buttonVal = newButtonVal;
  }

  // Super simple rotary encoder code
  if(newEncVal1 != encVal1 || newEncVal2 != encVal2) {
    if(encVal1 == 1 && newEncVal1 == 1 && encVal2 == 1 && newEncVal2 == 0) {
      wheelVal--;
      if(wheelVal < 0) wheelVal = maxWheelVal;
      
      Serial.print("Backward ");
      Serial.println(wheelVal);
    }

    if(encVal1 == 1 && newEncVal1 == 0 && encVal2 == 1 && newEncVal2 == 1) {
      wheelVal = (wheelVal + 1) % maxWheelVal;
      
      Serial.print("Forward ");
      Serial.println(wheelVal);
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
