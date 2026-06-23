#include <LEDPROJECT_NVIC.h>
#include <LEDPROJECT_GPIO.h>
#include <LEDPROJECT_MATRIX.h>
#include <LEDPROJECT_FFT.h>
#include <LEDPROJECT_PDM.h>
#include <LEDPROJECT_BT.h>
#include <LEDPROJECT_ADC.h>

#define NRF_TIMER2_BASE_ADDRESS  (0x4000A000UL)

// Global Variables
char userSelectedMode;
color userSelectedColor;
char matrixLED_colorG;
char matrixLED_colorR;
char matrixLED_colorB;
bool modeUpdated;
bool colorUpdated;
char userTypedMessageLength;
char userTypedMessage[50];
bool endTextScrolling;
bool lowBattery;

int k;
int colorRed, colorGreen, colorBlue;
bool RtoG;
bool GtoB;
bool BtoR;

volatile int analogResult = 0;
volatile float voltageResult = 0;
float convert = 1137.77;

char rxbuffer[55]; //RX Buffer

void BTinit() {
  memset(rxbuffer, 0, 55);
  /*Start HF crystal for improved clock tolerance*/
  if (*((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET)) == 0) {
    *((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_TASKS_HFCLKSTART_OFFSET)) = 1;
    while (*((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET)) == 0);
  }

  /*Set up UARTE*/
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)BAUDRATE)) = BAUD9600; //Set 9600 baud rate
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)PSELTXD)) = (0x7FFFFFE3UL); //Select TX and RX pins
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)PSELRXD)) = (0x7FFFFFEAUL);
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)CONFIGURE)) = 0; //Configure UARTE
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)ENABLEUARTE)) = (8UL); //Enable the UART (starts using the TX/RX pins)

  /*Set up RX*/
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)RXDMAXCNT)) = sizeof(rxbuffer); //Configure receive buffer and start reception
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)RXDPTR)) = (uint32_t)rxbuffer;
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)STARTRXD)) = 1;

  *((volatile uint32_t *)(UARTEBASE + (uint32_t)EVENTSENDRX)) = 0;

  /*Set up interrupts*/
  *((volatile uint32_t *)(UARTEBASE + (uint32_t)INTENSETS)) |= (1UL << 4);
  NVIC_ClearPendingIRQ_Call(UARTE0_UART0_IRQn_Type);
  NVIC_EnableIRQ_Call(UARTE0_UART0_IRQn_Type);
}

void UARTE0_UART0_IRQHandler(void) {
  NVIC_ClearPendingIRQ_Call(UARTE0_UART0_IRQn_Type);
  volatile uint32_t dummy;
  if (*((volatile uint32_t *)(UARTEBASE + (uint32_t)EVENTSENDRX)) == 1) {
    if (userSelectedMode == 0b00000100) {
      endTextScrolling = true;
      modeUpdated = true;
    }
    if (userSelectedMode != rxbuffer[0]) {
      userSelectedMode = rxbuffer[0];
      modeUpdated = true;
    }
    char temp_colorG = rxbuffer[2];
    char temp_colorR = rxbuffer[1];
    char temp_colorB = rxbuffer[3];

    if ( temp_colorG != matrixLED_colorG || temp_colorR != matrixLED_colorR || temp_colorB != matrixLED_colorB) {
      matrixLED_colorG = temp_colorG;
      matrixLED_colorR = temp_colorR;
      matrixLED_colorB = temp_colorB;
      colorUpdated = true;
    }

    char textSize = rxbuffer[4];
    if(userSelectedMode == 0b00000100){
      if (textSize != 0) {
        if (textSize > 50) {
          userSelectedMode = 0b00000000;
          *((volatile uint32_t *)(UARTEBASE + (uint32_t)STARTRXD)) = 1;
          *((volatile uint32_t *)(UARTEBASE + (uint32_t)EVENTSENDRX)) = 0;
          return;
        }
        userTypedMessageLength = textSize;
        for (char i = 5; i < textSize + 5; i++) {
          userTypedMessage[i - 5] = rxbuffer[i];
        }
      } else {
        if (userSelectedMode == 0b00000100) {
          userSelectedMode = 0b00000000;
        }
      }
    }
    
    *((volatile uint32_t *)(UARTEBASE + (uint32_t)STARTRXD)) = 1;
    *((volatile uint32_t *)(UARTEBASE + (uint32_t)EVENTSENDRX)) = 0;

    dummy = *((volatile uint32_t *)(UARTEBASE + (uint32_t)EVENTSENDRX));
    dummy;
  }
}

