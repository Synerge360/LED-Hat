#include "LEDPROJECT_MATRIX.h"
#include "LEDPROJECT_NVIC.h"

char oscillatingBrightness_count =  0;

// char matrixLED_colorG = 255;
// char matrixLED_colorR = 255;
// char matrixLED_colorB = 255;

uint16_t neoPixelWavFrm[NUMBITS] = {};
uint16_t neoPixelWavFrm_temp[NUMBITS] = {};

void matrixPWM_initialization() {
  *((volatile uint32_t *)((uint8_t *)NRF_GPIO_BASE_ADDRESS + (uint32_t)NRF_GPIO_DIRSET_OFFSET)) = (1 << PIXPIN); //set ref pin as output
  *((volatile uint32_t *)((uint8_t *)NRF_GPIO_BASE_ADDRESS + (uint32_t)NRF_GPIO_OUTCLR_OFFSET)) = (1 << PIXPIN); //set ref pin low
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_MODE_OFFSET)) = 0; //up counter
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_PRESCALER_OFFSET)) = 0; //16MHz ticks
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_COUNTERTOP_OFFSET)) = 20; //1.25 µsec pwm period
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_LOOP_OFFSET)) = 0; //looping disabled
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_DECODER_OFFSET)) = 0; //common, refresh count
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_REFRESH_OFFSET)) = 0;
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_ENDDELAY_OFFSET)) = 0;
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_PSEL_OUT0_OFFSET)) = PIXPIN;
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_ENABLE_OFFSET)) = 1;

  for (unsigned int i = 0; i < NUMBITS; ++i) { //Initialize the matrix to be off
    neoPixelWavFrm[i] = T0H;
  }

  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_PTR_OFFSET)) = (uint32_t)(neoPixelWavFrm);
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_CNT_OFFSET)) = NUMBITS; //one uint16_t for each of the 24 bits per pixel
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_REFRESH_OFFSET)) = 0;
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_ENDDELAY_OFFSET)) = 0;
}

void matrixPWM_run() {
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_EVENTS_SEQEND0_OFFSET)) = 0;

  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_TASKS_SEQSTART0_OFFSET)) = 1;

  while (1) {
    if (*((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_EVENTS_SEQEND0_OFFSET))) break;
  }
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_TASKS_STOP_OFFSET)) = 1;  //drives PIXPIN low as determined by   NRF_GPIO->OUTCLR = (1 << PIXPIN) to latch data for 50µs
}

void matrixPWM_switchWaveformArray(uint16_t *waveformArray) {
  *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_PTR_OFFSET)) = (uint32_t)(waveformArray);
}

uint32_t matrixPWM_getCurrentWaveformArray() {
  return *((volatile uint32_t *)((uint8_t *)NRF_PWM1_BASE_ADDRESS + (uint32_t)NRF_PWM_SEQ0_PTR_OFFSET));
}

void updateNeoPixelWavFrmArr_allPixelsOff(uint16_t *waveformArray) {
  for (unsigned int i = 0; i < NUMBITS; ++i) {
    waveformArray[i] = T0H;
  }
}

void updateNeoPixelWavFrmArr_singlePixel(color desiredColor, char pixelNumber, uint16_t *waveformArray) {
  if ((pixelNumber <= 55) || (200 <= pixelNumber)) {
    return;
  }
  for (unsigned char i = 0; i < 8; ++i) {
    if (desiredColor.colorG & (0x80 >> i)) {
      waveformArray[i + pixelNumber * 24] = T1H;
    } else {
      waveformArray[i + pixelNumber * 24] = T0H;
    }
  }
  for (unsigned char i = 0; i < 8; ++i) {
    if (desiredColor.colorR & (0x80 >> i)) {
      waveformArray[i + pixelNumber * 24 + 8] = T1H;
    } else {
      waveformArray[i + pixelNumber * 24 + 8] = T0H;
    }
  }
  for (unsigned char i = 0; i < 8; ++i) {
    if (desiredColor.colorB & (0x80 >> i)) {
      waveformArray[i + pixelNumber * 24 + 16] = T1H;
    } else {
      waveformArray[i + pixelNumber * 24 + 16] = T0H;
    }
  }
}

