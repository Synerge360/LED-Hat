#include <LEDPROJECT_MATRIX.h>
#include <arduinoFFT.h>
#include <LEDPROJECT_PDM.h>
#include <LEDPROJECT_FFT.h>
#include <cmath>

// buffer to read samples into, each sample is 16-bits
short audioBuffer[256];
double realBuffer2[256];
double imaginaryBuffer2[256];
float testFrequency2;
char matrixLED_colorG;
char matrixLED_colorR;
char matrixLED_colorB;
color userSelectedColor;

// number of samples read
int audioRead;
ArduinoFFT<double> FFT2 = ArduinoFFT<double>(realBuffer2, imaginaryBuffer2, static_cast<float>(256), static_cast<float>(16000), false);

void onPDMdatas() {
  int bytesAvailable = PDM_available();   // query the number of bytes available
  PDM_read(audioBuffer, bytesAvailable);  // read into the sample buffer
  audioRead = bytesAvailable / 2;         // 16-bit, 2 bytes per sample
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  matrixLED_colorG = 255;
  matrixLED_colorR = 255;
  matrixLED_colorB = 255;
  userSelectedColor.colorG = (char)scaleValue(matrixLED_colorG, 0, 255, 0, MATRIX_BRIGHTNESS);
  userSelectedColor.colorR = (char)scaleValue(matrixLED_colorR, 0, 255, 0, MATRIX_BRIGHTNESS);
  userSelectedColor.colorB = (char)scaleValue(matrixLED_colorB, 0, 255, 0, MATRIX_BRIGHTNESS);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  // - 40 gain
  if (!PDM_begin(1, 16000, 40, onPDMdatas)) {
    Serial.println("Failed to start PDM!");
    while (1)
      ;
  }
  matrixPWM_initialization();
  matrixPWM_run();
  delay(6000);
}

void loop() {
  //delay(1000);

  if (audioRead) {
    for (int i = 0; i < 256; i++) {
      realBuffer2[i] = static_cast<float>(audioBuffer[i]);
      imaginaryBuffer2[i] = static_cast<float>(0);
    }

    //Using my FFT Library
    windowingFunction(realBuffer2);
    computeFFT(realBuffer2, imaginaryBuffer2);

    double totalFreq = 0.0;
    double totalMag = 0.0;
    int count = 0;
    int index = 0;

    for (int i = 0; i < sampleSize/2 - 2; i++) {
      magnitude[i] = sqrt((realBuffer2[i] * realBuffer2[i]) + (imaginaryBuffer2[i] * imaginaryBuffer2[i]));
      // Serial.print("Frequency:");
      // Serial.print(i * 16000.0/sampleSize); 
      // Serial.print(",");
      // Serial.print("Magnitude:");
      // Serial.println(magnitude[i]);
      totalFreq += (i * 16000.0/sampleSize);
      totalMag += magnitude[i];
      count++;
      if (count == 7) {
        totalFreq /= 7;
        totalMag /= 7;
        avgFrequency[index] = static_cast<int>(totalFreq);
        avgMagnitude[index] = static_cast<int>(totalMag);
        totalFreq = 0;
        totalMag = 0;
        count = 0;
        index++;
      }
    }
    updateNeoPixelWavFrmArr_allPixelsOff(neoPixelWavFrm);
    matrixPWM_run();

    for(int i = 0; i < 18; i++){
      // Serial.print("AvgFrequency:");
      // Serial.print(avgFrequency[i]);
      // Serial.print(", ");
      // Serial.print("AvgMagnitude:");
      // Serial.println(avgMagnitude[i]);
      updateNeoPixelWavFrmArr_singleColumnCustomHeight (userSelectedColor, (char)scaleValue(avgFrequency[i], 0, 8000, 24, 7), (char)scaleValue(avgMagnitude[i], 100, 30000, 0, 8), neoPixelWavFrm);
    }
    matrixPWM_run();


    audioRead = 0;
  }  //End of if
}  //End of loop
