#include <LEDPROJECT_FFT.h>

//#include <arduinoFFT.h>
// #include <cmath>
// #include <LEDPROJECT_PDM.h>
// #include <LEDPROJECT_MATRIX.h>

//volatile int samplesRead; //number of samples read
//const short sampleSize = 256;
//arduinoFFT FFT = arduinoFFT();
color userSelectedColor;

//double imaginaryBuffer[sampleSize], resultBuffer[sampleSize], magnitude[sampleSize];
//short sampleBuffer[sampleSize];
//int avgFrequency[18], avgMagnitude[18];

// void onPDMdata() {
//   int bytesAvailable = PDM_available(); // query the number of bytes available
//   PDM_read(sampleBuffer, bytesAvailable); // read into the sample buffer
//   samplesRead = bytesAvailable / 2; // 16-bit, 2 bytes per sample
// }

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  // - 30 gain
  if (!PDM_begin(1, 16000, 80, onPDMdata)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
  matrixPWM_initialization();
}

void loop() {
  delay(4000);
  while (1) {
    //delay(1000);
    if (samplesRead) {
      for (int i = 0; i < sampleSize; i++) {
        resultBuffer[i] = static_cast<float>(sampleBuffer[i]);
        imaginaryBuffer[i] = static_cast<float>(0);
      }

      // Serial.println("\n\nReal Before");
      // for (int i = 0; i < sampleSize; i++) {
      //   Serial.print(resultBuffer[i]);
      //   Serial.print(" ");
      // }

      // Serial.println("\nImaginary Before");
      // for (int i = 0; i < sampleSize; i++) {
      //   Serial.print(imaginaryBuffer[i]);
      //   Serial.print(" ");
      // }

      // FFT.Windowing(resultBuffer, sampleSize, FFT_WIN_TYP_HAMMING, FFT_FORWARD); //Window
      // FFT.Compute(resultBuffer, imaginaryBuffer, sampleSize, FFT_FORWARD); //Compute FFT
      windowingTwo(resultBuffer);
      computeFFT(resultBuffer, imaginaryBuffer);

      // Serial.println("\nReal After");
      // for (int i = 0; i < sampleSize; i++) {
      //   Serial.print(resultBuffer[i]);
      //   Serial.print(" ");
      // }

      // Serial.println("\nImaginary After");
      // for (int i = 0; i < sampleSize; i++) {
      //   Serial.print(imaginaryBuffer[i]);
      //   Serial.print(" ");
      // }

      for (int i = 0; i < sampleSize; i++) {
        magnitude[i] = sqrt((resultBuffer[i] * resultBuffer[i]) + (imaginaryBuffer[i] * imaginaryBuffer[i]));
      }

      // Serial.println("\nMagnitude");
      // for (int i = 0; i < sampleSize; i++) {
      //   Serial.print(magnitude[i]);
      //   Serial.print(" ");
      // }

      userSelectedColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, MATRIX_BRIGHTNESS);
      userSelectedColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, MATRIX_BRIGHTNESS);
      userSelectedColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, MATRIX_BRIGHTNESS);

      double totalFreq = 0.0;
      double totalMag = 0.0;
      int count = 0;
      int index = 0;

      for(int i = 0; i < 252; i++){
        totalFreq += resultBuffer[i];
        totalMag += magnitude[i];
        count++;
        if(count == 14){
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

      // Serial.println("\nAVGFREQUENCY");
      // for (int i = 0; i < 18; i++) {
      //   Serial.print(avgFrequency[i]);
      //   Serial.print(" ");
      // }

      // Serial.println("\nAVGMAGNITUDE");
      // for (int i = 0; i < 18; i++) {
      //   Serial.print(avgMagnitude[i]);
      //   Serial.print(" ");
      // }

      updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
      matrixPWM_run();

      for(int i = 0; i < 18; i++){
        updateNeoPixelWavFrmArr_singleColumnCustomHeight (userSelectedColor, (char)scaleValue(avgFrequency[i], 0, 4000, 24, 7), (char)scaleValue(avgMagnitude[i], 6000, 60000, 0, 8), neoPixelWavFrm);
      }
      matrixPWM_run();

      samplesRead = 0;  // clear the read count

    }//End of if(samplesRead)
  }//End of while(1)
}//End of void loop

//try increasing gain
//try changing avgMagnitude scaleValue

//18 columns, 8 rows
//array of [18] for columns (frequency) meter
//array of [18] for rows (amplitude)
//magnitude use ComplexToMagnitude (y axis)
