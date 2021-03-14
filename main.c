#include "stm32g071xx.h"

#define DEBUG_LED 5 // PA5

typedef uint32_t uint32;


int main(void)
{
  // Clock at 64 MHz need to slow memory
  FLASH->ACR &= ~(0x00000017);
  // 2 wait states
  FLASH->ACR |= (FLASH_ACR_LATENCY_1 | FLASH_ACR_LATENCY_0 | FLASH_ACR_PRFTEN);

  // Prepare PLL module to use
  // disable PLL module
  // TODO ADD checking is actual Clock source is PLL RCC->CFGR SWS register
  if (RCC->CR & RCC_CR_PLLON)
  {
    RCC->CR &= ~(RCC_CR_PLLON);
    // wait untill PLL turn off
    while (RCC->CR & RCC_CR_PLLON) continue;
  }

  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLR)) | RCC_PLLCFGR_PLLR_0;
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLQ)) | RCC_PLLCFGR_PLLQ_0;
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLP)) | RCC_PLLCFGR_PLLP_0;
  RCC->PLLCFGR |= (RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLREN);
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLN)) | RCC_PLLCFGR_PLLN_3;
  RCC->PLLCFGR &= (~RCC_PLLCFGR_PLLM);
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLSRC)) | RCC_PLLCFGR_PLLSRC_1;

  // enable PLL again
  RCC->CR |= RCC_CR_PLLON;

  // wait untill PLL is ready
  while (!(RCC->CR & RCC_CR_PLLRDY)) continue;

  // HSI RC 16 MHz, LSI RC 32 kHz
  // Reading from flash prepared above, now speed up freq.
  RCC->CFGR |= RCC_CFGR_SW_1;

  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER = (GPIOA->MODER & (~GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_0;

 // Prepare timer to test 1 s

  RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

  RCC->APBENR1 |= RCC_APBRSTR1_TIM3RST;
  RCC->APBRSTR1 &= (~RCC_APBRSTR1_TIM3RST);
  // Disable TIMER3
  TIM3->CR1 &= ~(TIM_CR1_CEN);

  TIM3->PSC = 6399;
  TIM3->ARR = 10;

  TIM3->EGR |= TIM_EGR_UG;

  TIM3->DIER |= TIM_DIER_UIE;

  TIM3->CR1 |= TIM_CR1_CEN;

  NVIC_SetPriority(TIM3_IRQn, 0x00);
  NVIC_EnableIRQ(TIM3_IRQn);

  while (1)
  {
  }
}

volatile uint32 test_value = 500;

void TIM3_IRQHandler(void)
{
  static uint32 j = 0;
  if (TIM3->SR & TIM_SR_UIF)
  {
    j++;

    if (j >= test_value)
    {
      j = 0;
      GPIOA->ODR ^= GPIO_ODR_OD5;
    }
    TIM3->SR &= ~(TIM_SR_UIF);
  }
}
