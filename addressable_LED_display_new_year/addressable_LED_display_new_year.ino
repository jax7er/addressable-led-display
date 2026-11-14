#include "display.hpp"

#include <FastLED.h>  // http://fastled.io/docs/3.1/functions.html

int loop_millis = 0;

struct StringColour {
};

#define STRINGS_LEN 4
const String strings[STRINGS_LEN] = { "HAPPY", "NEW", "YEAR", "!" };
const CRGB::HTMLColorCode string_colours[STRINGS_LEN] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White};
int string_widths[STRINGS_LEN] = {};
int total_width = 0;
int x = Display::right + 1;

void setup() {
  Serial.begin(115200);

  Display::setup();

  randomSeed(analogRead(0));

  int w;
  for (int i = 0; i < STRINGS_LEN; ++i) {
    w = Letters::string_width(strings[i]);
    string_widths[i] = w;
    total_width += w + 1;
  }
  total_width -= 1;
}

void loop() {
  loop_millis = millis();

  Display::clear();

  int pos = x;
  for (int i = 0; i < STRINGS_LEN; ++i) {
    Letters::draw_string(strings[i], pos, 3, string_colours[i]);
    pos += string_widths[i] + 1;
  }

  Display::show();

  --x;
  if (x == (Display::left - total_width)) {
    x = Display::right + 1;
  }

  delay(200);
}