void adcInit() {
  //NRF_CLOCK->TASKS_HFCLKSTART = 1;
  *((volatile uint32_t *)(CLOCKBASE + (uint32_t)HFCLKSTART)) = 1;
  //while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
  while(*((volatile uint32_t *)(CLOCKBASE + (uint32_t)EVENTHFCLKSTART)) == 0);
  //NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  *((volatile uint32_t *)(CLOCKBASE + (uint32_t)EVENTHFCLKSTART)) = 0;

  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCONFIG)) = 0;
  // NRF_SAADC->CH[0].CONFIG = 0; //Single-ended, 1/6 gain, internal reference
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCPSELP)) = (1UL);
  // NRF_SAADC->CH[0].PSELP = (1UL); //AnalogInput0 (A5)
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCPSELN)) = (0UL);
  // NRF_SAADC->CH[0].PSELN = (0UL);
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCRESOLUTION)) = (2UL);
  // NRF_SAADC->RESOLUTION = (2UL); //12 Bit Resolution
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCMAXCNT)) = 1;
  // NRF_SAADC->RESULT.MAXCNT = 1; //Set up buffer
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCPTR)) = (uint32_t)&analogResult;
  // NRF_SAADC->RESULT.PTR = (uint32_t)&analogResult;

  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSAMPLERATE)) = (0UL << 12UL);
  // NRF_SAADC->SAMPLERATE = (0UL << 12UL); //Use sample task rate

  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCENABLE)) = 1;
  // NRF_SAADC->ENABLE = 1; //Enable ADC

  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCTASKCALIBRATE)) = 1;
  // NRF_SAADC->TASKS_CALIBRATEOFFSET = 1; //Callibrate ADC
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCALIBRATEDONE)) == 0);
  // while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCALIBRATEDONE)) = 0;
  // NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTATUS)) == 1);
  // while (NRF_SAADC->STATUS == 1); //Check ADC Status
}

void initializeBatteryReadTimer() {
  // 16-bit timer
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_BITMODE_OFFSET)) = 2;

  // 0.000032 s timer period (slowest possible)
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_PRESCALER_OFFSET)) = 9;

  // 1 min compare value, generates EVENTS_COMPARE[0]
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_CC0_OFFSET)) = 1875000;

  // Enable IRQ on EVENTS_COMPARE[0]
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_INTENSET_OFFSET)) |= (1UL << 16);

  // Clear the timer when COMPARE0 event is triggered
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_SHORTS_OFFSET)) |= (1UL << 0);

  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) = 0;
  NVIC_ClearPendingIRQ_Call(TIMER2_IRQn_Type);
  NVIC_EnableIRQ_Call(TIMER2_IRQn_Type);
  *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_TASKS_START_OFFSET)) = 1;
}

