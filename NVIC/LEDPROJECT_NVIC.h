#ifndef LEDPROJECTNVIC_h
#define LEDPROJECTNVIC_h

#include <stdint.h>

#define NVIC_PRIORITY_BITS 3 //number of bits used for priority

#define NVIC_BASE_ADDRESS 0xE000E100UL
#define NVIC_ISER_OFFSET  0x000
#define NVIC_ICER_OFFSET  0x080
#define NVIC_ICPR_OFFSET  0x180
#define NVIC_IP_OFFSET    0x300

/*==  nrf52 Specific Interrupt Numbers === */
typedef enum {
  POWER_CLOCK_IRQn_Type = 0,       // 0  POWER_CLOCK */
  RADIO_IRQn_Type = 1,             // 1  RADIO */
  UARTE0_UART0_IRQn_Type = 2,      // 2  UARTE0_UART0 */
  SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn_Type = 3, // 3  SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0 */
  SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn_Type = 4, // 4  SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1 */
  NFCT_IRQn_Type = 5,              // 5  NFCT */
  GPIOTE_IRQn_Type = 6,            // 6  GPIOTE */
  SAADC_IRQn_Type = 7,             // 7  SAADC */
  TIMER0_IRQn_Type = 8,            // 8  TIMER0 */
  TIMER1_IRQn_Type = 9,            // 9  TIMER1 */
  TIMER2_IRQn_Type = 10,           // 10 TIMER2 */
  RTC0_IRQn_Type = 11,             // 11 RTC0 */
  TEMP_IRQn_Type = 12,             // 12 TEMP */
  RNG_IRQn_Type = 13,              // 13 RNG */
  ECB_IRQn_Type = 14,              // 14 ECB */
  CCM_AAR_IRQn_Type = 15,          // 15 CCM_AAR */
  WDT_IRQn_Type = 16,              // 16 WDT */
  RTC1_IRQn_Type = 17,             // 17 RTC1 */
  QDEC_IRQn_Type = 18,             // 18 QDEC */
  COMP_LPCOMP_IRQn_Type = 19,      // 19 COMP_LPCOMP */
  SWI0_EGU0_IRQn_Type = 20,        // 20 SWI0_EGU0 */
  SWI1_EGU1_IRQn_Type = 21,        // 21 SWI1_EGU1 */
  SWI2_EGU2_IRQn_Type = 22,        // 22 SWI2_EGU2 */
  SWI3_EGU3_IRQn_Type = 23,        // 23 SWI3_EGU3 */
  SWI4_EGU4_IRQn_Type = 24,        // 24 SWI4_EGU4 */
  SWI5_EGU5_IRQn_Type = 25,        // 25 SWI5_EGU5 */
  TIMER3_IRQn_Type = 26,           // 26 TIMER3 */
  TIMER4_IRQn_Type = 27,           // 27 TIMER4 */
  PWM0_IRQn_Type = 28,             // 28 PWM0 */
  PDM_IRQn_Type = 29,              // 29 PDM */
  MWU_IRQn_Type = 32,              // 32 MWU */
  PWM1_IRQn_Type = 33,             // 33 PWM1 */
  PWM2_IRQn_Type = 34,             // 34 PWM2 */
  SPIM2_SPIS2_SPI2_IRQn_Type = 35, // 35 SPIM2_SPIS2_SPI2 */
  RTC2_IRQn_Type = 36,             // 36 RTC2 */
  I2S_IRQn_Type = 37,              // 37 I2S */
  FPU_IRQn_Type = 38               // 38 FPU */
} int_Type;

void NVIC_EnableIRQ_Call( int_Type); //Enables Interrupt for a Peripheral
void NVIC_DisableIRQ_Call( int_Type); //Disables Interrupt for a Peripheral
void NVIC_SetPriority_Call(int_Type, int); //Sets the Priority of an Interrupt
void NVIC_ClearPendingIRQ_Call(int_Type); //Clears pending Interrupts to enable new ones

#endif
