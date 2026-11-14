#include <RTClib.h>

#include <FastLED.h> // http://fastled.io/docs/3.1/functions.html

RTC_DS1307 rtc;

uint8_t start_month = (11 - 1);  // 0-11
uint8_t start_day = (17 - 1);  // 0-30
uint8_t start_hour = 22;  // 0-23
uint8_t start_minute = 18;  // 0-59
uint8_t start_second = 0;  // 0-59

uint64_t period_1s_ms = 1000ULL;  // must be at least 1

#define s_per_minute (60ULL)
#define s_per_hour (s_per_minute * 60ULL)
#define s_per_day (s_per_hour * 24ULL)
#define s_per_year (s_per_day * 365ULL)

#define for_range(counter, less_than) for (size_t counter = 0; counter < less_than; counter++)

#define WIDTH    10UL
#define HEIGHT   11UL
#define BOTTOM   0UL
#define LEFT     0UL
#define TOP      (HEIGHT - 1UL)
#define RIGHT    (WIDTH - 1UL)
#define NUM_LEDS (WIDTH * HEIGHT)

#define LED_PIN     2
#define BRIGHTNESS  32 // MAX 38 for 1A for 110 LEDs white, diode on nano USB 5V rated for 1A

CRGB leds[NUM_LEDS];
#define xy(x, y) (((x) % 2) ? (HEIGHT * ((x) + 1)) - (y) - 1 : (y) + (HEIGHT * (x))) // serpentine layout, origin at bottom left
#define led(x, y) leds[xy(x, y)]

CRGB on = CRGB::White;
CRGB off = CRGB::Black;
#define ledOn(x, y) led((x), (y)) = on
#define ledOff(x, y) led((x), (y)) = off
#define isLedOn(x, y) led((x), (y)) != off
#define isLedOff(x, y) led((x), (y)) == off

DateTime now;
DateTime year_start;
uint32_t s_this_year;
uint32_t s_this_month;
uint32_t s_this_day;
uint32_t s_this_hour;
uint32_t s_this_minute;
uint64_t ms_this_second;
uint64_t loop_millis;
uint64_t loop_seconds;
uint64_t prev_loop_seconds;
uint8_t month;
uint32_t days_per_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
uint32_t s_per_month[12];
#define n_leds(ring) (WIDTH - 2 * ring) * 2 + (HEIGHT - 2 * (ring + 1)) * 2
const uint8_t n_year_leds = n_leds(0);
const uint8_t n_month_leds = n_leds(1);
const uint8_t n_day_leds = n_leds(2);
const uint8_t n_hour_leds = n_leds(3);
const uint8_t n_minute_leds = n_leds(4);
CRGB * year_leds[n_year_leds] = {
  &led(5, 10), &led(6, 10), &led(7, 10), &led(8, 10), &led(9, 10),
  &led(9, 9), &led(9, 8), &led(9, 7), &led(9, 6), &led(9, 5), &led(9, 4), &led(9, 3), &led(9, 2), &led(9, 1), &led(9, 0),
  &led(8, 0), &led(7, 0), &led(6, 0), &led(5, 0), &led(4, 0), &led(3, 0), &led(2, 0), &led(1, 0), &led(0, 0),
  &led(0, 1), &led(0, 2), &led(0, 3), &led(0, 4), &led(0, 5), &led(0, 6), &led(0, 7), &led(0, 8), &led(0, 9), &led(0, 10),
  &led(1, 10), &led(2, 10), &led(3, 10), &led(4, 10)
};
CRGB * month_leds[n_month_leds] = {
  &led(5, 9), &led(6, 9), &led(7, 9), &led(8, 9),
  &led(8, 8), &led(8, 7), &led(8, 6), &led(8, 5), &led(8, 4), &led(8, 3), &led(8, 2), &led(8, 1),
  &led(7, 1), &led(6, 1), &led(5, 1), &led(4, 1), &led(3, 1), &led(2, 1), &led(1, 1),
  &led(1, 2), &led(1, 3), &led(1, 4), &led(1, 5), &led(1, 6), &led(1, 7), &led(1, 8), &led(1, 9),
  &led(2, 9), &led(3, 9), &led(4, 9)
};
CRGB * day_leds[n_day_leds] = {
  &led(5, 8), &led(6, 8), &led(7, 8),
  &led(7, 7), &led(7, 6), &led(7, 5), &led(7, 4), &led(7, 3), &led(7, 2),
  &led(6, 2), &led(5, 2), &led(4, 2), &led(3, 2), &led(2, 2),
  &led(2, 3), &led(2, 4), &led(2, 5), &led(2, 6), &led(2, 7), &led(2, 8),
  &led(3, 8), &led(4, 8)
};
CRGB * hour_leds[n_hour_leds] = {
  &led(5, 7), &led(6, 7),
  &led(6, 6), &led(6, 5), &led(6, 4), &led(6, 3),
  &led(5, 3), &led(4, 3), &led(3, 3),
  &led(3, 4), &led(3, 5), &led(3, 6), &led(3, 7),
  &led(4, 7)
};
CRGB * minute_leds[n_minute_leds] = {
  &led(5, 6),
  &led(5, 5), &led(5, 4),
  &led(4, 4),
  &led(4, 5), &led(4, 6)
};

