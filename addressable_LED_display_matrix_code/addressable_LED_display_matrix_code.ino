#include "display.hpp"

#include <FastLED.h>  // http://fastled.io/docs/3.1/functions.html

unsigned long loop_millis = 0;

const int subdivisions = 100;
int locations[Display::width] = {};
int speeds[Display::width] = {11, 8, 3, 12, 5, 4, 10, 7, 6, 9};
const CRGB colour = CRGB::Green;

void setup() {
  Serial.begin(115200);

  Display::setup();

  randomSeed(analogRead(0));

  for (int x = 0; x < Display::width; ++x)
  {
    locations[x] = (Display::height - speeds[x]) * subdivisions;
  }

  Display::set_on(colour);
}

void loop() {
  loop_millis = millis();

  Display::clear();

  for (int x = 0; x < Display::width; ++x)
  {
    Serial.print(locations[x]);
    Serial.flush();
    Serial.print(',');
    if (locations[x] < -(speeds[x] * subdivisions))
    {
      locations[x] = Display::height * subdivisions;
    }
    for (int y = 0; y <= speeds[x]; ++y)
    {
      Display::on(x, locations[x] / subdivisions + y, colour.lerp8(CRGB::Black, map(y, 0, speeds[x], 0, 255)));
    }
    locations[x] -= speeds[x];
  }
  Serial.print('\n');

  Display::show();

  const unsigned long end_millis = loop_millis + 20;
  while (millis() < end_millis)
  {
    ;
  }
}