void updateNeoPixelWavFrmArr_singleRow(color desiredColor, char rowNumber, uint16_t *waveformArray) {
  if ((rowNumber < 0) || (7 < rowNumber)) {
    return;
  }
  for (unsigned int i = rowNumber + 56; i < rowNumber + 200; i += 16) {
    updateNeoPixelWavFrmArr_singlePixel(desiredColor, i, waveformArray);
  }
  for (unsigned int i = rowNumber + 56 + 15 - (rowNumber * 2); i < rowNumber + 200; i += 16) {
    updateNeoPixelWavFrmArr_singlePixel(desiredColor, i, waveformArray);
  }
}

void updateNeoPixelWavFrmArr_singleColumn(color desiredColor, char columnNumber, uint16_t *waveformArray) {
  if ((columnNumber <= 6) || (25 <= columnNumber)) {
    return;
  }
  for (unsigned int i = columnNumber * 8; i < (columnNumber + 1) * 8; ++i) {
    updateNeoPixelWavFrmArr_singlePixel(desiredColor, i, waveformArray);
  }
}

void updateNeoPixelWavFrmArr_singleColumnCustomHeight(color desiredColor, char columnNumber, char height, uint16_t *waveformArray) {
  if ((columnNumber <= 6) || (25 <= columnNumber)) {
    return;
  }
  if ((height < 0) || (height > 8)) {
    return;
  }

  color off;
  off.colorG = 0;
  off.colorR = 0;
  off.colorB = 0;

  if (columnNumber % 2 == 0) {
    char startingPixel = columnNumber * 8;
    for (char i = 0; i < height; ++i) {
      updateNeoPixelWavFrmArr_singlePixel(desiredColor, startingPixel + i, waveformArray);
    }
    for (char i = height; i < 8; ++i) {
      updateNeoPixelWavFrmArr_singlePixel(off, startingPixel + i, waveformArray);
    }
  } else {
    char startingPixel = ((columnNumber + 1) * 8) - 1;
    for (char i = 0; i < height; ++i) {
      updateNeoPixelWavFrmArr_singlePixel(desiredColor, startingPixel - i, waveformArray);
    }
    for (char i = height; i < 8; ++i) {
      updateNeoPixelWavFrmArr_singlePixel(off, startingPixel - i, waveformArray);
    }
  }
}

void updateNeoPixelWavFrmArr_switchTwoPixels(char pixelNumber1, char pixelNumber2, uint16_t *waveformArray) {
  if ((pixelNumber1 <= 55) || (200 <= pixelNumber1)) {
    return;
  }
  if ((pixelNumber2 <= 55) || (200 <= pixelNumber2)) {
    return;
  }

  uint16_t tempArray[24];
  for (unsigned char i = 0; i < 24; ++i) {
    tempArray[i] = waveformArray[i + pixelNumber1 * 24];
  }
  for (unsigned char i = 0; i < 24; ++i) {
    waveformArray[i + pixelNumber1 * 24] = waveformArray[i + pixelNumber2 * 24];
  }
  for (unsigned char i = 0; i < 24; ++i) {
    waveformArray[i + pixelNumber2 * 24] = tempArray[i];
  }
}

void updateNeoPixelWavFrmArr_flipSingleColumn(char columnNumber, uint16_t *waveformArray) {
  if ((columnNumber <= 6) || (25 <= columnNumber)) {
    return;
  }
  char startingPixel = columnNumber * 8;
  char endingPixel = ((columnNumber + 1) * 8) - 1;
  for (char i = 0; i < 4; ++i) {
    updateNeoPixelWavFrmArr_switchTwoPixels(startingPixel + i, endingPixel - i, waveformArray);
  }
}

void updateNeoPixelWavFrmArr_solidColorMode(color desiredColor, uint16_t *waveformArray) {
  for (unsigned int i = 0; i < 7 * NUMBITS / 32; ++i) {
    waveformArray[i] = T0H;
  }

  for (unsigned int i = 7 * NUMBITS / 32; i < 25 * NUMBITS / 32; i += 24) {
    for (unsigned char j = 0; j < 8; ++j) {
      if (desiredColor.colorG & (0x80 >> j)) {
        waveformArray[j + i] = T1H;
      } else {
        waveformArray[j + i] = T0H;
      }
    }
    for (unsigned char j = 0; j < 8; ++j) {
      if (desiredColor.colorR & (0x80 >> j)) {
        waveformArray[j + i + 8] = T1H;
      } else {
        waveformArray[j + i + 8] = T0H;
      }
    }
    for (unsigned char j = 0; j < 8; ++j) {
      if (desiredColor.colorB & (0x80 >> j)) {
        waveformArray[j + i + 16] = T1H;
      } else {
        waveformArray[j + i + 16] = T0H;
      }
    }
  }

  for (unsigned int i = 25 * NUMBITS / 32; i < NUMBITS; ++i) {
    waveformArray[i] = T0H;
  }
}

