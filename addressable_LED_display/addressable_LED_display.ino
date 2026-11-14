// _____asm_____("nop\n\t"); generates a delay of 62.5ns
// GRB wth MSB first

#include <avr/pgmspace.h>

#define SET_PIN_2_HIGH PORTD|=4
#define SET_PIN_2_LOW PORTD&=251
#define NUM_LEDS 144
#define NUM_BITS_IN_WORD 24

const uint16_t NUM_BITS = NUM_LEDS * NUM_BITS_IN_WORD;

uint8_t grbData[NUM_LEDS][3];

int main(void) {
  init();

  Serial.begin(115200);
  DDRD |= 4;
  SET_PIN_2_LOW;

  while (1) {
    updateData();
    writeData();
    SET_PIN_2_LOW; // set pin 2 low for reset ready for next data update
    delay(250);
  }
}

void updateData() {
  for (int ledIndex = 0; ledIndex < NUM_LEDS; ledIndex++) {
    grbData[ledIndex][0] = ((millis() / 10 + ledIndex) % 256); // green
    grbData[ledIndex][1] = ((millis() / 10 + 85 + ledIndex) % 256); // red
    grbData[ledIndex][2] = ((millis() / 10 + 170 + ledIndex) % 256); // blue
  }
}

void writeData() {
  uint8_t ledCount = 0;
  uint8_t colourCount = 0;
  uint8_t bitCount = 0;

  do {
    do {
      do {
        SET_PIN_2_HIGH; // set pin 2 high
    
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); // wait 375ns (400ns / 62.5ns = 6.4) (~1 cycle for pin set)
        if (grbData[ledCount][colourCount] & (1UL << bitCount)) __asm__("nop\n\t""nop\n\t"); // wait 437.5ns if code 1 (800ns / 62.5ns = 12.8) (~5 cycles for if)
        
        SET_PIN_2_LOW; // set pin 2 low
        
        __asm__("nop\n\t""nop\n\t""nop\n\t"); // wait 375ns (~1 cycle for pin set) (~2 cycles for while)
        if (!(grbData[ledCount][colourCount] & (1UL << bitCount))) __asm__("nop\n\t"); // wait 437.5ns if code 0 (~6 cycles for if)
      } while (++bitCount < 8);
    } while (++colourCount < 3);
  } while (++ledCount < NUM_LEDS);
}