void light_leds(CRGB **x_leds, const CRGB& colour, uint32_t seconds_this_x, uint32_t seconds_per_x, uint32_t n_x_leds, uint32_t s_this_day)
{
  static uint32_t led_i, intensity, intensity_timed, seconds_per_led, leds_to_light;
  static uint32_t intensity_max = 1000UL;
  static uint32_t intensity_min = intensity_max / 8UL;
  static CRGB colour_timed;
  
  seconds_per_led = seconds_per_x / n_x_leds;
  leds_to_light = (n_x_leds * seconds_this_x) / seconds_per_x;
  intensity_timed = map(
    (s_this_day < (s_per_day / 2UL) ? s_this_day : s_per_day - s_this_day),
    0UL, s_per_day / 2UL,
    intensity_min, intensity_max
  );
  colour_timed = CRGB(
    map(intensity_timed, 0UL, intensity_max, 0UL, colour.red),
    map(intensity_timed, 0UL, intensity_max, 0UL, colour.green),
    map(intensity_timed, 0UL, intensity_max, 0UL, colour.blue)
  );
  
  for (led_i = 0; led_i < leds_to_light; led_i++)
  {
    *x_leds[led_i] = colour_timed;
  }
  
  if (leds_to_light < n_x_leds)
  {
    intensity = map(seconds_this_x % seconds_per_led, 0UL, seconds_per_led - 1UL, 0UL, intensity_timed);
    intensity = (intensity * intensity) / intensity_timed;
    
    *x_leds[leds_to_light] = CRGB(
      map(intensity, 0UL, intensity_max, 0UL, colour.red),
      map(intensity, 0UL, intensity_max, 0UL, colour.green),
      map(intensity, 0UL, intensity_max, 0UL, colour.blue)
    );
  }
}

void setup()
{
  Serial.begin(115200);

  if (rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  randomSeed(analogRead(0));
  FastLED.setBrightness(BRIGHTNESS);

  for (month = 0; month < 12; month++)
  {
    s_per_month[month] = days_per_month[month] * s_per_day;
  }  

  now = rtc.now();
  year_start = DateTime(now.year(), 1, 1, 0, 0, 0);
//  s_this_year = (now - year_start).totalseconds();

  s_this_year = start_day * s_per_day + start_hour * s_per_hour + start_minute * s_per_minute + start_second;
  for (month = 0; month < start_month; month++)
  {
    s_this_year += s_per_month[month];
  }
}

void loop()
{
  loop_millis = millis();
  loop_seconds = loop_millis / period_1s_ms;
  ms_this_second = loop_millis % period_1s_ms;

//  now = rtc.now();
//  if (now.year() != year_start.year())
//  {
//    year_start = DateTime(now.year(), 1, 1, 0, 0, 0);
//  }
//  s_this_year = (now - year_start).totalseconds();

  if (loop_seconds != prev_loop_seconds)
  {
    s_this_year = (s_this_year + 1) % s_per_year;
  }
    
  s_this_month = s_this_year;
  month = 0;
  while (s_this_month >= s_per_month[month])
  {
    s_this_month -= s_per_month[month];
    month++;
  }
  month--;
  s_this_day = s_this_year % s_per_day;
  s_this_hour = s_this_year % s_per_hour;
  s_this_minute = s_this_year % s_per_minute;
  
  clearLeds();

  light_leds(year_leds, CRGB::Purple, s_this_year, s_per_year, n_year_leds, s_this_day);
  light_leds(month_leds, CRGB::Blue, s_this_month, s_per_month[month], n_month_leds, s_this_day);
  light_leds(day_leds, CRGB::Green, s_this_day, s_per_day, n_day_leds, s_this_day);
  light_leds(hour_leds, CRGB::Yellow, s_this_hour, s_per_hour, n_hour_leds, s_this_day);
  light_leds(minute_leds, CRGB::Red, s_this_minute * period_1s_ms + ms_this_second, s_per_minute * period_1s_ms, n_minute_leds, s_this_day);
  
  FastLED.show();

  prev_loop_seconds = loop_seconds;

//  Serial.print(s_this_year);
//  Serial.print(":");
//  Serial.print(s_this_month);
//  Serial.print(":");
//  Serial.print(s_this_day);
//  Serial.print(":");
//  Serial.print(s_this_hour);
//  Serial.print(":");
//  Serial.print(s_this_minute);
//  Serial.print("\n");
}
