#include <FastLED.h> // http://fastled.io/docs/3.1/functions.html

#define for_range(counter, lessThan) for (uint8_t counter = 0; counter < lessThan; counter++)

#define WIDTH    10
#define HEIGHT   11
#define BOTTOM   0
#define LEFT     0
#define TOP      (HEIGHT-1)
#define RIGHT    (WIDTH-1)
#define NUM_LEDS (WIDTH*HEIGHT)

#define TOP_ROW    6
#define BOTTOM_ROW 0

#define LED_PIN     2
#define BRIGHTNESS  30 // MAX 38 for 1A for 110 LEDs white, diode on nano USB 5V rated for 1A

#define FPS 5

typedef struct letter {
  uint8_t lines[5];
  uint8_t width;
} letter_t;

#define makeLetter(l1,l2,l3,l4,l5,w) (letter_t){.lines={0b##l1,0b##l2,0b##l3,0b##l4,0b##l5},.width=w}

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

#define A (letters[0])
#define C (letters[1])
#define J (letters[2])
#define K (letters[3])
#define QM (letters[4])
#define getSymbol(c) ((c)==' ' ? letters[26] : ((c)=='+' ? letters[27] : letters[28]))
#define getLetter(c) ((c)>='a'&&(c)<='z' ? letters[(c)-'a'] : getSymbol(c))

const String topString = "wine +";
const String bottomString = "cheese";

CRGB leds[NUM_LEDS];
#define xy(x,y) ((x)%2 ? (HEIGHT*((x)+1))-(y)-1 : (y)+(HEIGHT*(x))) // serpentine layout, origin at bottom left
#define led(x,y) leds[xy(x,y)]

const CRGB vanbrughPurple = CRGB::Purple; //(84, 38, 87);
const CRGB vanbrughTeal = CRGB(90, 134, 133);
const CRGB vanbrughGreen = CRGB(38, 163, 23);
CRGB on = CRGB::Red;
CRGB off = CRGB::Black;
#define ledOn(x,y) led(x,y)=on
#define ledOff(x,y) led(x,y)=off
#define isLedOn(x,y) led(x,y)!=off
#define isLedOff(x,y) led(x,y)==off

volatile int16_t activePosX = 0;
volatile int16_t activePosY = 0;

void clearLeds(void) {
  for_range(i, NUM_LEDS) {
    leds[i] = off;
  }
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  randomSeed(analogRead(0));
  FastLED.setBrightness(BRIGHTNESS);
  activePosY = 3;
  activePosX = WIDTH;
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
    Serial.println(currentChar);
    
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

void loop() {
  activePosX = WIDTH;  
  
  do {
    clearLeds();
  
    drawString("wine", activePosX, TOP_ROW, vanbrughPurple);
    drawString("+", activePosX + stringWidth("wine "), TOP_ROW, vanbrughTeal);
    drawString("cheese", activePosX, BOTTOM_ROW, CRGB::Yellow);
    
    FastLED.show();
    
    delay(1000 / FPS);
  
    activePosX--;
  } while (activePosX > -((int16_t)(stringWidth("wine +"))));

  int8_t wineLevel, wineDir;
  for_range(_, 3) {
    wineLevel = 0;
    wineDir = 1;
    do {
      clearLeds();
      
      on = CRGB::Grey;
      for_range(x, 4) ledOn(3 + x, 0);
      for_range(x, 2) {
        for_range(y, 3) ledOn(4 + x, 1 + y);
      }
      ledOn(3, 4);
      ledOn(6, 4);
      for_range(y, 5) {
        ledOn(2, 5 + y);
        ledOn(7, 5 + y);
      }
  
      on = vanbrughPurple;
      if (wineLevel > 0) {
        ledOn(4, 4);
        ledOn(5, 4);
      }
      for_range(y, wineLevel) {
        for_range(x, 4) {
          ledOn(3 + x, 5 + y);
        }
      }
  
      FastLED.show();
      
      delay(1000 / FPS);
  
      wineLevel += wineDir;
      if (wineLevel == 5) {
        wineDir = -1;
      }
    } while (wineLevel >= 0);
  }

  int8_t slices, sliceDir;
  for_range(_, 3) {
    slices = 0;
    sliceDir = 1;
    do {
      clearLeds();
      
      on = CRGB::Yellow;
      switch (slices) {
        case 4: 
          for_range(y, 5) {
            for_range(x, 2) ledOn(5 + x, 5 + y);
          }
          for_range(y, 4) ledOn(7, 5 + y);
          for_range(y, 2) ledOn(8, 5 + y);
        case 3: 
          for_range(y, 5) {
            for_range(x, 2) ledOn(5 + x, 4 - y);
          }
          for_range(y, 4) ledOn(7, 4 - y);
          for_range(y, 2) ledOn(8, 4 - y);
        case 2:
          for_range(y, 5) {
            for_range(x, 2) ledOn(4 - x, 4 - y);
          }
          for_range(y, 4) ledOn(2, 4 - y);
          for_range(y, 2) ledOn(1, 4 - y);
        case 1:
          for_range(y, 5) {
            for_range(x, 2) ledOn(4 - x, 5 + y);
          }
          for_range(y, 4) ledOn(2, 5 + y);
          for_range(y, 2) ledOn(1, 5 + y);
      }
  
      FastLED.show();
      
      delay(1000 / FPS);

      slices += sliceDir;
      if (slices == 4) {
        sliceDir = -1;
      }
    } while (slices >= 0);
  }
}
