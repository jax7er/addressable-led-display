#include "display.hpp"

#include <FastLED.h>  // http://fastled.io/docs/3.1/functions.html

int loop_millis = 0;

void setup() {
  Serial.begin(115200);

  Display::setup();

  randomSeed(analogRead(0));
}

const String merry = "MERRY";
const String christmas = "CHRISTMAS";
const int merry_width = Letters::string_width(merry);
const int christmas_width = Letters::string_width(christmas);
const int merry_christmas_width = merry_width + 1 + christmas_width;
int x = Display::right + 1;

void loop() {
  loop_millis = millis();

  Display::clear();

  Letters::draw_string(merry, x, 3, CRGB::Green);
  Letters::draw_string(christmas, x + merry_width + 1, 3, CRGB::Red);

  Display::show();

  --x;
  if (x == (Display::left - merry_christmas_width)) {
    x = Display::right + 1;
  }

  delay(200);
}