int scaleValue(int value, int inputMin, int inputMax, int outputMin, int outputMax) {
  return (value - inputMin) * (outputMax - outputMin) / (inputMax - inputMin) + outputMin; // Linear interpolation formula
}

void updateNeoPixelWavFrmArr_leftShift(bool rotate, uint16_t *waveformArray) {
  uint16_t tempArr[8 * 24];
  if (rotate) {
    for (unsigned int i = 56 * 24; i < (8 * 24) + (56 * 24); ++i) {
      tempArr[i - (56 * 24)] = waveformArray[i];
    }
  }

  for (unsigned int i = (8 * 24) + (56 * 24); i < 4800; ++i) {
    waveformArray[i - (8 * 24)] = waveformArray[i];
  }

  if (rotate) {
    for (unsigned int i = 0; i < 8 * 24; ++i) {
      waveformArray[i + (32 - 7 - 1) * 24 * 8] = tempArr[i];
    }
  } else {
    for (unsigned int i = 0; i < 8 * 24; ++i) {
      waveformArray[i + (32 - 7 - 1) * 24 * 8] = T0H;
    }
  }

  for (char i = 7; i < 25; ++i) {
    updateNeoPixelWavFrmArr_flipSingleColumn(i, waveformArray);
  }
}

void updateNeoPixelWavFrmArr_rightShift(bool rotate, uint16_t *waveformArray) {
  uint16_t tempArr[8 * 24];
  // Copies the last column
  if (rotate) {
    for (unsigned int i = 24 * 8 * 24; i < (8 * 24) + (24 * 8 * 24); ++i) {
      tempArr[i - (24 * 8 * 24)] = waveformArray[i];
    }
  }

  for (unsigned int i = (200 * 24) - 1; i > (56 * 24) - 1; --i) {
    waveformArray[i] = waveformArray[i - (8 * 24)];
  }

  // Writes the last column in the spot of the first column
  if (rotate) {
    for (unsigned int i = 0; i < 8 * 24; ++i) {
      waveformArray[i + 7 * 24 * 8] = tempArr[i];
    }
  } else {
    for (unsigned int i = 0; i < 8 * 24; ++i) {
      waveformArray[i + 7 * 24 * 8] = T0H;
    }
  }

  for (char i = 7; i < 25; ++i) {
    updateNeoPixelWavFrmArr_flipSingleColumn(i, waveformArray);
  }
}

void updateNeoPixelWavFrmArr_blinkColorMode_On(color desiredColor) {
  updateNeoPixelWavFrmArr_solidColorMode(desiredColor, neoPixelWavFrm); // G R B
  matrixPWM_run();

  // 16-bit timer
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_BITMODE_OFFSET)) = 0;

  // 0.000032 s timer period (slowest possible)
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_PRESCALER_OFFSET)) = 9;

  // 2 s compare value, generates EVENTS_COMPARE[0]
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_CC0_OFFSET)) = BLINK_COUNT;

  // Enable IRQ on EVENTS_COMPARE[0]
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_INTENSET_OFFSET)) |= (1UL << 16);

  // Clear the timer when COMPARE0 event is triggered
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_SHORTS_OFFSET)) |= (1UL << 0);

  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) = 0;
  NVIC_ClearPendingIRQ_Call(TIMER0_IRQn_Type);
  NVIC_EnableIRQ_Call(TIMER0_IRQn_Type);
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_TASKS_START_OFFSET)) = 1;
}

