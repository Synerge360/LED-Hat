#include <cmath>
#include <LEDPROJECT_ADC.h>

volatile int analogResult = 0;
volatile float voltageResult = 0; //EasyDMA analog buffer
float convert = 1137.77;

void setup(){
  Serial.begin(9600);
  while (!Serial);

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
  
}

void loop(){

  delay(1000);
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCTASKCALIBRATE)) = 1;
  // NRF_SAADC->TASKS_CALIBRATEOFFSET = 1; //Callibrate ADC
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCALIBRATEDONE)) == 0);
  // while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCCALIBRATEDONE)) = 0;
  // NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTATUS)) == 1);
  // while (NRF_SAADC->STATUS == 1); //Check ADC Status

  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTART)) = 1;
  //Serial.println("Here 1");
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTART)) == 0);
  //while(NRF_SAADC->EVENTS_STARTED == 0);

  //Serial.println("Here 2");
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSAMPLE)) = 1;
  //NRF_SAADC->TASKS_SAMPLE = 1;
  //while(NRF_SAADC->EVENTS_END == 0);
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTEND)) == 0);
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTEND)) = 0;
  //Serial.println("Here 3");
  //NRF_SAADC->TASKS_STOP = 1;
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCSTOP)) = 1;
  while(*((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTOP)) == 0);
  *((volatile uint32_t *)(ADCBASE + (uint32_t)ADCEVENTSTOP)) = 0;
  //Serial.println("Here 4");
  Serial.print("ADC Samples: ");
  Serial.println(NRF_SAADC->RESULT.AMOUNT);

  Serial.print("Voltage Result: ");
  voltageResult = static_cast<float>(analogResult) / convert;
  Serial.println(voltageResult);

  //while(1);
}//End of Loop

