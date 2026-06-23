//PDM.h: Header file for PDM.cpp

#ifndef _LEDPROJECT_PDM_H_INCLUDED
#define _LEDPROJECT_PDM_H_INCLUDED

#include <Arduino.h>
#include <pinDefinitions.h>

#include "LEDPROJECT_DoubleBuffer.h"
#include "LEDPROJECT_GPIO.h"
#include "LEDPROJECT_NVIC.h"
#include "LEDPROJECT_FFT.h"

#define DEFAULT_PDM_GAIN     20  ///Change later
#define PDM_IRQ_PRIORITY     7

#define pwrPinInt     17
#define clkPinInt     26
#define dinPinInt     25

#define NRF_P0_BASE_ADDRESS  (0x50000000UL)
#define NRF_P0_OUTCLR_OFFSET  (0x50C)
#define NRF_P0_OUTSET_OFFSET  (0x508)

#define NRF_CLOCK_BASE_ADDRESS  (0x40000000UL)
#define NRF_CLOCK_TASKS_HFCLKSTART_OFFSET  (0x000)
#define NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET  (0x100)

#define NRF_PDM_BASE_ADDRESS  (0x4001D000UL)
#define NRF_PDM_EVENTS_STARTED_OFFSET  (0x100)
#define NRF_PDM_EVENTS_STOPPED_OFFSET  (0x104)
#define NRF_PDM_EVENTS_END_OFFSET  (0x108)
#define NRF_PDM_TASKS_START_OFFSET  (0x000)
#define NRF_PDM_RATIO_OFFSET  (0x520)
#define NRF_PDM_PDMCLKCTRL_OFFSET  (0x504)
#define NRF_PDM_MODE_OFFSET  (0x508)
#define NRF_PDM_GAINL_OFFSET  (0x518)
#define NRF_PDM_GAINR_OFFSET  (0x51C)
#define NRF_PDM_PSEL_CLK_OFFSET  (0x540)
#define NRF_PDM_PSEL_DIN_OFFSET  (0x544)
#define NRF_PDM_INTENSET_OFFSET  (0x304)
#define NRF_PDM_ENABLE_OFFSET  (0x500)
#define NRF_PDM_SAMPLE_PTR_OFFSET  (0x560)
#define NRF_PDM_SAMPLE_MAXCNT_OFFSET  (0x564)

#define NRF_PDM_FREQ_1280K  (0x0A000000UL)           ///< PDM_CLK= 1.280 MHz (32 MHz / 25) => Fs= 20000 Hz
#define NRF_PDM_FREQ_2000K  (0x10000000UL)           ///< PDM_CLK= 2.000 MHz (32 MHz / 16) => Fs= 31250 Hz
#define NRF_PDM_FREQ_2667K  (0x15000000UL)           ///< PDM_CLK= 2.667 MHz (32 MHz / 12) => Fs= 41667 Hz
#define NRF_PDM_FREQ_3200K  (0x19000000UL)           ///< PDM_CLK= 3.200 MHz (32 MHz / 10) => Fs= 50000 Hz
#define NRF_PDM_FREQ_4000K  (0x20000000UL)           ///< PDM_CLK= 4.000 MHz (32 MHz /  8) => Fs= 62500 Hz


/* Peripheral: PDM */
/* Description: Pulse Density Modulation (Digital Microphone) Interface */

/* Register: PDM_INTEN */
/* Description: Enable or disable interrupt */

/* Bit 2 : Enable or disable interrupt for END event */
#define PDM_INTEN_END_Pos (2UL) /*!< Position of END field. */
#define PDM_INTEN_END_Msk (0x1UL << PDM_INTEN_END_Pos) /*!< Bit mask of END field. */
#define PDM_INTEN_END_Disabled (0UL) /*!< Disable */
#define PDM_INTEN_END_Enabled (1UL) /*!< Enable */

/* Bit 1 : Enable or disable interrupt for STOPPED event */
#define PDM_INTEN_STOPPED_Pos (1UL) /*!< Position of STOPPED field. */
#define PDM_INTEN_STOPPED_Msk (0x1UL << PDM_INTEN_STOPPED_Pos) /*!< Bit mask of STOPPED field. */
#define PDM_INTEN_STOPPED_Disabled (0UL) /*!< Disable */
#define PDM_INTEN_STOPPED_Enabled (1UL) /*!< Enable */