void TIMER0_IRQHandler(void) {
  NVIC_ClearPendingIRQ_Call(TIMER0_IRQn_Type);
  volatile uint32_t dummy;
  if (*((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) == 1) {
    color tempColor;
    tempColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, (char)(fabs(cos(oscillatingBrightness_count*0.05*M_PI)) * (double)MATRIX_BRIGHTNESS));
    tempColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, (char)(fabs(cos(oscillatingBrightness_count*0.05*M_PI)) * (double)MATRIX_BRIGHTNESS));
    tempColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, (char)(fabs(cos(oscillatingBrightness_count*0.05*M_PI)) * (double)MATRIX_BRIGHTNESS));
    oscillatingBrightness_count = (oscillatingBrightness_count + 1) % 20;
    while(oscillatingBrightness_count == 9 || oscillatingBrightness_count == 11 || oscillatingBrightness_count == 8 || oscillatingBrightness_count == 12){
      ++oscillatingBrightness_count;
    }
    updateNeoPixelWavFrmArr_solidColorMode(tempColor, neoPixelWavFrm); // G R B

    matrixPWM_run();

    *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) = 0;

    // Read back event register so ensure we have cleared it before exiting IRQ handler.
    dummy = *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET));
    dummy; // to get rid of set but not used warning
  }
}

void updateNeoPixelWavFrmArr_blinkColorMode_Off() {
  matrixPWM_switchWaveformArray(neoPixelWavFrm);
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_INTENCLR_OFFSET)) |= (1UL << 16);
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_TASKS_STOP_OFFSET)) = 1; // Stops the clock signal
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_TASKS_CLEAR_OFFSET)) = 1; // Clear the count
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER0_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) = 0;
  matrixPWM_run();
}

