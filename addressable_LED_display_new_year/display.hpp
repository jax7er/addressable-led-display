#include <FastLED.h>  // http://fastled.io/docs/3.1/functions.html

#define for_range(counter, size) for (int counter = 0; counter < (size); ++counter)

namespace Display {
const int width = 10;
const int height = 11;
const int bottom = 0;
const int left = 0;
const int top = height - 1;
const int right = width - 1;
const int num_leds = width * height;

const int led_pin = 2;
const int brightness = 16;  // MAX 38 for 1A for 110 LEDs white, diode on nano USB 5V rated for 1A

CRGB leds[num_leds];
CRGB on_state = CRGB::White;
CRGB off_state = CRGB::Black;
CRGB null_led = off_state;

// always use this for safe LED access
CRGB& led(const int x, const int y) {
  if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) {
    return null_led;
  } else {
    // serpentine layout, origin at bottom left
    return leds[((x % 2) ? (height * (x + 1)) - y - 1 : y + (height * x))];
  }
}

void set_on(const CRGB& colour) {
  on_state = colour;
}
void set_off(const CRGB& colour) {
  off_state = colour;
}
void on(const int x, const int y) {
  led(x, y) = on_state;
}
void off(const int x, const int y) {
  led(x, y) = off_state;
}
bool is_on(const int x, const int y) {
  return led(x, y) != off_state;
}
bool is_off(const int x, const int y) {
  return led(x, y) == off_state;
}

void clear() {
  for_range(i, num_leds) {
    leds[i] = off_state;
  }
}

void setup() {
  FastLED.addLeds<WS2811, led_pin, GRB>(leds, num_leds);
  FastLED.setBrightness(brightness);
}

void show() {
  FastLED.show();
}
}

enum class Line : int {
  // do not modify order
  _____ = 0,
  ____x,
  ___x_,
  ___xx,
  __x__,
  __x_x,
  __xx_,
  __xxx,
  _x___,
  _x__x,
  _x_x_,
  _x_xx,
  _xx__,
  _xx_x,
  _xxx_,
  _xxxx,
  x____,
  x___x,
  x__x_,
  x__xx,
  x_x__,
  x_x_x,
  x_xx_,
  x_xxx,
  xx___,
  xx__x,
  xx_x_,
  xx_xx,
  xxx__,
  xxx_x,
  xxxx_,
  xxxxx,
  // named lines
  none = _____,
  left = x____,
  middle = __x__,
  right = ____x,
  outer = x___x,
  not_left = _xxxx,
  not_middle = xx_xx,
  not_right = xxxx_,
  not_outer = _xxx_,
  all = xxxxx,
};

Line operator|(const Line& l1, const Line& l2) {
  return static_cast<Line>(static_cast<int>(l1) | static_cast<int>(l2));
}

class Letter {
public:
  static const int max_width = 5;

  Letter(const int character, const Line l1, const Line l2, const Line l3, const Line l4, const Line l5) {
    c = character;
    lines[0] = l5;
    lines[1] = l4;
    lines[2] = l3;
    lines[3] = l2;
    lines[4] = l1;
    switch (l1 | l2 | l3 | l4 | l5) {
      case Line::_____: w = 1; break;
      case Line::____x: w = 1; break;
      case Line::___x_: w = 1; break;
      case Line::___xx: w = 2; break;
      case Line::__x__: w = 1; break;
      case Line::__x_x: w = 3; break;
      case Line::__xx_: w = 2; break;
      case Line::__xxx: w = 3; break;
      case Line::_x___: w = 1; break;
      case Line::_x__x: w = 4; break;
      case Line::_x_x_: w = 3; break;
      case Line::_x_xx: w = 4; break;
      case Line::_xx__: w = 2; break;
      case Line::_xx_x: w = 4; break;
      case Line::_xxx_: w = 3; break;
      case Line::_xxxx: w = 4; break;
      case Line::x____: w = 1; break;
      case Line::x___x: w = 5; break;
      case Line::x__x_: w = 4; break;
      case Line::x__xx: w = 5; break;
      case Line::x_x__: w = 3; break;
      case Line::x_x_x: w = 5; break;
      case Line::x_xx_: w = 4; break;
      case Line::x_xxx: w = 5; break;
      case Line::xx___: w = 2; break;
      case Line::xx__x: w = 5; break;
      case Line::xx_x_: w = 4; break;
      case Line::xx_xx: w = 5; break;
      case Line::xxx__: w = 3; break;
      case Line::xxx_x: w = 5; break;
      case Line::xxxx_: w = 4; break;
      case Line::xxxxx: w = 5; break;
      default: w = 0;
    }
  }

  Letter(const int character)
    : Letter(character, Line::none, Line::none, Line::none, Line::none, Line::none) {}

  Letter()
    : Letter('\0') {}

  int character() const {
    return c;
  }

  int width() const {
    return w;
  }

  int draw(const int x, const int y, const CRGB& colour) {
    if (width() > 0) {
      int draw_x = 0;
      int draw_y = 0;
      Display::on_state = colour;

      for_range(column, width()) {
        for_range(line, 5) {
          if (0 != (static_cast<int>(lines[line]) & (1 << column))) {
            draw_x = x + width() - 1 - column;
            draw_y = y + line;

            if ((draw_x >= Display::left) && (draw_x <= Display::right) && (draw_y >= Display::bottom) && (draw_y <= Display::top)) {
              Display::on(draw_x, draw_y);
            }
          }
        }
      }
    }

    return width();
  }

private:
  int c;
  Line lines[max_width];
  int w;
};

