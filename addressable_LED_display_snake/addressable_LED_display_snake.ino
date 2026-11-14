#include <FastLED.h> // http://fastled.io/docs/3.1/functions.html
#include <math.h>

#define for_range(counter, lessThan) for (uint8_t counter = 0; counter < lessThan; counter++)

#define WIDTH    10
#define HEIGHT   11
#define BOTTOM   0
#define LEFT     0
#define TOP      (HEIGHT-1)
#define RIGHT    (WIDTH-1)
#define NUM_LEDS (WIDTH*HEIGHT)

#define LED_PIN     2
#define BRIGHTNESS  30 // MAX 38 for 1A for 110 LEDs white, diode on nano USB 5V rated for 1A

#define FPS 30

typedef struct segment_ {
  int8_t x;
  int8_t y;
  CRGB c;
} segment_t;

#define new_segment(_x,_y,_c) (segment_t){.x=_x,.y=_y,.c=_c}

typedef struct snake_ {
  segment_t * segments;
  uint8_t length;
} snake_t;

#define NUM_SEGMENTS 11
segment_t segmentStore[NUM_SEGMENTS];
snake_t snake = (snake_t){.segments = segmentStore, .length = NUM_SEGMENTS};

#define head snake.segments[0]
#define tail snake.segments[snake.length-1]

CRGB leds[NUM_LEDS];
#define xy(x,y) ((x)%2 ? (HEIGHT*((x)+1))-(y)-1 : (y)+(HEIGHT*(x))) // serpentine layout, origin at bottom left
#define led(x,y) leds[xy(x,y)]

CRGB off = CRGB::Black;
CRGB headC = CRGB::White;
CRGB bodyC = CRGB::Red;
#define isLedOn(x,y) led(x,y)!=NULL
#define isLedOff(x,y) led(x,y)==NULL

uint8_t count = 0;

void clearLeds(void) {
  for_range(i, NUM_LEDS) {
    leds[i] = off;
  }
}

void drawSnake(void) {
  segment_t seg;
  for_range(i, snake.length) {
    seg = snake.segments[i];
    if (led(seg.x, seg.y) == off) {
      led(seg.x, seg.y) = seg.c;
    }
  }
}

uint8_t getHue(CRGB * c) {
  return atan2(sqrt(3) * (c->g - c->b), 2 * (c->r - c->g - c->b));
}

int8_t h = 1;
int8_t v = 1;
void moveSnake() {
  segment_t newHead = head;

  if (random(5) == 0) {
    h = random(2) * 2 - 1;
  }
  if (random(5) == 0) {
    v = random(2) * 1 - 1;
  }

  newHead.y += v;
  
  if (newHead.y < 0 || newHead.y > TOP) {  
    newHead.y = (newHead.y - v + HEIGHT) % HEIGHT;
    v *= -1;
  }
  
  newHead.x += h;
  
  if (newHead.x < 0 || newHead.x > RIGHT) {      
    newHead.x = (newHead.x - h + WIDTH) % WIDTH;
    h *= -1;
  }

  segment_t seg;
  for_range(i, snake.length - 1) {
    seg = snake.segments[snake.length - 2 - i];
    snake.segments[snake.length - 1 - i].x = seg.x;
    snake.segments[snake.length - 1 - i].y = seg.y;
  }
  head.x = newHead.x;
  head.y = newHead.y;

//  if (count <= snake.length) {
//    printSnake();
//    Serial.println();
//  }
}

void printSnake() {
  for_range(i, snake.length) {
    Serial.println("Snake " + String(i) + " = (" + String(snake.segments[i].x) + ", " + String(snake.segments[i].y) + ")");
  }
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  randomSeed(analogRead(0));
  FastLED.setBrightness(BRIGHTNESS);
  clearLeds();
  uint8_t x, y;
  for_range(i, snake.length) {
//    y = i / WIDTH;
//    x = (y % 2 == 0) ? i % WIDTH : WIDTH - 1 - i % WIDTH;
    y = HEIGHT / 2;
    x = WIDTH / 2;
    snake.segments[i] = new_segment(x, y, bodyC);
    snake.segments[i].c.setHue((getHue(&bodyC) + (360 * i) / snake.length) % 360);
    snake.segments[i].c.fadeLightBy(255 * i / snake.length);
  }
  head.c = headC;

//  printSnake();
}

void loop() {
  clearLeds();
  moveSnake();
  drawSnake();
  FastLED.show();
//  count++;
//  if (count == snake.length / 2) {
//    printSnake();
//  } else if (count == snake.length) {
//    printSnake();
//    while (1);
//  }
  delay(1000 / FPS);
}