void TIMER2_IRQHandler(void) {
  NVIC_ClearPendingIRQ_Call(TIMER2_IRQn_Type);
  volatile uint32_t dummy;
  if (*((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) == 1) {

    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCTASKCALIBRATE)) = 1;
    // NRF_SAADC->TASKS_CALIBRATEOFFSET = 1; //Callibrate ADC
     while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCALIBRATEDONE)) == 0);
    // while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCALIBRATEDONE)) = 0;
    // NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
     while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTATUS)) == 1);
    // while (NRF_SAADC->STATUS == 1); //Check ADC Status

    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTART)) = 1;
    // NRF_SAADC->TASKS_START = 1; //Start ADC
    while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) == 0);
    // while (NRF_SAADC->EVENTS_STARTED == 0);
    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) = 0;
    // NRF_SAADC->EVENTS_STARTED = 0;

    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSAMPLE)) = 1;
    // NRF_SAADC->TASKS_SAMPLE = 1; //Sample
    while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTEND)) == 0);
    // while (NRF_SAADC->EVENTS_END == 0);
    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTEND)) = 0;
    // NRF_SAADC->EVENTS_END = 0;

    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTOP)) = 1;
    // NRF_SAADC->TASKS_STOP = 1; //Stop ADC
    while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTOP)) == 0);
    // while (NRF_SAADC->EVENTS_STOPPED == 0);
    *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTOP)) = 0;
    // NRF_SAADC->EVENTS_STOPPED = 0;

    voltageResult = static_cast<float>(analogResult) / convert; //Calculate voltage

    if (voltageResult < 2.459) {
      
      *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTART)) = 1;
      // NRF_SAADC->TASKS_START = 1; //Start ADC
      while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) == 0);
      // while (NRF_SAADC->EVENTS_STARTED == 0);
      *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) = 0;
      // NRF_SAADC->EVENTS_STARTED = 0;

      *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTART)) = 1;
      // NRF_SAADC->TASKS_START = 1; //Start ADC
      while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) == 0);
      // while (NRF_SAADC->EVENTS_STARTED == 0);
      *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) = 0;
      // NRF_SAADC->EVENTS_STARTED = 0;

      *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTOP)) = 1;
      // NRF_SAADC->TASKS_STOP = 1; //Stop ADC
      while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTOP)) == 0);
      // while (NRF_SAADC->EVENTS_STOPPED == 0);
      *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTOP)) = 0;
      // NRF_SAADC->EVENTS_STOPPED = 0;

      voltageResult = static_cast<float>(analogResult) / convert; //Calculate voltage
      if (voltageResult < 2.459) {
        lowBattery = true;
      }
    } else {
      lowBattery = false;
    }

    *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET)) = 0;

    // Read back event register so ensure we have cleared it before exiting IRQ handler.
    dummy = *((volatile uint32_t *)((uint8_t *)NRF_TIMER2_BASE_ADDRESS + (uint32_t)NRF_TIMER_EVENTS_COMPARE0_OFFSET));
    dummy; // to get rid of set but not used warning
  }
}

// State Machines
enum System_States {SYSTEM_INIT, SYSTEM_OFF, SYSTEM_LOWBATTERY, SYSTEM_MODEANDCOLORSELECTOR, SYSTEM_SOLIDMODE, SYSTEM_STROBEMODE, SYSTEM_RGBMODE, SYSTEM_TEXTMODE, SYSTEM_MUSICSPECTRUMMODE, SYSTEM_COLORSPECTRUMMODE} System_state;

typedef struct task {
  int state;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 1;
task tasks[tasksNum];

void updateColor() {
  userSelectedColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, MATRIX_BRIGHTNESS);
  userSelectedColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, MATRIX_BRIGHTNESS);
  userSelectedColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, MATRIX_BRIGHTNESS);
}

void calculateAndDisplaySpectrum() {
  if (samplesRead) {
    for (int i = 0; i < sampleSize; i++) {
      resultBuffer[i] = static_cast<float>(sampleBuffer[i]);
      imaginaryBuffer[i] = static_cast<float>(0);
    }

    windowingTwo(resultBuffer);
    computeFFT(resultBuffer, imaginaryBuffer);

    for (int i = 0; i < sampleSize; i++) {
      magnitude[i] = sqrt((resultBuffer[i] * resultBuffer[i]) + (imaginaryBuffer[i] * imaginaryBuffer[i]));
    }

    userSelectedColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, MATRIX_BRIGHTNESS);
    userSelectedColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, MATRIX_BRIGHTNESS);
    userSelectedColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, MATRIX_BRIGHTNESS);

    double totalFreq = 0.0;
    double totalMag = 0.0;
    int count = 0;
    int index = 0;

    for (int i = 0; i < 252; i++) {
      totalFreq += resultBuffer[i];
      totalMag += magnitude[i];
      count++;
      if (count == 14) {
        totalFreq /= 14;
        totalMag /= 14;
        avgFrequency[index] = abs(static_cast<int>(totalFreq));
        avgMagnitude[index] = static_cast<int>(totalMag);
        totalFreq = 0;
        totalMag = 0;
        count = 0;
        index++;
      }
    }
    updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
    matrixPWM_run();

    for (int i = 0; i < 18; i++) {
      updateNeoPixelWavFrmArr_singleColumnCustomHeight (userSelectedColor, (char)scaleValue(avgFrequency[i], 0, 4000, 24, 7), (char)scaleValue(avgMagnitude[i], 6000, 60000, 0, 8), neoPixelWavFrm);
    }
    matrixPWM_run();

    samplesRead = 0;  // clear the read count

  }
}

