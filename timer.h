#ifndef __TIMER_H
#define __TIMER_H

#include "types.h"

void TIM2_Init(void);
void TIM3_Init(void);
void StartTimer(void);
void RestartTimer(void);
void StopTimer(void);
uint8 CheckIsTimeElapsed(uint32 timer_cnt);
uint8 IsTimerOn(void);

#endif