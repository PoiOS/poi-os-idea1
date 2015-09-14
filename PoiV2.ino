#include "FastLED.h"

#define NUM_LEDS 36
#define DATA_PIN 0
#define CLOCK_PIN 2
#define PROG_SIZE 128

// Instruction set command numbers
#define CMD_SET 0 // requires 3 parameters: colorIndex, start, end
#define CMD_WAIT 1 // requires 1 parameter: delay (in ms)
#define CMD_CLEAR 2 // requires 0 parameters
#define CMD_SHOW 3 // requires 0 parameters
#define CMD_END 4 // requires 0 parameters

CRGB leds[NUM_LEDS];

// A programme is a list of instructions.
// Each instruction is a command byte followed by a 
// list of parameters. The number of parameters varies
// depending on the command.
const PROGMEM  uint8_t defaultProg[] = { 
  19, // First number is length of programme
  CMD_SET, 0, 0, 36,
  CMD_SHOW,
  CMD_WAIT, 1000,
  CMD_CLEAR,
  CMD_SET, 128, 18, 36,
  CMD_SHOW,
  CMD_WAIT, 1000,
  CMD_CLEAR,
  CMD_SHOW,
  CMD_WAIT, 1000
};

CRGBPalette16 currentPalette;
TBlendType currentBlending;
uint8_t currentProg[PROG_SIZE];
uint8_t pc; // programme counter

void setup() {
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS);
  FastLED.clear();

  // If programe has been uploaded to EEPROM use that, here we use default
  // programme stored in PROGMEM and copy to currentProg
  uint8_t iMax = pgm_read_byte_near(defaultProg) + 1;
  for (uint8_t i = 1; i < iMax; ++i) {
    currentProg[i - 1] = pgm_read_byte_near(defaultProg + i);
  }
  // Terminate programme with END command (loop back to beginning)
  currentProg[iMax] = CMD_END;
  currentPalette = PartyColors_p;
  currentBlending = BLEND;
  pc = 0;
}

void setLEDs(uint8_t colorIndex, uint8_t startLED, uint8_t endLED) {
  CRGB col = ColorFromPalette(currentPalette, colorIndex, 255, currentBlending);
  for (uint8_t i = startLED; i < endLED; ++i) {
    leds[i] = col;
  }
}

void loop() {
  switch(currentProg[pc]) {
    case CMD_SET: 
      setLEDs(currentProg[pc + 1], currentProg[pc + 2], currentProg[pc + 3]);
      pc += 4; // jump to next instruction in programme
      break;
    case CMD_WAIT:
      FastLED.delay(currentProg[pc + 1]);
      pc += 2;
      break;
    case CMD_CLEAR:
      FastLED.clear();
      pc++;
      break;
    case CMD_SHOW:
      FastLED.show();
      pc++; 
      break;
    case CMD_END:
      pc = 0;
      break;
  }
  if (pc >= PROG_SIZE) pc = 0;
}




