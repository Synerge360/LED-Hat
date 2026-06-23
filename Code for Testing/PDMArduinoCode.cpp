#include <LEDPROJECT_PDM.h>
#include <cmath>

// buffer to read samples into, each sample is 16-bits
short audioBuffer[256];

// number of samples read
int audioRead;
int maxAudio = 0;

void onPDMdatas() {
  int bytesAvailable = PDM_available(); // query the number of bytes available
  PDM_read(audioBuffer, bytesAvailable); // read into the sample buffer
  audioRead = bytesAvailable / 2; // 16-bit, 2 bytes per sample
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

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
    while (1);
  }
  delay(6000);
}

void loop() {
  // wait for samples to be read
  //delay(1000);
  if (audioRead) {
    // print samples to the serial monitor or plotter
    for (int i = 0; i < audioRead; i++) {

      // if(audioBuffer[i] > maxAudio){
      //   maxAudio = audioBuffer[i];
      //   Serial.println(maxAudio);
      // }
      Serial.println(audioBuffer[i]);

    }

    audioRead = 0;
  }

}
