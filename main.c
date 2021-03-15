#include "stm32g071xx.h"
#include "types.h"

void PreparePLLAndChooseAsClock(void);
void SlowFlashMemory(void);

int main(void)
{
  SlowFlashMemory();
  PreparePLLAndChooseAsClock();

  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER = (GPIOA->MODER & (~GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_0;


  while (1)
  {
  }
}

volatile uint32 test_value = 1000;

void TIM3_IRQHandler(void)
{
  static uint32 j = 0;
  if (TIM3->SR & TIM_SR_UIF)
  {
    TIM3->SR &= ~(TIM_SR_UIF);
    j++;

    if (j >= test_value)
    {
      j = 0;
      GPIOA->ODR ^= GPIO_ODR_OD5;
    }
  }
}

// Function to set SysClock to 64 MHz
void PreparePLLAndChooseAsClock(void)
{
  // Turn off PLL module to change settings (mentioned in datasheet)
  if (RCC->CR & RCC_CR_PLLON)
  {
    RCC->CR &= ~(RCC_CR_PLLON);
    // wait untill PLL turn off
    while (RCC->CR & RCC_CR_PLLON) continue;
  }

  // HSI RC at 16 MHz
  // PLLM = HSI / M, M = 1, PLLM 16 MHz
  // PLLM register need to be set to 000 to get div by 1
  RCC->PLLCFGR &= (~RCC_PLLCFGR_PLLM);
  // PLLN register need to be set to 0b0001000 to get multiplication by 8
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLN)) | RCC_PLLCFGR_PLLN_3;
  // PLLQ/PLLR/PLLP = HSI/M*N/R(and Q and P)
  // PLLR/PLLRQ register need to be set to 0b001 to get div by 2
  // PLLP register need to be set to 0b00001 to get div by 2
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLR)) | RCC_PLLCFGR_PLLR_0;
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLQ)) | RCC_PLLCFGR_PLLQ_0;
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLP)) | RCC_PLLCFGR_PLLP_0;
  // Enable PLLQ/R/P Clocks set above
  RCC->PLLCFGR |= (RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLREN);
  
  // Change source of PLL to HSI
  RCC->PLLCFGR = (RCC->PLLCFGR & (~RCC_PLLCFGR_PLLSRC)) | RCC_PLLCFGR_PLLSRC_1;

  // enable PLL again
  RCC->CR |= RCC_CR_PLLON;

  // wait untill PLL is ready
  while (!(RCC->CR & RCC_CR_PLLRDY)) continue;
  // When PLL is ready change to PLLRCLK
  RCC->CFGR |= RCC_CFGR_SW_1;
}


void SlowFlashMemory(void)
{
  // Clock at 64 MHz need to slow memory
  FLASH->ACR &= ~(0x00000017);
  // 2 wait states
  FLASH->ACR |= (FLASH_ACR_LATENCY_1 | FLASH_ACR_LATENCY_0 | FLASH_ACR_PRFTEN);
}

void TIM3_Init(void)
{
  // Prepare timer to test 1 s
  RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

  RCC->APBENR1 |= RCC_APBRSTR1_TIM3RST;
  RCC->APBRSTR1 &= (~RCC_APBRSTR1_TIM3RST);
  // Disable TIMER3
  TIM3->CR1 &= ~(TIM_CR1_CEN);

  // dunno why I can't use PSC 63999 and ARR 1
  // but 
  TIM3->PSC = 6399;
  TIM3->ARR = 10;

  TIM3->EGR |= TIM_EGR_UG;

  TIM3->DIER |= TIM_DIER_UIE;

  TIM3->CR1 |= TIM_CR1_CEN;

  NVIC_SetPriority(TIM3_IRQn, 0x00);
  NVIC_EnableIRQ(TIM3_IRQn);
}