#define LetterBin(c, l1, l2, l3, l4, l5) Letter(c, static_cast<Line>(0b##l1), static_cast<Line>(0b##l2), static_cast<Line>(0b##l3), static_cast<Line>(0b##l4), static_cast<Line>(0b##l5))

namespace Letters {
const Letter empty_letter;
const Letter letters[] = {
  Letter(' '),
  LetterBin('!', 010, 010, 010, 000, 010),
  empty_letter,  // "
  empty_letter,  // #
  empty_letter,  // $
  empty_letter,  // %
  empty_letter,  // &
  empty_letter,  // '
  empty_letter,  // (
  empty_letter,  // )
  empty_letter,  // *
  LetterBin('+', 000, 010, 111, 010, 000),
  empty_letter,  // ,
  empty_letter,  // -
  empty_letter,  // .
  empty_letter,  // /
  // --- Arabic numerals
  LetterBin('0', 010, 101, 101, 101, 010),
  LetterBin('1', 010, 010, 010, 010, 010),
  LetterBin('2', 110, 001, 010, 100, 111),
  LetterBin('3', 111, 001, 110, 001, 111),
  LetterBin('4', 101, 101, 111, 001, 001),
  LetterBin('5', 111, 100, 110, 001, 110),
  LetterBin('6', 011, 100, 110, 101, 010),
  LetterBin('7', 111, 001, 001, 001, 001),
  LetterBin('8', 010, 101, 010, 101, 111),
  LetterBin('9', 010, 101, 011, 001, 110),
  // ---
  // --- dice numbers
  // LetterBin('0', 000, 000, 000, 000, 000),
  // LetterBin('1', 000, 000, 010, 000, 000),
  // LetterBin('2', 000, 000, 101, 000, 000),
  // LetterBin('3', 000, 000, 111, 000, 000),
  // LetterBin('4', 000, 101, 000, 101, 000),
  // LetterBin('5', 000, 101, 010, 101, 000),
  // LetterBin('6', 000, 101, 101, 101, 000),
  // LetterBin('7', 000, 101, 111, 101, 000),
  // LetterBin('8', 000, 111, 101, 111, 000),
  // LetterBin('9', 000, 111, 111, 111, 000),
  // ---
  empty_letter,  // :
  empty_letter,  // ;
  empty_letter,  // <
  empty_letter,  // =
  empty_letter,  // >
  LetterBin('?', 111, 001, 011, 000, 010),
  empty_letter,  // @
  LetterBin('A', 010, 101, 111, 101, 101),
  LetterBin('B', 110, 101, 110, 101, 110),
  LetterBin('C', 011, 100, 100, 100, 011),
  LetterBin('D', 110, 101, 101, 101, 110),
  LetterBin('E', 111, 100, 110, 100, 111),
  LetterBin('F', 111, 100, 110, 100, 100),
  LetterBin('G', 0111, 1000, 1011, 1001, 0111),
  LetterBin('H', 101, 101, 111, 101, 101),
  LetterBin('I', 111, 010, 010, 010, 111),
  LetterBin('J', 011, 001, 001, 101, 011),
  LetterBin('K', 1001, 1010, 1100, 1010, 1001),
  LetterBin('L', 100, 100, 100, 100, 111),
  LetterBin('M', 10001, 11011, 10101, 10001, 10001),
  LetterBin('N', 10001, 11001, 10101, 10011, 10001),
  LetterBin('O', 010, 101, 101, 101, 010),
  LetterBin('P', 110, 101, 110, 100, 100),
  LetterBin('Q', 01110, 10001, 10101, 10010, 01101),
  LetterBin('R', 1110, 1001, 1110, 1010, 1001),
  LetterBin('S', 0111, 1000, 0110, 0001, 1110),
  LetterBin('T', 111, 010, 010, 010, 010),
  LetterBin('U', 101, 101, 101, 101, 111),
  LetterBin('V', 101, 101, 101, 101, 010),
  LetterBin('W', 10001, 10001, 10101, 11011, 10001),
  LetterBin('X', 101, 101, 010, 101, 101),
  LetterBin('Y', 101, 101, 010, 010, 010),
  LetterBin('Z', 11111, 00010, 00100, 01000, 11111)
};
const int size = sizeof(letters) / sizeof(Letter);

Letter get(const int character) {
  const int index = character - letters[0].character();
  return (index < size) ? letters[index] : empty_letter;
}

int string_width(String s) {
  int current_index = 0;
  Letter current_letter = Letters::get(s[current_index]);
  int total = 0;

  while (current_letter.character() != '\0') {
    if (current_letter.width() > 0) {
      total += current_letter.width() + 1;
    }
    ++current_index;
    current_letter = Letters::get(s[current_index]);
  }

  return total - 1;
}

int draw(const int c, const int x, const int y, const CRGB& colour) {
  return get(c).draw(x, y, colour);
}

void draw_string(const String& s, const int x, const int y, const CRGB& colour) {
  int current_x = x;
  int current_index = 0;
  int current_char = s[current_index];
  int char_width = 0;

  while (current_char != '\0') {
    char_width = draw(current_char, current_x, y, colour);

    if (char_width > 0) {
      current_x += char_width + 1;
    }

    ++current_index;
    current_char = s[current_index];
  }
}
}