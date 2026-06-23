//PDM.cpp: Controls microphone for Spectrometer Mode

#include "LEDPROJECT_PDM.h"

#if defined(ARDUINO_ARCH_NRF52840)

DoubleBuffer PDMBuffer(512);
void(*_onReceive)(void); 
int _channels;

int PDM_begin(int channels, int sampleRate, int gain, void(*function)(void))
{
  _onReceive = function;
  _channels = channels;

  // Enable high frequency oscillator if not already enabled
  if (*((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET)) == 0) {
    *((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_TASKS_HFCLKSTART_OFFSET)) = 1;
    while (*((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET)) == 0) { }
  }

  // configure the sample rate and channels
  switch (sampleRate) {
    case 16000:
      *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_RATIO_OFFSET)) = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
      *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_PDMCLKCTRL_OFFSET)) = ((NRF_PDM_FREQ_1280K << PDM_PDMCLKCTRL_FREQ_Pos) & PDM_PDMCLKCTRL_FREQ_Msk); // Sets PDM clock frequency to 1.280 MHz
      break;
    case 41667:
      *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_PDMCLKCTRL_OFFSET)) = ((NRF_PDM_FREQ_2667K << PDM_PDMCLKCTRL_FREQ_Pos) & PDM_PDMCLKCTRL_FREQ_Msk); // Sets PDM clock frequency to 2.667 MHz
      break;
    default:
      return 0; // unsupported
  }

  switch (channels) {
    case 2:
      *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_MODE_OFFSET)) = ((PDM_MODE_OPERATION_Stereo << PDM_MODE_OPERATION_Pos) & PDM_MODE_OPERATION_Msk)
        | ((PDM_MODE_EDGE_LeftFalling << PDM_MODE_EDGE_Pos) & PDM_MODE_EDGE_Msk);
      break;

    case 1:
      *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_MODE_OFFSET)) = ((PDM_MODE_OPERATION_Mono << PDM_MODE_OPERATION_Pos) & PDM_MODE_OPERATION_Msk)
        | ((PDM_MODE_EDGE_LeftFalling << PDM_MODE_EDGE_Pos) & PDM_MODE_EDGE_Msk);
      break;

    default:
      return 0; // unsupported
  }

  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_GAINL_OFFSET)) = gain;
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_GAINR_OFFSET)) = gain;

  // configure the I/O and mux
  Port0Output(26);
  *((volatile uint32_t *)((uint8_t *)NRF_P0_BASE_ADDRESS + (uint32_t)NRF_P0_OUTCLR_OFFSET)) = (1UL << clkPinInt);

  Port0Input(dinPinInt);

  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_PSEL_CLK_OFFSET)) = 0x0000001AUL; // Port 0 Pin 26
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_PSEL_DIN_OFFSET)) = 0x00000019UL; // Port 0 Pin 25

  // clear events and enable PDM interrupts
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STARTED_OFFSET)) = 0x0UL;
  (void)(*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STARTED_OFFSET)));
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_END_OFFSET)) = 0x0UL;
  (void)(*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_END_OFFSET)));
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STOPPED_OFFSET)) = 0x0UL;
  (void)(*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STOPPED_OFFSET)));
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_INTENSET_OFFSET)) = PDM_INTENSET_STARTED_Msk | PDM_INTENSET_STOPPED_Msk;  

  // Turn on the microphone 
  Port0Output(pwrPinInt);
  *((volatile uint32_t *)((uint8_t *)NRF_P0_BASE_ADDRESS + (uint32_t)NRF_P0_OUTSET_OFFSET)) = (1UL << pwrPinInt);

  // clear the buffer
  PDMBuffer.resetBuffer();

  // set the PDM IRQ priority and enable
  NVIC_SetPriority_Call(PDM_IRQn_Type, PDM_IRQ_PRIORITY);
  NVIC_ClearPendingIRQ_Call(PDM_IRQn_Type);
  NVIC_EnableIRQ_Call(PDM_IRQn_Type);

  // enable and trigger start task
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_ENABLE_OFFSET)) = (PDM_ENABLE_ENABLE_Enabled << PDM_ENABLE_ENABLE_Pos);
  // Set then read. The read acts as a delay to make sure everything settles. 
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STARTED_OFFSET)) = 0x0UL;
  (void)NRF_PDM->EVENTS_STARTED;
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_TASKS_START_OFFSET)) = 0x1UL;

  return 1;
}//End of Begin

void PDM_end() //Alvin is doing this
{
  // disable PDM and IRQ
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_ENABLE_OFFSET)) = 0;

  NVIC_DisableIRQ_Call(PDM_IRQn_Type);

  // power the mic off
  *((volatile uint32_t *)((uint8_t *)NRF_P0_BASE_ADDRESS + (uint32_t)NRF_P0_OUTCLR_OFFSET)) = (1UL << pwrPinInt);
  Port0Input(pwrPinInt);

  // Don't disable high frequency oscillator since it could be in use by RADIO

  // unconfigure the I/O and un-mux
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_PSEL_CLK_OFFSET)) |= (1UL << 31);
  *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_PSEL_DIN_OFFSET)) |= (1UL << 31);

  Port0Input(clkPinInt);
}

int PDM_available()
{
  NVIC_DisableIRQ_Call(PDM_IRQn_Type);

  size_t avail = PDMBuffer.available();

  NVIC_EnableIRQ_Call(PDM_IRQn_Type);

  return avail;
}

int PDM_read(void* buffer, int size)
{
  NVIC_DisableIRQ_Call(PDM_IRQn_Type);

  int read = PDMBuffer.readBuffer(buffer, size);

  NVIC_EnableIRQ_Call(PDM_IRQn_Type);

  return read;
}

void onPDMdata() {
  int bytesAvailable = PDM_available(); // query the number of bytes available
  PDM_read(sampleBuffer, bytesAvailable); // read into the sample buffer
  samplesRead = bytesAvailable / 2; // 16-bit, 2 bytes per sample
}

void PDM_IrqHandler(bool halftranfer)
{
  if (*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STARTED_OFFSET))) {
    // Clear event and read register to ensure it has time to settle. 
    *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STARTED_OFFSET)) = 0x0UL;
    (void)(*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STARTED_OFFSET)));

    if (PDMBuffer.available() == 0) {

        // EasyDMA Transfer
        *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_SAMPLE_PTR_OFFSET)) = (uint32_t)PDMBuffer.getData();
        *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_SAMPLE_MAXCNT_OFFSET)) = PDMBuffer.availableForWrite() / (sizeof(int16_t) * _channels);
      // #endif

      // make the current one available for reading
      PDMBuffer.swapBuffers(PDMBuffer.availableForWrite());

      // call receive callback if provided
      if (_onReceive) {
        _onReceive();
      }
    } else {
      // buffer overflow, stop
      *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_ENABLE_OFFSET)) = 0;
    }
  } else if (*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STOPPED_OFFSET))) {

    // Clear event and read register to ensure it has time to settle. 
    *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STOPPED_OFFSET)) = 0x0UL;
    (void)(*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_STOPPED_OFFSET)));
  } else if (*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_END_OFFSET))) {

    // Clear event and read register to ensure it has time to settle. 
    *((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_END_OFFSET)) = 0x0UL;
    (void)(*((volatile uint32_t *)((uint8_t *)NRF_PDM_BASE_ADDRESS + (uint32_t)NRF_PDM_EVENTS_END_OFFSET)));
  }
}

extern "C" {
  __attribute__((__used__)) void PDM_IRQHandler_v(void)
  {
    PDM_IrqHandler(true);
  }
}

#endif