void updateNeoPixelWavFrmArr_rainbowMode_Init(uint16_t *waveformArray) {
  color red;
  red.colorG = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
  red.colorR = (char)scaleValue(255, 0, 255, 0, MATRIX_BRIGHTNESS);
  red.colorB = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);

  color orange;
  orange.colorG = (char)scaleValue(83, 0, 255, 0, MATRIX_BRIGHTNESS);
  orange.colorR = (char)scaleValue(229, 0, 255, 0, MATRIX_BRIGHTNESS);
  orange.colorB = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);

  color yellow;
  yellow.colorG = (char)scaleValue(240, 0, 255, 0, MATRIX_BRIGHTNESS);
  yellow.colorR = (char)scaleValue(250, 0, 255, 0, MATRIX_BRIGHTNESS);
  yellow.colorB = (char)scaleValue(31, 0, 255, 0, MATRIX_BRIGHTNESS);

  color green;
  green.colorG = (char)scaleValue(255, 0, 255, 0, MATRIX_BRIGHTNESS);
  green.colorR = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
  green.colorB = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);

  color blue;
  blue.colorG = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
  blue.colorR = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
  blue.colorB = (char)scaleValue(255, 0, 255, 0, MATRIX_BRIGHTNESS);

  color violet;
  violet.colorG = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
  violet.colorR = (char)scaleValue(148, 0, 255, 0, MATRIX_BRIGHTNESS);
  violet.colorB = (char)scaleValue(211, 0, 255, 0, MATRIX_BRIGHTNESS);

  updateNeoPixelWavFrmArr_singleColumn(red, 7, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(red, 8, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(red, 9, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(orange, 10, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(orange, 11, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(orange, 12, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(yellow, 13, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(yellow, 14, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(yellow, 15, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(green, 16, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(green, 17, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(green, 18, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(blue, 19, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(blue, 20, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(blue, 21, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(violet, 22, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(violet, 23, waveformArray);
  updateNeoPixelWavFrmArr_singleColumn(violet, 24, waveformArray);
}

void updateNeoPixelWavFrmArr_rainbowMode_Loop(uint16_t *waveformArray) {
  updateNeoPixelWavFrmArr_leftShift(true, waveformArray);
}

void updateNeoPixelWavFrmArr_textScrollMode(color desiredColor, char *text, char textLength, uint16_t *waveformArray) {
  std::map<char, std::vector<std::vector<char>>> lettersMap;
  lettersMap['A'] = {{57, 58, 59, 60, 61, 62}, {56, 59}, {56, 59}, {56, 59}, {57, 58, 59, 60, 61, 62}};
  lettersMap['B'] = {{56, 57, 58, 59, 60, 61, 62}, {56, 59, 62}, {56, 59, 62}, {56, 59, 62}, {57, 58, 60, 61}};
  lettersMap['C'] = {{57, 58, 59, 60, 61}, {56, 62}, {56, 62}, {56, 62}, {57, 61}};
  lettersMap['D'] = {{56, 57, 58, 59, 60, 61, 62}, {56, 62}, {56, 62}, {56, 62}, {57, 58, 59, 60, 61}};
  lettersMap['E'] = {{56, 57, 58, 59, 60, 61, 62}, {56, 59, 62}, {56, 59, 62}, {56, 59, 62}, {56, 62}};
  lettersMap['F'] = {{56, 57, 58, 59, 60, 61, 62}, {56, 59}, {56, 59}, {56, 59}, {56}};
  lettersMap['G'] = {{57, 58, 59, 60, 61}, {56, 62}, {56, 59, 62}, {56, 59, 62}, {57, 59, 60, 61}};
  lettersMap['H'] = {{56, 57, 58, 59, 60, 61, 62}, {59}, {59}, {59}, {56, 57, 58, 59, 60, 61, 62}};
  lettersMap['I'] = {{56, 62}, {56, 57, 58, 59, 60, 61, 62}, {56, 62}};
  lettersMap['J'] = {{61}, {62}, {62}, {56, 57, 58, 59, 60, 61}};
  lettersMap['K'] = {{56, 57, 58, 59, 60, 61, 62}, {59}, {59}, {58, 60}, {56, 57, 61, 62}};
  lettersMap['L'] = {{56, 57, 58, 59, 60, 61, 62}, {62}, {62}, {62}, {62}};
  lettersMap['M'] = {{56, 57, 58, 59, 60, 61, 62}, {57}, {58}, {57}, {56, 57, 58, 59, 60, 61, 62}};
  lettersMap['N'] = {{56, 57, 58, 59, 60, 61, 62}, {58}, {59}, {60}, {56, 57, 58, 59, 60, 61, 62}};
  lettersMap['O'] = {{57, 58, 59, 60, 61}, {56, 62}, {56, 62}, {56, 62}, {57, 58, 59, 60, 61}};
  lettersMap['P'] = {{56, 57, 58, 59, 60, 61, 62}, {56, 59}, {56, 59}, {56, 59}, {57, 58}};
  lettersMap['Q'] = {{57, 58, 59, 60, 61}, {56, 62}, {56, 60, 62}, {56, 61, 62}, {57, 58, 59, 60, 61, 62}};
  lettersMap['R'] = {{56, 57, 58, 59, 60, 61, 62}, {56, 59}, {56, 59}, {56, 59}, {57, 58, 60, 61, 62}};
  lettersMap['S'] = {{57, 58, 62}, {56, 59, 62}, {56, 59, 62}, {56, 59, 62}, {56, 60, 61}};
  lettersMap['T'] = {{56}, {56}, {56, 57, 58, 59, 60, 61, 62}, {56}, {56}};
  lettersMap['U'] = {{56, 57, 58, 59, 60, 61}, {62}, {62}, {62}, {56, 57, 58, 59, 60, 61}};
  lettersMap['V'] = {{56, 57, 58, 59, 60}, {61}, {62}, {61}, {56, 57, 58, 59, 60}};
  lettersMap['W'] = {{56, 57, 58, 59, 60, 61, 62}, {61}, {60}, {61}, {56, 57, 58, 59, 60, 61, 62}};
  lettersMap['X'] = {{56, 57, 61, 62}, {58, 60}, {59}, {58, 60}, {56, 57, 61, 62}};
  lettersMap['Y'] = {{56, 57}, {58}, {59, 60, 61, 62}, {58}, {56, 57}};
  lettersMap['Z'] = {{56, 61, 62}, {56, 60, 62}, {56, 59, 62}, {56, 58, 62}, {56, 57, 62}};

  lettersMap['a'] = {{61}, {58, 60, 62}, {58, 60, 62}, {58, 60, 62}, {59, 60, 61, 62}};
  lettersMap['b'] = {{56, 57, 58, 59, 60, 61, 62}, {59, 62}, {59, 62}, {59, 62}, {60, 61}};
  lettersMap['c'] = {{59, 60, 61}, {58, 62}, {58, 62}, {58, 62}, {59, 61}};
  lettersMap['d'] = {{60, 61}, {59, 62}, {59, 62}, {59, 62}, {56, 57, 58, 59, 60, 61, 62}};
  lettersMap['e'] = {{59, 60, 61}, {58, 60, 62}, {58, 60, 62}, {58, 60, 62}, {59, 60, 62}};
  lettersMap['f'] = {{59}, {57, 58, 59, 60, 61, 62}, {56, 59}, {56}, {57}};
  lettersMap['g'] = {{59}, {58, 60, 62}, {58, 60, 62}, {58, 60, 62}, {59, 60, 61}};
  lettersMap['h'] = {{56, 57, 58, 59, 60, 61, 62}, {59}, {59}, {59}, {60, 61, 62}};
  lettersMap['i'] = {{58, 62}, {56, 58, 59, 60, 61, 62}, {62}};
  lettersMap['j'] = {{61}, {62}, {58, 62}, {56, 58, 59, 60, 61}};
  lettersMap['k'] = {{56, 57, 58, 59, 60, 61, 62}, {60}, {60}, {59, 61}, {58, 62}};
  lettersMap['l'] = {{56, 62}, {56, 57, 58, 59, 60, 61, 62}, {62}};
  lettersMap['m'] = {{58, 59, 60, 61, 62}, {58}, {58, 59, 60, 61, 62}, {58}, {59, 60, 61, 62}};
  lettersMap['n'] = {{58, 59, 60, 61, 62}, {58}, {58}, {58}, {59, 60, 61, 62}};
  lettersMap['o'] = {{59, 60, 61}, {58, 62}, {58, 62}, {58, 62}, {59, 60, 61}};
  lettersMap['p'] = {{58, 59, 60, 61, 62}, {58, 60}, {58, 60}, {59}};
  lettersMap['q'] = {{59}, {58, 60}, {58, 60}, {58, 59, 60, 61, 62}};
  lettersMap['r'] = {{58, 59, 60, 61, 62}, {59}, {58}, {58}, {58}};
  lettersMap['s'] = {{59, 62}, {58, 60, 62}, {58, 60, 62}, {58, 60, 62}, {58, 61}};
  lettersMap['t'] = {{58}, {56, 57, 58, 59, 60, 61}, {58, 62}, {62}};
  lettersMap['u'] = {{58, 59, 60, 61}, {62}, {62}, {61}, {58, 59, 60, 61, 62}};
  lettersMap['v'] = {{58, 59, 60}, {61}, {62}, {61}, {58, 59, 60}};
  lettersMap['w'] = {{58, 59, 60, 61, 62}, {61}, {60, 61, 62}, {61}, {58, 59, 60, 61, 62}};
  lettersMap['x'] = {{58, 62}, {59, 61}, {60}, {59, 61}, {58, 62}};
  lettersMap['y'] = {{58, 59, 62}, {60, 62}, {60, 62}, {60, 62}, {58, 59, 60, 61}};
  lettersMap['z'] = {{58, 62}, {58, 61, 62}, {58, 60, 62}, {58, 59, 62}, {58, 62}};

  lettersMap['0'] = {{57, 58, 59, 60, 61}, {56, 60, 62}, {56, 59, 62}, {56, 58, 62}, {57, 58, 59, 60, 61}};
  lettersMap['1'] = {{58, 62}, {57, 62}, {56, 57, 58, 59, 60, 61, 62}, {62}, {62}};
  lettersMap['2'] = {{57, 61, 62}, {56, 60, 62}, {56, 59, 62}, {56, 59, 62}, {57, 58, 62}};
  lettersMap['3'] = {{57, 61}, {56, 62}, {56, 59, 62}, {56, 59, 62}, {57, 58, 60, 61}};
  lettersMap['4'] = {{59, 60}, {58, 60}, {57, 60}, {56, 57, 58, 59, 60, 61, 62}, {60}};
  lettersMap['5'] = {{56, 57, 58, 61}, {56, 58, 62}, {56, 58, 62}, {56, 58, 62}, {56, 59, 60, 61}};
  lettersMap['6'] = {{57, 58, 59, 60, 61}, {56, 59, 62}, {56, 59, 62}, {56, 59, 62}, {57, 60, 61}};
  lettersMap['7'] = {{56}, {56}, {56, 59, 60, 61, 62}, {56, 58}, {56, 57}};
  lettersMap['8'] = {{57, 58, 60, 61}, {56, 59, 62}, {56, 59, 62}, {56, 59, 62}, {57, 58, 60, 61}};
  lettersMap['9'] = {{57, 58, 61}, {56, 59, 62}, {56, 59, 62}, {56, 59, 62}, {57, 58, 59, 60, 61}};

  lettersMap['&'] = {{57, 58, 60, 61}, {56, 59, 62}, {57, 58, 60, 62}, {61}, {60, 62}};
  lettersMap['\''] = {{56, 57}};
  lettersMap['*'] = {{57, 59, 61}, {58, 59, 60}, {57, 58, 59, 60, 61}, {58, 59, 60}, {57, 59, 61}};
  lettersMap['+'] = {{59}, {59}, {57, 58, 59, 60, 61}, {59}, {59}};
  lettersMap['-'] = {{59}, {59}, {59}, {59}, {59}};
  lettersMap['='] = {{58, 60}, {58, 60}, {58, 60}};
  lettersMap['.'] = {{62}};
  lettersMap['!'] = {{56, 57, 58, 59, 60, 62}};
  lettersMap['"'] = {{56, 57}, {}, {56, 57}};
  lettersMap['#'] = {{58, 60}, {56, 57, 58, 59, 60, 61, 62}, {58, 60}, {56, 57, 58, 59, 60, 61, 62}, {58, 60}};
  lettersMap['$'] = {{58, 61}, {57, 59, 61}, {56, 57, 58, 59, 60, 61, 62}, {57, 59, 61}, {57, 60}};
  lettersMap['%'] = {{56, 57, 61}, {56, 57, 60}, {59}, {58, 61, 62}, {57, 61, 62}};
  lettersMap['^'] = {{58}, {57}, {56}, {57}, {58}};
  lettersMap[','] = {{63}, {62}};
  lettersMap[':'] = {{58, 60}};
  lettersMap[';'] = {{61}, {58, 60}};
  lettersMap['?'] = {{57}, {56}, {56, 60, 62}, {56, 59}, {57, 58}};
  lettersMap['@'] = {{57, 58, 59, 60, 61}, {56, 62}, {56, 58, 59, 60, 62}, {56, 58, 60, 62}, {57, 58, 59, 62}};
  lettersMap['/'] = {{61}, {60}, {59}, {58}, {57}};
  lettersMap['<'] = {{59}, {58, 60}, {57, 61}, {56, 62}};
  lettersMap['>'] = {{56, 62}, {57, 61}, {58, 60}, {59}};
  lettersMap['|'] = {{56, 57, 58, 59, 60, 61, 62}};
  lettersMap['\\'] = {{57}, {58}, {59}, {60}, {61}};
  lettersMap['['] = {{56, 57, 58, 59, 60, 61, 62}, {56, 62}};
  lettersMap[']'] = {{56, 62}, {56, 57, 58, 59, 60, 61, 62}};
  lettersMap['{'] = {{59}, {57, 58, 60, 61}, {56, 62}};
  lettersMap['}'] = {{56, 62}, {57, 58, 60, 61}, {59}};
  lettersMap['_'] = {{62}, {62}, {62}, {62}, {62}};
  lettersMap[' '] = {{}};
  lettersMap['('] = {{57, 58, 59, 60, 61}, {56, 62}};
  lettersMap[')'] = {{56, 62}, {57, 58, 59, 60, 61}};

  for (char i = 0; i < textLength; ++i) {
    if (lettersMap.find(text[i]) == lettersMap.end()) {
      continue;
    }
    for (char j = 0; j < lettersMap[text[i]].size(); ++j) {
      for (char k = 0; k < lettersMap[text[i]][j].size(); ++k) {
        if(endTextScrolling){
          return;
        }
        updateNeoPixelWavFrmArr_singlePixel(desiredColor, lettersMap[text[i]][j][k], waveformArray);
      }
      matrixPWM_run();
      for (volatile int i = 0; i < SCROLLING_DELAY; ++i);
      updateNeoPixelWavFrmArr_rightShift(false, waveformArray);
    }
    matrixPWM_run();
    for (volatile int i = 0; i < SCROLLING_DELAY; ++i);
    updateNeoPixelWavFrmArr_rightShift(false, waveformArray);
  }
}

extern "C" {
  __attribute__((__used__)) void TIMER0_IRQHandler_v(void)
  {
    TIMER0_IRQHandler();
  }
}