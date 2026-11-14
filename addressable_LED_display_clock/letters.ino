#include <FastLED.h> // http://fastled.io/docs/3.1/functions.html

typedef struct letter {
  char c;
  uint8_t lines[5];
  uint8_t width;
} letter_t;

#define makeLetter(ch, l1, l2, l3, l4, l5, w) {.c = ch, .lines = {0b##l5, 0b##l4, 0b##l3, 0b##l2, 0b##l1}, .width = w}

letter_t empty_letter = makeLetter('\0', 111, 111, 111, 111, 111, 3);
const letter_t letters[] = {
  makeLetter(' ', 0, 0, 0, 0, 0, 1),
  empty_letter, // !
  empty_letter, // "
  empty_letter, // #
  empty_letter, // $
  empty_letter, // %
  empty_letter, // &
  empty_letter, // '
  empty_letter, // (
  empty_letter, // )
  empty_letter, // *
  makeLetter('+', 000, 010, 111, 010, 000, 3),
  empty_letter, // ,
  empty_letter, // -
  empty_letter, // .
  empty_letter, // /
//  makeLetter('0', 010, 101, 101, 101, 010, 3),
//  makeLetter('1', 010, 010, 010, 010, 010, 3),
//  makeLetter('2', 110, 001, 010, 100, 111, 3),
//  makeLetter('3', 111, 001, 110, 001, 111, 3),
//  makeLetter('4', 101, 101, 111, 001, 001, 3),
//  makeLetter('5', 111, 100, 110, 001, 110, 3),
//  makeLetter('6', 011, 100, 110, 101, 010, 3),
//  makeLetter('7', 111, 001, 001, 001, 001, 3),
//  makeLetter('8', 010, 101, 010, 101, 111, 3),
//  makeLetter('9', 010, 101, 011, 001, 110, 3), 
// --- dice numbers
  makeLetter('0', 000, 000, 000, 000, 000, 3),
  makeLetter('1', 000, 000, 010, 000, 000, 3),
  makeLetter('2', 000, 000, 101, 000, 000, 3),
  makeLetter('3', 000, 000, 111, 000, 000, 3),
  makeLetter('4', 000, 101, 000, 101, 000, 3),
  makeLetter('5', 000, 101, 010, 101, 000, 3),
  makeLetter('6', 000, 101, 101, 101, 000, 3),
  makeLetter('7', 000, 101, 111, 101, 000, 3),
  makeLetter('8', 000, 111, 101, 111, 000, 3),
  makeLetter('9', 000, 111, 111, 111, 000, 3),
// ---
  empty_letter, // :
  empty_letter, // ;
  empty_letter, // <
  empty_letter, // =
  empty_letter, // >
  makeLetter('?', 111, 001, 011, 000, 010, 3),
  empty_letter, // @
  makeLetter('A', 010, 101, 111, 101, 101, 3),
  makeLetter('B', 110, 101, 110, 101, 110, 3),
  makeLetter('C', 011, 100, 100, 100, 011, 3),
  makeLetter('D', 110, 101, 101, 101, 110, 3),
  makeLetter('E', 111, 100, 110, 100, 111, 3),
  makeLetter('F', 111, 100, 110, 100, 100, 3),
  makeLetter('G', 0111, 1000, 1011, 1001, 0111, 4),
  makeLetter('H', 101, 101, 111, 101, 101, 3),
  makeLetter('I', 111, 010, 010, 010, 111, 3),
  makeLetter('J', 011, 001, 001, 101, 011, 3),
  makeLetter('K', 1001, 1010, 1100, 1010, 1001, 4),
  makeLetter('L', 100, 100, 100, 100, 111, 3),
  makeLetter('M', 10001, 11011, 10101, 10001, 10001, 5),
  makeLetter('N', 10001, 11001, 10101, 10011, 10001, 5),
  makeLetter('O', 010, 101, 101, 101, 010, 3),
  makeLetter('P', 110, 101, 110, 100, 100, 3),
  makeLetter('Q', 01110, 10001, 10101, 10010, 01101, 5),
  makeLetter('R', 1110, 1001, 1110, 1010, 1001, 4),
  makeLetter('S', 0111, 1000, 0110, 0001, 1110, 4),
  makeLetter('T', 111, 010, 010, 010, 010, 3),
  makeLetter('U', 101, 101, 101, 101, 111, 3),
  makeLetter('V', 101, 101, 101, 101, 010, 3),
  makeLetter('W', 10001, 10001, 10101, 11011, 10001, 5),
  makeLetter('X', 101, 101, 010, 101, 101, 3),
  makeLetter('Y', 101, 101, 010, 010, 010, 3),
  makeLetter('Z', 11111, 00010, 00100, 01000, 11111, 5)
};
#define getLetter(ch) letters[(ch) - letters[0].c]

void clearLeds(void) {
  for_range(i, NUM_LEDS) {
    leds[i] = off;
  }
}

void drawLetter(char c, int16_t x, int16_t y, CRGB colour) {
  letter_t letter = getLetter(c);
  int16_t drawX, drawY;
  on = colour;
  
  for_range(column, letter.width) {
    for_range(line, 5) {
      if (letter.lines[line] & (1 << column)) {
        drawX = x + letter.width - 1 - column;
        drawY = y + line;

        if (drawX >= LEFT && drawX <= RIGHT && drawY >= BOTTOM && drawY <= TOP) {
          ledOn(drawX, drawY);
        }
      }
    }
  }
}

void drawString(String s, int16_t x, int16_t y, CRGB colour) {
  int16_t currentX = x;
  uint8_t currentIndex = 0;
  char currentChar = s[0];

  while (currentChar != '\0') {
//    Serial.println(currentChar);
    
    drawLetter(currentChar, currentX, y, colour);
    
    currentX += getLetter(currentChar).width + 1;
    
    currentIndex++;
    currentChar = s[currentIndex];
  }
}

uint16_t stringWidth(String s) {
  uint8_t currentIndex = 0;
  char currentChar = s[0];
  letter_t * letter_p;
  uint16_t total = 0;

  while (currentChar != '\0') {
    total += getLetter(currentChar).width + 1;
    currentIndex++;
    currentChar = s[currentIndex];
  }

  return total - 1;
}
