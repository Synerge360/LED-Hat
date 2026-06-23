// Cpp file for setting GPIO pins to output

#include "LEDPROJECT_GPIO.h"

void Port0Output(int n)
{
  uint32_t Pin_Offset = Pin_Base + (uint32_t)(n * 4);

  *((volatile uint32_t *)((uint8_t *)Port0_Base + (uint32_t)Pin_Offset)) = (1UL << 0UL) |
                                                                           (0UL << 8UL) |
                                                                           (0UL << 1UL) |
                                                                           (0UL << 2UL) |
                                                                           (0UL << 16UL);
}

void Port0Input(int n)
{
  uint32_t Pin_Offset = Pin_Base + (uint32_t)(n * 4);

  *((volatile uint32_t *)((uint8_t *)Port0_Base + (uint32_t)Pin_Offset)) = (0UL << 0UL) |
                                                                           (0UL << 8UL) |
                                                                           (0UL << 1UL) |
                                                                           (0UL << 2UL) |
                                                                           (0UL << 16UL);
}

void Port1Output(int n)
{
  uint32_t Pin_Offset = Pin_Base + (uint32_t)(n * 4);

  *((volatile uint32_t *)((uint8_t *)Port1_Base + (uint32_t)Pin_Offset)) = (1UL << 0UL) |
                                                                           (0UL << 8UL) |
                                                                           (0UL << 1UL) |
                                                                           (0UL << 2UL) |
                                                                           (0UL << 16UL);
}

void Port1Input(int n)
{
  uint32_t Pin_Offset = Pin_Base + (uint32_t)(n * 4);

  *((volatile uint32_t *)((uint8_t *)Port1_Base + (uint32_t)Pin_Offset)) = (0UL << 0UL) |
                                                                           (0UL << 8UL) |
                                                                           (0UL << 1UL) |
                                                                           (0UL << 2UL) |
                                                                           (0UL << 16UL);
}