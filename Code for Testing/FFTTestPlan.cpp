#include <arduinoFFT.h>
#include <LEDPROJECT_PDM.h>
#include <LEDPROJECT_FFT.h>
#include <cmath>

// buffer to read samples into, each sample is 16-bits
short audioBuffer[256];
float realBuffer1[256];
float imaginaryBuffer1[256];
double realBuffer2[256];
double imaginaryBuffer2[256];
float testFrequency;
float testFrequency2;

// number of samples read
int audioRead;
ArduinoFFT<float> FFT = ArduinoFFT<float>(realBuffer1, imaginaryBuffer1, static_cast<float>(256), static_cast<float>(16000), false);
ArduinoFFT<double> FFT2 = ArduinoFFT<double>(realBuffer2, imaginaryBuffer2, static_cast<float>(256), static_cast<float>(16000), false);

void onPDMdatas() {
  int bytesAvailable = PDM_available();   // query the number of bytes available
  PDM_read(audioBuffer, bytesAvailable);  // read into the sample buffer
  audioRead = bytesAvailable / 2;         // 16-bit, 2 bytes per sample
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  // configure the data receive callback
  // PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  // - 30 gain
  if (!PDM_begin(1, 16000, 40, onPDMdatas)) {
    Serial.println("Failed to start PDM!");
    while (1)
      ;
  }

  delay(6000);
}

void loop() {

  delay(1000);
  if (audioRead) {
    for (int i = 0; i < 256; i++) {
      realBuffer1[i] = static_cast<float>(audioBuffer[i]);
      realBuffer2[i] = static_cast<float>(audioBuffer[i]);
      imaginaryBuffer1[i] = static_cast<float>(0);
      imaginaryBuffer2[i] = static_cast<float>(0);
    }

    //Using Arduino Library
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);  //Reduces the amplitude of discontinuities at boundaries of each finite sequence
    FFT.compute(FFTDirection::Forward);                        //Compute FFT
    FFT.complexToMagnitude(realBuffer1, imaginaryBuffer1, 256);
    testFrequency = FFT.majorPeak();

    //Using my FFT Library
    windowingFunction(realBuffer2);
    computeFFT(realBuffer2, imaginaryBuffer2);
    complexToValue(realBuffer2, imaginaryBuffer2, 256);
    testFrequency2 = FFT2.majorPeak();

    Serial.print("Arduino Library Reading: ");
    Serial.print(testFrequency);
    Serial.print(", ");
    Serial.print("My Library Reading: ");
    Serial.println(testFrequency2);
    // for(int i = 0; i < 256; i++){
    //   Serial.print("Arduino:");
    //   Serial.print(realBuffer1[i]);
    //   Serial.print(",");
    //   Serial.print("Mine:");
    //   Serial.print(realBuffer2[i]);
    //   Serial.print(",");
    //   Serial.print("Test:");
    //   Serial.print(testFrequency);
    //   Serial.print(", ");
    //   Serial.print("Test2:");
    //   Serial.println(testFrequency2);
    // }

    audioRead = 0;
  }//End of if
}//End of loop
