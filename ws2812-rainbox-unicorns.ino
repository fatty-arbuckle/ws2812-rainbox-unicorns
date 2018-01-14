#include "FastLED.h"

void bounds_check();
void update_color();
void set_leds();
void update_current();

enum Scheme {
  CHASE_FORWARD,
  CHASE_BACKWARD,
  BOUNCE,
  MOVING_RAINBOW
} scheme = MOVING_RAINBOW;

const unsigned int NUM_LEDS = 50;
CRGB leds[NUM_LEDS];
int currentLed = 0;
int previousLed = 0;
CRGB currentColor = CHSV(0,0,0);
CRGB secondaryColor = CHSV(0,0,0);
char dir = 1;


void setup() {
  // a WS2811 on pin 3
  FastLED.addLeds<WS2811, 3, RGB>(leds, NUM_LEDS);

  switch (scheme) {
    case CHASE_FORWARD:
      dir = 1;
      break;
    case CHASE_BACKWARD:
      dir = -1;
      break;
    case BOUNCE:
    case MOVING_RAINBOW:
    default:
      dir = 1;
      break;
  }
}

void loop() {
  bounds_check();
  update_color();
  set_leds();
  FastLED.show();
  delay(how_long());
  update_current();
}


// bounds checks the currentLed to and links the beginning and the end of the strip
void bounds_check() {
  switch (scheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
    case MOVING_RAINBOW:
      if (currentLed < 0) {
        currentLed = NUM_LEDS - 1;
      } else if (currentLed >= NUM_LEDS) {
        currentLed = 0;
      } else {
        // in bounds
      }
      break;
    case BOUNCE:
      if (currentLed <= 0) {
        currentLed = 0;
        dir *= -1;
      } else if (currentLed >= (NUM_LEDS-1)) {
        currentLed = (NUM_LEDS-1);
        dir *= -1;
      }
    default:
      currentLed %= NUM_LEDS;
  }
}


void update_color() {
  switch (scheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
    case BOUNCE:
      if (currentLed == 0 || currentLed == (NUM_LEDS-1)) {
        currentColor = CHSV(random8(),random8(),random8());
        secondaryColor = CHSV(random8(),random8(),random8());
      }
      break;
    default:
      currentColor = CHSV(random8(),random8(),random8());
      break;
  }
}


void set_leds() {
  switch (scheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
      leds[currentLed] = currentColor;
      break;
    case BOUNCE:
      leds[currentLed] = currentColor;
      leds[previousLed] = secondaryColor;
      break;
    case MOVING_RAINBOW:
      for (unsigned char i=0; i<8; i++) {
        leds[(currentLed+i)%NUM_LEDS] = CRGB::Red;
      }
      for (unsigned char i=8; i<16; i++) {
        leds[(currentLed+i)%NUM_LEDS] = CRGB::OrangeRed;
      }    
      for (unsigned char i=16; i<24; i++) {
        leds[(currentLed+i)%NUM_LEDS] = CRGB::Yellow;
      }
      for (unsigned char i=24; i<32; i++) {
        leds[(currentLed+i)%NUM_LEDS] = CRGB::Green;
      }
      for (unsigned char i=32; i<40; i++) {
        leds[(currentLed+i)%NUM_LEDS] = CRGB::Blue;
      }
      for (unsigned char i=40; i<50; i++) {
        leds[(currentLed+i)%NUM_LEDS] = CRGB::DarkViolet;
      }
      break;
    default:
      leds[currentLed] = currentColor;
      break;
  }
}


int how_long() {
  switch (scheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
    case BOUNCE:
      return 20;
    case MOVING_RAINBOW:
      return 200;
    default:
      return 10;
  }
}
  

void update_current() {
  previousLed = currentLed;
  switch (scheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
    case BOUNCE:
    default:
      currentLed += dir;
      break;
  }
}