/* Bit 0 : Enable or disable interrupt for STARTED event */
#define PDM_INTEN_STARTED_Pos (0UL) /*!< Position of STARTED field. */
#define PDM_INTEN_STARTED_Msk (0x1UL << PDM_INTEN_STARTED_Pos) /*!< Bit mask of STARTED field. */
#define PDM_INTEN_STARTED_Disabled (0UL) /*!< Disable */
#define PDM_INTEN_STARTED_Enabled (1UL) /*!< Enable */

/* Register: PDM_INTENSET */
/* Description: Enable interrupt */

/* Bit 2 : Write '1' to Enable interrupt for END event */
#define PDM_INTENSET_END_Pos (2UL) /*!< Position of END field. */
#define PDM_INTENSET_END_Msk (0x1UL << PDM_INTENSET_END_Pos) /*!< Bit mask of END field. */
#define PDM_INTENSET_END_Disabled (0UL) /*!< Read: Disabled */
#define PDM_INTENSET_END_Enabled (1UL) /*!< Read: Enabled */
#define PDM_INTENSET_END_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to Enable interrupt for STOPPED event */
#define PDM_INTENSET_STOPPED_Pos (1UL) /*!< Position of STOPPED field. */
#define PDM_INTENSET_STOPPED_Msk (0x1UL << PDM_INTENSET_STOPPED_Pos) /*!< Bit mask of STOPPED field. */
#define PDM_INTENSET_STOPPED_Disabled (0UL) /*!< Read: Disabled */
#define PDM_INTENSET_STOPPED_Enabled (1UL) /*!< Read: Enabled */
#define PDM_INTENSET_STOPPED_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to Enable interrupt for STARTED event */
#define PDM_INTENSET_STARTED_Pos (0UL) /*!< Position of STARTED field. */
#define PDM_INTENSET_STARTED_Msk (0x1UL << PDM_INTENSET_STARTED_Pos) /*!< Bit mask of STARTED field. */
#define PDM_INTENSET_STARTED_Disabled (0UL) /*!< Read: Disabled */
#define PDM_INTENSET_STARTED_Enabled (1UL) /*!< Read: Enabled */
#define PDM_INTENSET_STARTED_Set (1UL) /*!< Enable */

/* Register: PDM_INTENCLR */
/* Description: Disable interrupt */

/* Bit 2 : Write '1' to Disable interrupt for END event */
#define PDM_INTENCLR_END_Pos (2UL) /*!< Position of END field. */
#define PDM_INTENCLR_END_Msk (0x1UL << PDM_INTENCLR_END_Pos) /*!< Bit mask of END field. */
#define PDM_INTENCLR_END_Disabled (0UL) /*!< Read: Disabled */
#define PDM_INTENCLR_END_Enabled (1UL) /*!< Read: Enabled */
#define PDM_INTENCLR_END_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to Disable interrupt for STOPPED event */
#define PDM_INTENCLR_STOPPED_Pos (1UL) /*!< Position of STOPPED field. */
#define PDM_INTENCLR_STOPPED_Msk (0x1UL << PDM_INTENCLR_STOPPED_Pos) /*!< Bit mask of STOPPED field. */
#define PDM_INTENCLR_STOPPED_Disabled (0UL) /*!< Read: Disabled */
#define PDM_INTENCLR_STOPPED_Enabled (1UL) /*!< Read: Enabled */
#define PDM_INTENCLR_STOPPED_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to Disable interrupt for STARTED event */
#define PDM_INTENCLR_STARTED_Pos (0UL) /*!< Position of STARTED field. */
#define PDM_INTENCLR_STARTED_Msk (0x1UL << PDM_INTENCLR_STARTED_Pos) /*!< Bit mask of STARTED field. */
#define PDM_INTENCLR_STARTED_Disabled (0UL) /*!< Read: Disabled */
#define PDM_INTENCLR_STARTED_Enabled (1UL) /*!< Read: Enabled */
#define PDM_INTENCLR_STARTED_Clear (1UL) /*!< Disable */

/* Register: PDM_ENABLE */
/* Description: PDM module enable register */

/* Bit 0 : Enable or disable PDM module */
#define PDM_ENABLE_ENABLE_Pos (0UL) /*!< Position of ENABLE field. */
#define PDM_ENABLE_ENABLE_Msk (0x1UL << PDM_ENABLE_ENABLE_Pos) /*!< Bit mask of ENABLE field. */
#define PDM_ENABLE_ENABLE_Disabled (0UL) /*!< Disable */
#define PDM_ENABLE_ENABLE_Enabled (1UL) /*!< Enable */

