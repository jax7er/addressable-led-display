#include <FastLED.h> // http://fastled.io/docs/3.1/functions.html

#define for_range(counter, lessThan) for (uint8_t counter = 0; counter < lessThan; counter++)

#define WIDTH    10
#define HEIGHT   11
#define BOTTOM   0
#define LEFT     0
#define TOP      (HEIGHT-1)
#define RIGHT    (WIDTH-1)
#define NUM_LEDS (WIDTH*HEIGHT)

#define LED_PIN     2
#define NUM_FLASHES 3
#define FLASH_DELAY 100
#define DROP_DELAY  30
#define BRIGHTNESS  32 // MAX 38 for 1A for 110 LEDs white, diode on nano USB 5V rated for 1A

CRGB leds[NUM_LEDS];
#define xy(x,y) ((x)%2 ? (HEIGHT*((x)+1))-(y)-1 : (y)+(HEIGHT*(x))) // serpentine layout, origin at bottom left
#define led(x,y) leds[xy(x,y)]

const CRGB colours[] = {
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Cyan,
  CRGB::Blue,
  CRGB::Purple
};
#define numColours (sizeof(colours) / sizeof(colours[0]))
#define randomColour() colours[random(numColours)]
CRGB on = CRGB(10, 0, 0);
CRGB off = CRGB::Black;
#define ledOn(x,y) led(x,y)=on
#define ledOff(x,y) led(x,y)=off
#define isLedOn(x,y) led(x,y)!=off
#define isLedOff(x,y) led(x,y)==off

volatile uint8_t activePosX = 0;
volatile uint8_t activePosY = 0;

// fades BOTTOM row
void removeBottomRow(void) {
//  Serial.println("removing bottom row");

  CRGB temp[WIDTH];
  for_range(x, WIDTH) {
    temp[x] = led(x, BOTTOM);
  }

  CRGB current;
  for_range(i, 20) {
    for_range(x, WIDTH) {
      current = temp[x];
      current.fadeLightBy((255 * (i + 1)) / 20);
      led(x, BOTTOM) = current;
    }
    FastLED.show();
    delay(25);
  }
  
//  // show and hide the BOTTOM row NUM_FLASHES number of times
//  for_range(i, NUM_FLASHES) {
//    for_range(x, WIDTH) {
//      ledOff(x, BOTTOM);
//    }
//    FastLED.show();
//    delay(FLASH_DELAY);
//    
//    for_range(x, WIDTH) {
//      led(x, BOTTOM) = temp[x];
//    }
//    FastLED.show();
//    delay(FLASH_DELAY);
//  }

  // shifts grid down by 1 by removing the highest lit LED in each column
  for_range(x, WIDTH) {
    for_range(y, TOP) {            
      led(x, y) = led(x, y + 1);
    }
    ledOff(x, TOP);
  }

  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  randomSeed(analogRead(0));
  for_range(i, NUM_LEDS) {
    leds[i] = off;
  }
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  on = randomColour();
  
  // calculate new free starting x position for next brick
  activePosY = TOP;  
  do {
    activePosX = random(WIDTH);
  } while (isLedOn(activePosX, activePosY));
  
  ledOn(activePosX, activePosY);
  FastLED.show();
  
//  Serial.println("start pos = " + String(activePosX) + ", " + String(activePosY));
  
  // animate new brick falling until it hits another brick or the BOTTOM
  while (!(isLedOn(activePosX, activePosY - 1) || activePosY == BOTTOM)) {
    // move current brick down one row and continue if not fallen
    ledOff(activePosX, activePosY);
    activePosY--;
    ledOn(activePosX, activePosY);

    FastLED.show();
    
//    Serial.println("new pos = " + String(activePosX) + ", " + String(activePosY));
    
    delay(DROP_DELAY);
  }

//  Serial.println("final pos = " + String(activePosX) + ", " + String(activePosY));

  // check if bottom row is full
  for_range(x, WIDTH) {
    if (isLedOff(x, BOTTOM)) {
      break;
    } else if (x == RIGHT) {
      removeBottomRow();
    }
  }
}
