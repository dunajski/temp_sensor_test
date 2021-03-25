#include "types.h"
#include "timer.h"

// TIM2 here is used

void StartTimer(void)
{
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM2->EGR |= TIM_EGR_UG;
}

void RestartTimer(void)
{
  TIM2->EGR |= TIM_EGR_UG;
}

void StopTimer(void)
{
  TIM2->CR1 &= (~TIM_CR1_CEN);
}

uint8 CheckIsTimeElapsed(uint32 timer_cnt)
{
  uint32 actual_cnt = TIM2->CNT;
  if (actual_cnt >= timer_cnt) return TRUE;
  else return FALSE;
}

uint8 IsTimerOn(void)
{
  if (TIM2->CR1 & TIM_CR1_CEN) return TRUE;
  else return FALSE;
}

volatile uint32 test_value = 1000;

void TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_SR_UIF)
  {
    TIM2->SR &= ~(TIM_SR_UIF);
  }
}

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
      //GPIOA->ODR ^= GPIO_ODR_OD5;
    }
  }
}

void TIM3_Init(void)
{
  // Prepare timer to test 1 s
  RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

  RCC->APBENR1 |= RCC_APBRSTR1_TIM3RST;
  RCC->APBRSTR1 &= (~RCC_APBRSTR1_TIM3RST);
  // Disable TIMER3
  TIM3->CR1 &= ~(TIM_CR1_CEN);

  // I would like to better use 63999 and 1, but now I can't
  // ...and don't know why
  TIM3->PSC = 6399;
  TIM3->ARR = 10;

  TIM3->EGR |= TIM_EGR_UG;

  TIM3->DIER |= TIM_DIER_UIE;

  TIM3->CR1 |= TIM_CR1_CEN;

  NVIC_SetPriority(TIM3_IRQn, 0x00);
  NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM2_Init(void)
{
  // Prepare timer to test 1 s
  RCC->APBENR1 |= RCC_APBENR1_TIM2EN;

  RCC->APBENR1 |= RCC_APBRSTR1_TIM2RST;
  RCC->APBRSTR1 &= (~RCC_APBRSTR1_TIM2RST);
  // Disable TIMER2
  TIM2->CR1 &= ~(TIM_CR1_CEN);

  // 64 MHz / (63 + 1) = 1 MHz
  // 1 CNT == 1 uS
  TIM2->PSC = 63;
  TIM2->ARR = 0xFFFFFFFF;

  TIM2->EGR |= TIM_EGR_UG;

  TIM2->DIER |= TIM_DIER_UIE;

  TIM2->CR1 |= TIM_CR1_CEN;

  NVIC_SetPriority(TIM2_IRQn, 0x00);
  NVIC_EnableIRQ(TIM2_IRQn);
}
