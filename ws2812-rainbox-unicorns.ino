#include "FastLED.h"

void bounds_check();
void update_color();
void set_leds();
void update_current();

enum Scheme {
  FIRST_SCHEME = 0,
  CHASE_FORWARD = 0,
  CHASE_BACKWARD,
  BOUNCE,
  MOVING_RAINBOW,
  RAINBOW_OOZE,
  LAST_SCHEME,
  RANDOM_SCHEME
} scheme = RANDOM_SCHEME;

const unsigned int NUM_LEDS = 50;
CRGB leds[NUM_LEDS];
int currentLed = 0;
int previousLed = 0;
CRGB currentColor = CHSV(0,0,0);
CRGB secondaryColor = CHSV(0,0,0);
char dir = 1;
unsigned int hueCounter = 0;

Scheme currentScheme = FIRST_SCHEME;
unsigned long previousMillis = millis();
unsigned long delayInMillis = 10 * 1000;

const int PIN_FASTLED = 3;
const int PIN_BUTTON = 7;

int lastButtonState = LOW;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(PIN_BUTTON, INPUT);

  FastLED.addLeds<WS2811, PIN_FASTLED, RGB>(leds, NUM_LEDS);
  setup_scheme(); 
}

void setup_scheme() {
  switch (currentScheme) {
    case CHASE_FORWARD:
      dir = 1;
      break;
    case CHASE_BACKWARD:
      dir = -1;
      break;
    case BOUNCE:
    case MOVING_RAINBOW:
      dir = 1;
      break;
    case RAINBOW_OOZE:
      hueCounter = 0;
      break;
    default:
      dir = 1;
      break;
  }
}

const char * to_string(Scheme s) {
  switch (s) {
    case CHASE_FORWARD:   return "CHASE_FORWARD";
    case CHASE_BACKWARD:  return "CHASE_BACKWARD";
    case BOUNCE:          return "BOUNCE";
    case MOVING_RAINBOW:  return "MOVING_RAINBOW";
    case RAINBOW_OOZE:    return "RAINBOW_OOZE";
    default:              return "???";
  };
}

void loop() {
  
  int buttonState = digitalRead(PIN_BUTTON);

  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      // Serial.println("button is HIGH");
    } else {
      // if the current state is LOW then the button went from on to off:
      // Serial.println("button is LOW");
      currentScheme = (currentScheme+1)%(RANDOM_SCHEME-1);
      Serial.print("scheme changed by button to ");
      Serial.println(to_string(currentScheme));
      setup_scheme();
    }
  }
//  Serial.flush();
  
  lastButtonState = buttonState;

  
  bounds_check();
  update_color();
  set_leds();
  FastLED.show();
  delay(how_long());
  update_current();
}


// bounds checks the currentLed to and links the beginning and the end of the strip
void bounds_check() {
  switch (currentScheme) {
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
      break;
    case RAINBOW_OOZE:
      if (hueCounter >= 256) {
        hueCounter = 0;
      }
      break;
    default:
      currentLed %= NUM_LEDS;
  }
}


void update_color() {      
  switch (currentScheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
      currentColor = CHSV(random8(),random8(),random8());
      break;
    case BOUNCE:
      if (currentLed == 0 || currentLed == (NUM_LEDS-1)) {
        currentColor = CHSV(random8(),random8(),random8());
        secondaryColor = CHSV(random8(),random8(),random8());
      }
      break;
    case RAINBOW_OOZE:
        currentColor = CHSV(hueCounter, 255, 255);
      break;
    default:
      currentColor = CHSV(random8(),random8(),random8());
      break;
  }
}


void set_leds() {
  switch (currentScheme) {
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
    case RAINBOW_OOZE:
      for (unsigned char i=0; i<50; i++) {
        leds[i] = currentColor;
      }
      break;
    default:
      leds[currentLed] = currentColor;
      break;
  }
}


int how_long() {
  switch (currentScheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
    case BOUNCE:
      return 20;
    case MOVING_RAINBOW:
      return 100;
    case RAINBOW_OOZE:
      return 100;
    default:
      return 10;
  };
}
  

void update_current() {

  if (scheme == RANDOM_SCHEME) {
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= delayInMillis) {
//      Serial.write("changing schemes\n");
      currentScheme = (currentScheme+1)%RANDOM_SCHEME;
      Serial.println("scheme changed by timer");
      setup_scheme();
      previousMillis = currentMillis;
    }
  }

  previousLed = currentLed;
  switch (currentScheme) {
    case CHASE_FORWARD:
    case CHASE_BACKWARD:
    case BOUNCE:
      currentLed += dir;
      break;
    case RAINBOW_OOZE:
      hueCounter += 1;
      break;
    default:
      currentLed += dir;
      break;
  }
}


