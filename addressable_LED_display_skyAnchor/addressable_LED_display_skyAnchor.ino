#include <FastLED.h> // http://fastled.io/docs/3.1/functions.html

#define for_range(i, count) for (size_t i = 0; i < count; ++i)

#define WIDTH    10
#define HEIGHT   11
#define BOTTOM   0
#define LEFT     0
#define TOP      (HEIGHT - 1)
#define RIGHT    (WIDTH - 1)
#define NUM_LEDS (WIDTH * HEIGHT)

#define TOP_ROW    6
#define BOTTOM_ROW 0

#define LED_PIN     2
#define BRIGHTNESS  30 // MAX 38 for 1A for 110 LEDs white, diode on nano USB 5V rated for 1A

#define FPS 10

typedef struct
{
  uint8_t lines[5];
  uint8_t width;
}
letter_t;

#define makeLetter(l1, l2, l3, l4, l5, w) (letter_t){.lines = {0b##l1, 0b##l2, 0b##l3, 0b##l4, 0b##l5}, .width = w}

const letter_t letters[] = {
  makeLetter(101, 101, 111, 101, 010, 3), // A
  makeLetter(110, 101, 110, 101, 110, 3), // B
  makeLetter(011, 100, 100, 100, 011, 3), // C
  makeLetter(110, 101, 101, 101, 110, 3), // D
  makeLetter(111, 100, 110, 100, 111, 3), // E
  makeLetter(100, 100, 110, 100, 111, 3), // F
  makeLetter(0111, 1001, 1011, 1000, 0111, 4), // G
  makeLetter(101, 101, 111, 101, 101, 3), // H
  makeLetter(111, 010, 010, 010, 111, 3), // I
  makeLetter(011, 101, 001, 001, 011, 3), // J
  makeLetter(1001, 1010, 1100, 1010, 1001, 4), // K
  makeLetter(111, 100, 100, 100, 100, 3), // L
  makeLetter(10001, 10001, 10101, 11011, 10001, 5), // M
  makeLetter(10001, 10011, 10101, 11001, 10001, 5), // N
  makeLetter(010, 101, 101, 101, 010, 3), // O
  makeLetter(100, 100, 110, 101, 110, 3), // P
  makeLetter(01101, 10010, 10101, 10001, 01110, 5), // Q
  makeLetter(1001, 1010, 1110, 1001, 1110, 4), // R
  makeLetter(1110, 0001, 0110, 1000, 0111, 4), // S
  makeLetter(010, 010, 010, 010, 111, 3), // T
  makeLetter(111, 101, 101, 101, 101, 3), // U
  makeLetter(010, 101, 101, 101, 101, 3), // V
  makeLetter(10001, 11011, 10101, 10001, 10001, 5), // W
  makeLetter(101, 101, 010, 101, 101, 3), // X
  makeLetter(010, 010, 010, 101, 101, 3), // Y
  makeLetter(11111, 01000, 00100, 00010, 11111, 5), // Z
  makeLetter(0, 0, 0, 0, 0, 1), // SPACE
  makeLetter(000, 010, 111, 010, 000, 3), // +
  makeLetter(010, 000, 011, 001, 111, 3) // ?
};

#define get_symbol(c) (((c) == ' ') ? letters[26] : (((c) == '+') ? letters[27] : letters[28]))
#define get_letter(c) (((c) >= 'a') && ((c) <= 'z') ? letters[(c) - 'a'] : get_symbol(c))

CRGB leds[NUM_LEDS];
#define xy(x, y) ((x) % 2 ? (HEIGHT * ((x) + 1)) - (y) - 1 : (y) + (HEIGHT * (x))) // serpentine layout, origin at bottom left
#define led(x, y) (leds[xy(x, y)])

const CRGB blue = CRGB(24, 58, 87);  // #183A57
const CRGB pink = CRGB(255, 0, 153);  // #FF0099
CRGB off = CRGB::Black;
#define led_off(x, y) (led(x, y) = off)
#define is_led_off(x, y) (led(x, y) == off)
#define is_led_on(x, y) (! (is_led_off(x, y)))

void clear_leds(void)
{
  for_range(i, NUM_LEDS)
  {
    leds[i] = off;
  }
}

void draw_letter(char c, int16_t x, int16_t y, CRGB colour)
{
  letter_t letter = get_letter(c);
  int16_t draw_x = 0;
  int16_t draw_y = 0;
  
  for_range(column, letter.width)
  {
    uint8_t column_mask = 1 << column;
    for_range(line, 5)
    {
      if (letter.lines[line] & column_mask)
      {
        draw_x = x + letter.width - 1 - column;
        draw_y = y + line;

        if ((draw_x >= LEFT) && (draw_x <= RIGHT) && (draw_y >= BOTTOM) && (draw_y <= TOP))
        {
          led(draw_x, draw_y) = colour;
        }
      }
    }
  }
}

void draw_string(String s, int16_t x, int16_t y, CRGB colour)
{
  int16_t current_x = x;
  uint8_t current_index = 0;
  char current_char = s[0];

  while (current_char != '\0')
  {
    Serial.println(current_char);
    
    draw_letter(current_char, current_x, y, colour);
    
    current_x += get_letter(current_char).width + 1;
    
    ++current_index;
    current_char = s[current_index];
  }
}

uint16_t string_width(String s)
{
  uint8_t current_index = 0;
  char current_char = s[0];
  letter_t * letter_p;
  uint16_t total = 0;

  while (current_char != '\0')
  {
    total += get_letter(current_char).width + 1;
    ++current_index;
    current_char = s[current_index];
  }

  return total - 1;
}

const String top_string = "sky";
const String bottom_string = "anchor";
const int16_t top_string_width = string_width(top_string);
const int16_t bottom_string_width = string_width(bottom_string);

int16_t active_pos_x_top = 0;
int16_t active_pos_x_bottom = 0;

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  randomSeed(analogRead(0));
  FastLED.setBrightness(BRIGHTNESS);
  
  active_pos_x_top = WIDTH;  
  active_pos_x_bottom = WIDTH;  
}

void loop()
{
  clear_leds();

  draw_string(top_string, active_pos_x_top, TOP_ROW, blue);
  draw_string(bottom_string, active_pos_x_bottom, BOTTOM_ROW, pink);

  --active_pos_x_top;
  --active_pos_x_bottom;

  if (active_pos_x_top < -top_string_width)
  {
    active_pos_x_top = WIDTH;
  }
  if (active_pos_x_bottom < -bottom_string_width)
  {
    active_pos_x_bottom = WIDTH;
  }
  
  FastLED.show();
  
  delay(1000 / FPS);
}
