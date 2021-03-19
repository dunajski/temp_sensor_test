#ifndef __TIMER_H
#define __TIMER_H

#include "types.h"

void TIM2_Init(void);
void TIM3_Init(void);
void StartTimer(void);
void RestartTimer(void);
uint8 CheckTimer(uint32 timer_cnt);

#endif