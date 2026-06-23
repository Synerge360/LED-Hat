volatile int analogResult = 0;
volatile float voltageResult = 0;
float convert = 1137.77;

void setup(){
  Serial.begin(9600);
  while (!Serial);

  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

  NRF_SAADC->CH[0].CONFIG = 0; //Single-ended, 1/6 gain, internal reference
  NRF_SAADC->CH[0].PSELP = (1UL); //AnalogInput0 (A5)
  NRF_SAADC->CH[0].PSELN = (0UL);
  NRF_SAADC->RESOLUTION = (2UL); //12 Bit Resolution
  NRF_SAADC->RESULT.MAXCNT = 1; //Set up buffer
  NRF_SAADC->RESULT.PTR = (uint32_t)&analogResult;

  NRF_SAADC->SAMPLERATE = (0UL << 12UL); //Use sample task rate
  NRF_SAADC->ENABLE = 1; //Enable ADC

}

void loop(){

  delay(500);
  NRF_SAADC->TASKS_CALIBRATEOFFSET = 1; //Callibrate ADC
  while(NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
  NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
  while(NRF_SAADC->STATUS == 1); //Check ADC Status

  NRF_SAADC->TASKS_START = 1; //Start ADC
  while(NRF_SAADC->EVENTS_STARTED == 0);
  NRF_SAADC->EVENTS_STARTED = 0;

  NRF_SAADC->TASKS_SAMPLE = 1; //Sample
  while(NRF_SAADC->EVENTS_END == 0);
  NRF_SAADC->EVENTS_END = 0;

  NRF_SAADC->TASKS_STOP = 1; //Stop ADC
  while(NRF_SAADC->EVENTS_STOPPED == 0);
  NRF_SAADC->EVENTS_STOPPED = 0;

  voltageResult = static_cast<float>(analogResult)/ convert; //Calculate voltage
  Serial.println(voltageResult);


}//End of Loop