int System_Tick(int state) {
  switch (state) { // State transitions
    case SYSTEM_INIT:
      matrixLED_colorG = 255;
      matrixLED_colorR = 255;
      matrixLED_colorB = 255;
      userSelectedColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, MATRIX_BRIGHTNESS);
      userSelectedColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, MATRIX_BRIGHTNESS);
      userSelectedColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, MATRIX_BRIGHTNESS);
      matrixPWM_initialization();
      matrixPWM_run();
      modeUpdated = false;
      colorUpdated = false;
      endTextScrolling = false;
      adcInit();
      initializeBatteryReadTimer();
      BTinit();
      k = 0;
      colorRed = 0;
      colorGreen = 0;
      colorBlue = 0;
      RtoG = true;
      GtoB = false;
      BtoR = false;
      state = SYSTEM_OFF;
      break;
    case SYSTEM_OFF:
      state = SYSTEM_OFF;
      if (modeUpdated || colorUpdated) {
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_LOWBATTERY:
      state = SYSTEM_LOWBATTERY;
      if (!lowBattery || userSelectedMode == 0b00000000) {
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_MODEANDCOLORSELECTOR:
      if (lowBattery && userSelectedMode != 0b00000000) {
        for (volatile int i = 0; i < 250000; ++i);
        color red;
        red.colorG = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
        red.colorR = (char)scaleValue(255, 0, 255, 0, MATRIX_BRIGHTNESS);
        red.colorB = (char)scaleValue(0, 0, 255, 0, MATRIX_BRIGHTNESS);
        updateNeoPixelWavFrmArr_singlePixel(red, 123, neoPixelWavFrm);
        updateNeoPixelWavFrmArr_singlePixel(red, 124, neoPixelWavFrm);
        updateNeoPixelWavFrmArr_singlePixel(red, 131, neoPixelWavFrm);
        updateNeoPixelWavFrmArr_singlePixel(red, 132, neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_LOWBATTERY;
        break;
      }
      if (colorUpdated) {
        updateColor();
        colorUpdated = false;
      }
      modeUpdated = false;
      switch (userSelectedMode) {
        case 0b00000000:
          updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
          matrixPWM_run();
          state = SYSTEM_OFF;
          break;
        case 0b00000001:
          updateNeoPixelWavFrmArr_solidColorMode(userSelectedColor, neoPixelWavFrm);
          matrixPWM_run();
          state = SYSTEM_SOLIDMODE;
          break;
        case 0b00000010:
          updateNeoPixelWavFrmArr_blinkColorMode_On(userSelectedColor);
          state = SYSTEM_STROBEMODE;
          break;
        case 0b00000011:
          updateNeoPixelWavFrmArr_rainbowMode_Init(neoPixelWavFrm);
          state = SYSTEM_RGBMODE;
          break;
        case 0b00000100:
          state = SYSTEM_TEXTMODE;
          break;
        case 0b00000101:
          PDM_begin(1, 16000, 80, onPDMdata);
          for (volatile unsigned i = 0; i < 500000; ++i);
          state = SYSTEM_MUSICSPECTRUMMODE;
          break;
        case 0b00000110:
          k = 0;
          colorRed = 0;
          colorGreen = 0;
          colorBlue = 0;
          RtoG = true;
          GtoB = false;
          BtoR = false;
          state = SYSTEM_COLORSPECTRUMMODE;
          break;
        default:
          state = SYSTEM_INIT;
          break;
      }
      break;
    case SYSTEM_SOLIDMODE:
      state = SYSTEM_SOLIDMODE;
      if (modeUpdated || colorUpdated || lowBattery) {
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_STROBEMODE:
      state = SYSTEM_STROBEMODE;
      if (modeUpdated || colorUpdated || lowBattery) {
        updateNeoPixelWavFrmArr_blinkColorMode_Off();
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_RGBMODE:
      state = SYSTEM_RGBMODE;
      if (modeUpdated || colorUpdated || lowBattery) {
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_TEXTMODE:
      state = SYSTEM_TEXTMODE;
      if (modeUpdated || colorUpdated || endTextScrolling || lowBattery) {
        endTextScrolling = false;
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_MUSICSPECTRUMMODE:
      state = SYSTEM_MUSICSPECTRUMMODE;
      if (modeUpdated || colorUpdated || lowBattery) {
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    case SYSTEM_COLORSPECTRUMMODE:
      state = SYSTEM_COLORSPECTRUMMODE;
      if (modeUpdated || colorUpdated || lowBattery) {
        updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
        matrixPWM_run();
        state = SYSTEM_MODEANDCOLORSELECTOR;
      }
      break;
    default:
      state = SYSTEM_INIT;
      break;
  }

  switch (state) { // State Action
    case SYSTEM_OFF:
      // Intentionally Left Blank
      break;
    case SYSTEM_LOWBATTERY:
      // Intentionally Left Blank
      break;
    case SYSTEM_MODEANDCOLORSELECTOR:
      // Intentionally Left Blank
      break;
    case SYSTEM_SOLIDMODE:
      // Intentionally Left Blank
      break;
    case SYSTEM_STROBEMODE:
      // Intentionally Left Blank
      break;
    case SYSTEM_RGBMODE:
      updateNeoPixelWavFrmArr_rainbowMode_Loop(neoPixelWavFrm);
      matrixPWM_run();
      for (volatile int i = 0; i < 250000; ++i);
      break;
    case SYSTEM_TEXTMODE:
      updateNeoPixelWavFrmArr_textScrollMode(userSelectedColor, userTypedMessage, userTypedMessageLength, neoPixelWavFrm);
      for (char i = 0; i < 16; ++i) {
        if (endTextScrolling) {
          break;
        }
        matrixPWM_run();
        for (volatile int i = 0; i < SCROLLING_DELAY; ++i);
        updateNeoPixelWavFrmArr_rightShift(false, neoPixelWavFrm);
      }
      break;
    case SYSTEM_MUSICSPECTRUMMODE:
      calculateAndDisplaySpectrum();
      break;
    case SYSTEM_COLORSPECTRUMMODE:
      if (RtoG) {
        colorRed = 255 - k;
        colorGreen = k;
        colorBlue = 0;
        if (colorRed == 0) {
          RtoG = false;
          GtoB = true;
        }
      }//End of RtoG
      else if (GtoB) {
        colorRed = 0;
        colorGreen = 255 - k;
        colorBlue = k;
        if (colorGreen == 0) {
          GtoB = false;
          BtoR = true;
        }
      }//End of GtoB
      else if (BtoR) {
        colorRed = k;
        colorGreen = 0;
        colorBlue = 255 - k;
        if (colorBlue == 0) {
          BtoR = false;
          RtoG = true;
        }
      }//End of BtoR
      matrixLED_colorR = colorRed;
      matrixLED_colorG = colorGreen;
      matrixLED_colorB = colorBlue;
      updateColor();
      updateNeoPixelWavFrmArr_solidColorMode(userSelectedColor, neoPixelWavFrm);
      matrixPWM_run();
      if (k == 255) {
        k = 0;
      }
      for (volatile int i = 0; i < 5000; ++i); //Change to 150000 for higher speed
      k++;
      break;
    default:
      state = SYSTEM_INIT;
  }

  return state;
}

int main() {
  unsigned char i = 0;
  tasks[i].state = SYSTEM_INIT;
  tasks[i].TickFct = &System_Tick;

  while (1) {
    unsigned char i;
    for (i = 0; i < tasksNum; ++i) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
    }
  }

  return 0;
}

extern "C" {
  __attribute__((__used__)) void TIMER2_IRQHandler_v(void)
  {
    TIMER2_IRQHandler();
  }
}

extern "C" {
  __attribute__((__used__)) void UARTE0_UART0_IRQHandler_v(void)
  {
    UARTE0_UART0_IRQHandler();
  }
}