/* Register: PDM_PDMCLKCTRL */
/* Description: PDM clock generator control */

/* Bits 31..0 : PDM_CLK frequency */
#define PDM_PDMCLKCTRL_FREQ_Pos (0UL) /*!< Position of FREQ field. */
#define PDM_PDMCLKCTRL_FREQ_Msk (0xFFFFFFFFUL << PDM_PDMCLKCTRL_FREQ_Pos) /*!< Bit mask of FREQ field. */
#define PDM_PDMCLKCTRL_FREQ_1000K (0x08000000UL) /*!< PDM_CLK = 32 MHz / 32 = 1.000 MHz */
#define PDM_PDMCLKCTRL_FREQ_Default (0x08400000UL) /*!< PDM_CLK = 32 MHz / 31 = 1.032 MHz. Nominal clock for RATIO=Ratio64. */
#define PDM_PDMCLKCTRL_FREQ_1067K (0x08800000UL) /*!< PDM_CLK = 32 MHz / 30 = 1.067 MHz */
#define PDM_PDMCLKCTRL_FREQ_1231K (0x09800000UL) /*!< PDM_CLK = 32 MHz / 26 = 1.231 MHz */
#define PDM_PDMCLKCTRL_FREQ_1280K (0x0A000000UL) /*!< PDM_CLK = 32 MHz / 25 = 1.280 MHz. Nominal clock for RATIO=Ratio80. */
#define PDM_PDMCLKCTRL_FREQ_1333K (0x0A800000UL) /*!< PDM_CLK = 32 MHz / 24 = 1.333 MHz */

/* Register: PDM_MODE */
/* Description: Defines the routing of the connected PDM microphones' signals */

/* Bit 1 : Defines on which PDM_CLK edge Left (or mono) is sampled */
#define PDM_MODE_EDGE_Pos (1UL) /*!< Position of EDGE field. */
#define PDM_MODE_EDGE_Msk (0x1UL << PDM_MODE_EDGE_Pos) /*!< Bit mask of EDGE field. */
#define PDM_MODE_EDGE_LeftFalling (0UL) /*!< Left (or mono) is sampled on falling edge of PDM_CLK */
#define PDM_MODE_EDGE_LeftRising (1UL) /*!< Left (or mono) is sampled on rising edge of PDM_CLK */

/* Bit 0 : Mono or stereo operation */
#define PDM_MODE_OPERATION_Pos (0UL) /*!< Position of OPERATION field. */
#define PDM_MODE_OPERATION_Msk (0x1UL << PDM_MODE_OPERATION_Pos) /*!< Bit mask of OPERATION field. */
#define PDM_MODE_OPERATION_Stereo (0UL) /*!< Sample and store one pair (Left + Right) of 16bit samples per RAM word R=[31:16]; L=[15:0] */
#define PDM_MODE_OPERATION_Mono (1UL) /*!< Sample and store two successive Left samples (16 bit each) per RAM word L1=[31:16]; L0=[15:0] */

/* Register: PDM_GAINL */
/* Description: Left output gain adjustment */

/* Bits 6..0 : Left output gain adjustment, in 0.5 dB steps, around the default module gain (see electrical parameters) 0x00    -20 dB gain adjust 0x01  -19.5 dB gain adjust (...) 0x27   -0.5 dB gain adjust 0x28      0 dB gain adjust 0x29   +0.5 dB gain adjust (...) 0x4F  +19.5 dB gain adjust 0x50    +20 dB gain adjust */
#define PDM_GAINL_GAINL_Pos (0UL) /*!< Position of GAINL field. */
#define PDM_GAINL_GAINL_Msk (0x7FUL << PDM_GAINL_GAINL_Pos) /*!< Bit mask of GAINL field. */
#define PDM_GAINL_GAINL_MinGain (0x00UL) /*!< -20dB gain adjustment (minimum) */
#define PDM_GAINL_GAINL_DefaultGain (0x28UL) /*!< 0dB gain adjustment ('2500 RMS' requirement) */
#define PDM_GAINL_GAINL_MaxGain (0x50UL) /*!< +20dB gain adjustment (maximum) */

/* Register: PDM_GAINR */
/* Description: Right output gain adjustment */

