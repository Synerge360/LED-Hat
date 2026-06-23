#ifndef LEDPROJECTMATRIX_h
#define LEDPROJECTMATRIX_h

#define PIXPIN 27 //data pin for neopixel
#define NUMPIX 256 //number of pixels in matrix
#define MATRIX_BRIGHTNESS (char)(5) //brightness of matrix 0-255
#define BLINK_TIME 0.08 //1 second //maximum 2 seconds
#define SCROLLING_DELAY 1500000 //delay count for the for-loops for the scrolling

// 144x LEDs on Maximum Brightness (White): ~3.8 A
// 144x LEDs Scaled to 0- 10 (White): ~0.27 A
// 144x LEDs Scaled to 0-25 (White): ~0.56 A
// 144x LEDs Scaled to 0-20 (White): ~0.46 A
// 144x LEDs Scaled to 0-15 (White): ~0.36 A

#include <map>
#include <vector>
#include <cmath>

// try to make it word aligned

#define NUMBITS (24*NUMPIX) //number of bits needed for matrix
#define T0H (uint16_t)(6 | 0x8000) //time high to output bit 0
#define T1H (uint16_t)(14 | 0x8000) //time high to output bit 1
#define BLINK_COUNT (int)(BLINK_TIME/0.000032) //count or ticks to reach BLINK_TIME

#define M_PI 3.14159265358979323846  /* pi */

#define NRF_GPIO_BASE_ADDRESS  (0x50000000UL)
#define NRF_GPIO_DIRSET_OFFSET  (0x518)
#define NRF_GPIO_OUTCLR_OFFSET  (0x50C)

#define NRF_PWM1_BASE_ADDRESS  (0x40021000UL)
#define NRF_PWM_MODE_OFFSET  (0x504)
#define NRF_PWM_PRESCALER_OFFSET  (0x50C)
#define NRF_PWM_COUNTERTOP_OFFSET  (0x508)
#define NRF_PWM_LOOP_OFFSET  (0x514)
#define NRF_PWM_DECODER_OFFSET  (0x510)
#define NRF_PWM_SEQ0_REFRESH_OFFSET  (0x528)
#define NRF_PWM_SEQ0_ENDDELAY_OFFSET  (0x52C)
#define NRF_PWM_PSEL_OUT0_OFFSET  (0x560)
#define NRF_PWM_ENABLE_OFFSET  (0x500)
#define NRF_PWM_SEQ0_PTR_OFFSET  (0x520)
#define NRF_PWM_SEQ0_CNT_OFFSET  (0x524)
#define NRF_PWM_TASKS_SEQSTART0_OFFSET  (0x008)
#define NRF_PWM_EVENTS_SEQEND0_OFFSET  (0x110)
#define NRF_PWM_TASKS_STOP_OFFSET  (0x004)

#define NRF_TIMER0_BASE_ADDRESS  (0x40008000UL)
#define NRF_TIMER_BITMODE_OFFSET  (0x508)
#define NRF_TIMER_PRESCALER_OFFSET  (0x510)
#define NRF_TIMER_CC0_OFFSET  (0x540)
#define NRF_TIMER_INTENSET_OFFSET  (0x304)
#define NRF_TIMER_SHORTS_OFFSET  (0x200)
#define NRF_TIMER_EVENTS_COMPARE0_OFFSET  (0x140)
#define NRF_TIMER_TASKS_START_OFFSET  (0x000)
#define NRF_TIMER_INTENCLR_OFFSET  (0x308)
#define NRF_TIMER_TASKS_STOP_OFFSET  (0x004)
#define NRF_TIMER_TASKS_CLEAR_OFFSET  (0x00C)

extern char matrixLED_colorG;
extern char matrixLED_colorR;
extern char matrixLED_colorB;

extern bool endTextScrolling;

struct color {
  char colorG;
  char colorR;
  char colorB;
};

extern char oscillatingBrightness_currBrightness;
extern char oscillatingBrightness_count;

extern uint16_t neoPixelWavFrm[NUMBITS];
extern uint16_t neoPixelWavFrm_temp[NUMBITS];

void matrixPWM_initialization();
void matrixPWM_run();
void matrixPWM_switchWaveformArray(uint16_t *waveformArray);
uint32_t matrixPWM_getCurrentWaveformArray();
void updateNeoPixelWavFrmArr_allPixelsOff(uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_singlePixel(color desiredColor, char pixelNumber, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_singleRow(color desiredColor, char rowNumber, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_singleColumn(color desiredColor, char columnNumber, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_singleColumnCustomHeight(color desiredColor, char columnNumber, char height, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_switchTwoPixels(char pixelNumber1, char pixelNumber2, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_flipSingleColumn(char columnNumber, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_solidColorMode(color desiredColor, uint16_t *waveformArray);
int scaleValue(int value, int inputMin, int inputMax, int outputMin, int outputMax);
void updateNeoPixelWavFrmArr_leftShift(bool rotate, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_rightShift(bool rotate, uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_blinkColorMode_On(color desiredColor);
void updateNeoPixelWavFrmArr_blinkColorMode_Off();
void updateNeoPixelWavFrmArr_rainbowMode_Init(uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_rainbowMode_Loop(uint16_t *waveformArray);
void updateNeoPixelWavFrmArr_textScrollMode(color desiredColor, char *text, char textLength, uint16_t *waveformArray);

#endif
