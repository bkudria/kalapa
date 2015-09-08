#include <FastLED.h>

#define NUM_LEDS 16
#define DATA_PIN 0

#define BUTTON 1
#define PULLUP true
#define INVERT true
#define DEBOUNCE_MS 20
#define LONG_PRESS 1000

CRGB leds[NUM_LEDS];

unsigned long lastMillis        = 0;
unsigned long lastButtonPressAt = 0;

byte currentPattern = 2;
byte brightness = 4;

byte hue = HUE_RED;
bool periodic(short period, const unsigned char values[], CHSV color){
  unsigned long t = millis();
  int index = ( t % period) * 64 / period;
  if (t%2 == 0) hue = (hue + 1) % 255;
  if (color.s == 0) {
    fill_gradient(leds, 0, CHSV(hue, 255, values[index]/brightness), 15, CHSV(hue+1, 255, values[index]/brightness), LONGEST_HUES);
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(CHSV(color.h, color.s, values[index]/brightness)));
  }
  return true;
}

const unsigned char breath_values[] = {0,0,2,9,17,28,40,53,67,82,96,110,124,137,149,160,170,180,188,195,202,208,213,217,221,224,226,229,230,231,232,233,233,233,232,231,230,228,226,223,220,216,212,207,201,194,186,178,168,158,147,134,121,107,93,79,64,51,38,26,16,7,1,0};
bool breath() { return periodic(4000, breath_values, CHSV(HUE_YELLOW, 150, 0)); }

bool rainbow_breath() { return periodic(4000, breath_values, CHSV(0, 0, 0)); }

const unsigned char heartbeat_values[]  = {65,65,65,65,65,65,65,65,65,65,65,65,66,66,68,70,74,80,88,99,114,131,151,172,194,214,232,245,250,249,239,221,197,168,136,103,73,47,25,10,2,-1,2,8,16,25,34,42,49,54,58,61,63,64,64,65,65,65,65,65,65,65,65,65};
bool heartbeat() { return periodic(1000, heartbeat_values, CHSV(HUE_RED, 255, 0)); }

byte offset = 0;
CHSV color = CHSV(HUE_RED, 255, 200);
bool spinners(unsigned long delayMS) {
  if (delayMS % 20 == 0) color.h++;
  if (delayMS > 90) {
    offset = (offset + 1)%16;
    for(int i=0; i<16; i++) {
      CHSV c = CHSV(0,0,0);
      byte led = (offset + i) % 8;
      for (byte tail = 0; tail < 7; tail++){
        if (led == tail) c = CHSV(color.h + 2*tail*tail, color.s, max(0, (color.v/brightness - 2*tail*tail)) );
      }
      leds[i] = c;
    }
    return true;
  } else {return false;}
}


void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  unsigned long t = millis();
  unsigned long delayMS      = t - lastMillis;
  short lastButtonPressSince = t - lastButtonPressAt;
  if (lastButtonPressSince > 500 && digitalRead(BUTTON) == LOW) {
    lastButtonPressAt = t;
    changePattern();
  }

  bool result;
  switch (currentPattern) {
    case 0:
      result = breath();
      break;
    case 1:
      result = heartbeat();
      break;
    case 2:
      result = spinners(delayMS);
      break;
    case 3:
      result = rainbow_breath();
      break;
    default:
      result = heartbeat();
  }
  if (result) { lastMillis = t; }

  FastLED.show();
}

void changePattern() {
  brightness += 2;
  if (brightness > 6) {
    brightness = 4;
    currentPattern = (currentPattern + 1)%4;
  }
}

