#include <LEDPROJECT_MATRIX.h>

char matrixLED_colorG = 255;
char matrixLED_colorR = 255;
char matrixLED_colorB = 255;

int main() {
  color userSelectedColor;
  userSelectedColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, MATRIX_BRIGHTNESS);
  userSelectedColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, MATRIX_BRIGHTNESS);
  userSelectedColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, MATRIX_BRIGHTNESS);

  matrixPWM_initialization();

  /*
  while(1){
    updateNeoPixelWavFrmArr_textScrollMode(userSelectedColor, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789&'*+-=.!\"#$%^,:;?@/<>|\\[]{}_", neoPixelWavFrm);
    for(char i = 0; i < 16; ++i){
      matrixPWM_run();
      for (volatile int i = 0; i < SCROLLING_DELAY; ++i);
      updateNeoPixelWavFrmArr_rightShift(false, neoPixelWavFrm);
    }
  }
  */

//  updateNeoPixelWavFrmArr_textScrollMode(userSelectedColor, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", neoPixelWavFrm);

  /*
     Fake Spectrum
    while (1) {
      for (char i = 7; i < 24; ++i) {
        char random = rand();
        updateNeoPixelWavFrmArr_singleColumnCustomHeight(userSelectedColor, i, random % 9, neoPixelWavFrm);
      }

      matrixPWM_run();
      for (volatile int i = 0; i < 500000; ++i);
    }
  */

  /*
     Hi Text
    updateNeoPixelWavFrmArr_singleColumn(userSelectedColor, 11, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singleColumn(userSelectedColor, 9, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singleColumn(userSelectedColor, 7, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 84, neoPixelWavFrm);
    matrixPWM_run();
  */

  /*
     Alvin Text
    for(char i = 57; i <= 62; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 66, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 67, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 74, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 75, neoPixelWavFrm);
    for(char i = 81; i <= 86; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    for(char i = 97; i <= 102; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    for(char i = 114; i <= 118; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 126, neoPixelWavFrm);
    for(char i = 130; i <= 134; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 145, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 158, neoPixelWavFrm);
    for(char i = 161; i <= 166; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    for(char i = 177; i <= 182; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 185, neoPixelWavFrm);
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, 187, neoPixelWavFrm);
    for(char i = 193; i <= 198; ++i){
    updateNeoPixelWavFrmArr_singlePixel(userSelectedColor, i, neoPixelWavFrm);
    }
  */

  /*
     RGB Mode
    updateNeoPixelWavFrmArr_rainbowMode_Init(neoPixelWavFrm);

    while (1){
    updateNeoPixelWavFrmArr_rainbowMode_Loop(neoPixelWavFrm);
    matrixPWM_run();
    for(volatile int i = 0; i < 500000; ++i);
    }
  */

  // Blink Mode
  updateNeoPixelWavFrmArr_blinkColorMode_On(userSelectedColor);

  while (1) {
    __WFE();
  }

  return 0;
}