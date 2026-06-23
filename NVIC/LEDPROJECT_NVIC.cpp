//NVIC.cpp: Cpp code for NVIC Interrupts
#include "LEDPROJECT_NVIC.h"

//*((volatile uint32_t *)((uint8_t *)NRF_PDM + (uint32_t)NRF_PDM_EVENT_END)) = 0x0UL;
//Base Address + Desired Register Address = Value to change
//8 Registers for NVIC [0:7]
//Each Interrupt + 4 bits

//Enables Interrupt for a Peripheral by setting to 1
void NVIC_EnableIRQ_Call(int_Type IRQn){
  uint32_t ISER_Register = ((uint32_t)(int32_t)IRQn) >> 5UL;
  *((volatile uint32_t *)((uint8_t *)NVIC_BASE_ADDRESS + (uint32_t)(NVIC_ISER_OFFSET + (ISER_Register * 4)))) = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

//Disables Interrupt for a Peripheral by setting to 1
void NVIC_DisableIRQ_Call(int_Type IRQn){  
  uint32_t ICER_Register = ((uint32_t)IRQn) >> 5UL;
  *((volatile uint32_t *)((uint8_t *)NVIC_BASE_ADDRESS + (uint32_t)(NVIC_ICER_OFFSET + (ICER_Register * 4)))) = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

//Sets the Priority of an Interrupt by shifting priority 5 bits to the left & 0xFF (256 levels of priority)
void NVIC_SetPriority_Call(int_Type IRQn, int priority){
  uint32_t IP_Register = (uint32_t)IRQn;
  *((volatile uint32_t *)((uint8_t *)NVIC_BASE_ADDRESS + (uint32_t)(NVIC_IP_OFFSET + (IP_Register * 4)))) = (uint8_t)((priority << (8U - NVIC_PRIORITY_BITS)) & (uint32_t)0xFFUL);
}

//Clears pending Interrupts to enable new ones by setting to 1
void NVIC_ClearPendingIRQ_Call(int_Type IRQn){  
  uint32_t ICPR_Register = ((uint32_t)IRQn) >> 5UL;
  *((volatile uint32_t *)((uint8_t *)NVIC_BASE_ADDRESS + (uint32_t)(NVIC_ICPR_OFFSET + (ICPR_Register * 4)))) = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}