#ifndef LEDPROJECT_FFT_INCLUDED
#define LEDPROJECT_FFT_INCLUDED

#include <cmath>
#include <LEDPROJECT_PDM.h>
//#include <LEDPROJECT_MATRIX.h>

// array must be static (*samples)
// Sample Buffer always changing

extern volatile int samplesRead;
const short sampleSize = 256;
extern const short powerSample;
extern double imaginaryBuffer[sampleSize], resultBuffer[sampleSize], magnitude[sampleSize];
extern short sampleBuffer[sampleSize];
extern int avgFrequency[18], avgMagnitude[18];

void windowingFunction(double *);
void computeFFT(double *, double *);
void complexToValue(double *, double *, int);
int reverseBits(double *, double *);

#endif
