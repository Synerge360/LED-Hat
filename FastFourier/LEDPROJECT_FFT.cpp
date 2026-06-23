#include "LEDPROJECT_FFT.h"

volatile int samplesRead;
const short powerSample = log2(sampleSize);
double imaginaryBuffer[sampleSize]  = {};
double resultBuffer[sampleSize] = {};
double magnitude[sampleSize] = {};
short sampleBuffer[sampleSize] = {};
int avgFrequency[18] = {};
int avgMagnitude[18] = {};

void windowingFunction(double *samples) 
{
  double a0 = 0.54;
  double weightFactor;
  for (int i = 0; i < (sampleSize); i++) {

    weightFactor = a0 - ((1-a0) * cos(2*PI*((double)i/(sampleSize - 1))));
    samples[i] *= weightFactor;
  }
}

int reverseBits(double *samples, double *imaginary){
  int index = 0;
  for (int i = 0; i < 254; i++) {
    if (i < index) {
      double temp = samples[i];
      samples[i] = samples[index];
      samples[index] = temp;
    }
    int count = (sampleSize / 2);
    while (count <= index) {
      index -= count;
      count /= 2;
    }
    index += count;
  }
  return index;
}

void computeFFT(double *samples, double *imaginary)
{
  int one = 1;
  double negativeOne = -1.0;
  double base = 0.5;
  double zero = 0.0;
  double offset = base;
  int counter = reverseBits(samples, imaginary);

  for (int m = 0; m < powerSample; m++) { 

    double pointOne = 1.0;
    double pointZero = 0.0;
    int max = one;
    one *= 2;

    for (counter = 0; counter < max; counter++) {
      for (int i = counter; i < sampleSize; i += one) {
        int j = i + max;

        double index1 = pointOne * imaginary[j] + pointZero * samples[j];
        double index2 = pointOne * samples[j] - pointZero * imaginary[j];
        samples[j] = samples[i] - index2 + offset;
        samples[i] += index2 + offset;
        imaginary[j] = imaginary[i] - index1 + offset;
        imaginary[i] += index1 + offset;
      }
      double pointOneTemp = ((pointOne * negativeOne) - (pointZero * zero));
      pointZero = ((pointOne * zero) + (pointZero * negativeOne));
      pointOne = pointOneTemp;
    }

    double value = base * negativeOne;
    zero = -1*sqrt(base - value);
    negativeOne = sqrt(base + value);
  }
}

void complexToValue(double *samples, double *imaginary, int sampleSize) 
{
  for (int i = 0; i < sampleSize; i++) {
    samples[i] = sqrt((samples[i] * samples[i]) + (imaginary[i] * imaginary[i]));
  }
}