/* Bits 7..0 : Right output gain adjustment, in 0.5 dB steps, around the default module gain (see electrical parameters) */
#define PDM_GAINR_GAINR_Pos (0UL) /*!< Position of GAINR field. */
#define PDM_GAINR_GAINR_Msk (0xFFUL << PDM_GAINR_GAINR_Pos) /*!< Bit mask of GAINR field. */
#define PDM_GAINR_GAINR_MinGain (0x00UL) /*!< -20dB gain adjustment (minimum) */
#define PDM_GAINR_GAINR_DefaultGain (0x28UL) /*!< 0dB gain adjustment ('2500 RMS' requirement) */
#define PDM_GAINR_GAINR_MaxGain (0x50UL) /*!< +20dB gain adjustment (maximum) */

/* Register: PDM_RATIO */
/* Description: Selects the ratio between PDM_CLK and output sample rate. Change PDMCLKCTRL accordingly. */

/* Bit 0 : Selects the ratio between PDM_CLK and output sample rate */
#define PDM_RATIO_RATIO_Pos (0UL) /*!< Position of RATIO field. */
#define PDM_RATIO_RATIO_Msk (0x1UL << PDM_RATIO_RATIO_Pos) /*!< Bit mask of RATIO field. */
#define PDM_RATIO_RATIO_Ratio64 (0UL) /*!< Ratio of 64 */
#define PDM_RATIO_RATIO_Ratio80 (1UL) /*!< Ratio of 80 */

/* Register: PDM_PSEL_CLK */
/* Description: Pin number configuration for PDM CLK signal */

/* Bit 31 : Connection */
#define PDM_PSEL_CLK_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define PDM_PSEL_CLK_CONNECT_Msk (0x1UL << PDM_PSEL_CLK_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define PDM_PSEL_CLK_CONNECT_Connected (0UL) /*!< Connect */
#define PDM_PSEL_CLK_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define PDM_PSEL_CLK_PORT_Pos (5UL) /*!< Position of PORT field. */
#define PDM_PSEL_CLK_PORT_Msk (0x1UL << PDM_PSEL_CLK_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define PDM_PSEL_CLK_PIN_Pos (0UL) /*!< Position of PIN field. */
#define PDM_PSEL_CLK_PIN_Msk (0x1FUL << PDM_PSEL_CLK_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: PDM_PSEL_DIN */
/* Description: Pin number configuration for PDM DIN signal */

/* Bit 31 : Connection */
#define PDM_PSEL_DIN_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define PDM_PSEL_DIN_CONNECT_Msk (0x1UL << PDM_PSEL_DIN_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define PDM_PSEL_DIN_CONNECT_Connected (0UL) /*!< Connect */
#define PDM_PSEL_DIN_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define PDM_PSEL_DIN_PORT_Pos (5UL) /*!< Position of PORT field. */
#define PDM_PSEL_DIN_PORT_Msk (0x1UL << PDM_PSEL_DIN_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define PDM_PSEL_DIN_PIN_Pos (0UL) /*!< Position of PIN field. */
#define PDM_PSEL_DIN_PIN_Msk (0x1FUL << PDM_PSEL_DIN_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: PDM_SAMPLE_PTR */
/* Description: RAM address pointer to write samples to with EasyDMA */

/* Bits 31..0 : Address to write PDM samples to over DMA */
#define PDM_SAMPLE_PTR_SAMPLEPTR_Pos (0UL) /*!< Position of SAMPLEPTR field. */
#define PDM_SAMPLE_PTR_SAMPLEPTR_Msk (0xFFFFFFFFUL << PDM_SAMPLE_PTR_SAMPLEPTR_Pos) /*!< Bit mask of SAMPLEPTR field. */

/* Register: PDM_SAMPLE_MAXCNT */
/* Description: Number of samples to allocate memory for in EasyDMA mode */

/* Bits 14..0 : Length of DMA RAM allocation in number of samples */
#define PDM_SAMPLE_MAXCNT_BUFFSIZE_Pos (0UL) /*!< Position of BUFFSIZE field. */
#define PDM_SAMPLE_MAXCNT_BUFFSIZE_Msk (0x7FFFUL << PDM_SAMPLE_MAXCNT_BUFFSIZE_Pos) /*!< Bit mask of BUFFSIZE field. */


int PDM_begin(int channels, int sampleRate, int gain, void(*function)(void));
void PDM_end(); 
int PDM_available();
int PDM_read(void* buffer, int size);
void PDM_setBufferSize(int bufferSize);
void PDM_IrqHandler(bool halftranfer);
void onPDMdata();

#endif
