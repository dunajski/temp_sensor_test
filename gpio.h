#ifndef __GPIO_H
#define __GPIO_H
#include "types.h"

// GPIOA
#define LD4_PORT GPIOA
#define LD4 5

#define TEMPSENSOR_DQ 9
#define TEMPSENSOR_DQ_PORT GPIOA

// GPIOC
#define BLUE_BUTTON_PORT GPIOC
#define BLUE_BUTTON 13

typedef enum
{
  _INPUT  =    0x0UL,
  _OUTPUT =    0x1UL,
  _ALTERNATE = 0x2UL,
  _ANALOG =    0x3UL,
} EGpioType;

typedef enum
{
  _LOW_SPEED  = 0x0UL, // 2 MHz
  _MID_SPEED  = 0x1UL, // 10 MHz
  _HIGH_SPEED = 0x3UL, // 50 MHz
} EGpioSpeed;

typedef enum
{
  _NO_PUPD = 0x0UL, // no pull-up or pull-down
  _PU      = 0x1UL, // pull-up
  _PD      = 0x2UL, // pull-down
} EGpioPuPd;

typedef enum
{
  _PUSHPULL = 0x1UL,
  _OPENDRAIN = 0x1UL,
} EGpioPuPdOd;

// Gpio Type macros, MODER related
#define SetGpioMode(_PORT, _PIN, _TYPE) (_PORT->MODER = (volatile uint32)((_PORT->MODER & (~(GPIO_MODER_MODE##_PIN))) | (_TYPE << (_PIN << 1))))
#define SetGpioAsAlternate(_PORT, _PIN) (SetGpioMode(_PORT, _PIN, _ALTERNATE))
#define SetGpioAsAnalog(_PORT, _PIN)    (SetGpioMode(_PORT, _PIN, _ANALOG))
#define SetGpioAsInput(_PORT, _PIN)     (SetGpioMode(_PORT, _PIN, _INPUT))
#define SetGpioAsOutput(_PORT, _PIN)    (SetGpioMode(_PORT, _PIN, _OUTPUT))

// NOTE copied, not tested
// Gpio Speed  macros, OSPEEDR related
#define SetGpioSpeed(_PORT, _PIN, _SPEED) (_PORT->OSPEEDR = (volatile uint32)((_PORT->OSPEEDR & (~(0x3UL << (_PIN << 1)))) | (_PORT->OSPEEDR | (_SPEED<< (_PIN << 1)))))
#define SetGpioToLowSpeed(_PORT, _PIN)     (SetGpioSpeed(_PORT, _PIN, _LOW_SPEED))
#define SetGpioToMidSpeed(_PORT, _PIN)     (SetGpioSpeed(_PORT, _PIN, _MID_SPEED))
#define SetGpioToHighSpeed(_PORT, _PIN)    (SetGpioSpeed(_PORT, _PIN, _HIGH_SPEED))

// NOTE copied, not tested
// Gpio pull-up/pull-down macros, PUPDR related
#define SetGpioPuPd(_PORT, _PIN, _PUPDTYPE) (_PORT->PUPDR = (volatile uint32)((_PORT->PUPDR & (~(0x3UL << (_PIN << 1)))) | (_PORT->PUPDR | (_PUPDTYPE << (_PIN << 1)))))
#define SetGpioPu(_PORT, _PIN) (SetGpioPuPd(_PORT, _PIN, _PU))
#define SetGpioPd(_PORT, _PIN)    (SetGpioPuPd(_PORT, _PIN, _PD))
#define SetGpioNoPuPd(_PORT, _PIN)     (SetGpioPuPd(_PORT, _PIN, _NO_PUPD))

// NOTE copied, not tested
#define SetOutputAsPushPull(_PORT, _PIN) (_PORT->OTYPER = (volatile uint32)(_PORT->OTYPER & (~(_PUSHPULL << _PIN))))
#define SetOutputAsOpenDrain(_PORT, _PIN) (_PORT->OTYPER = (volatile uint32)(_PORT->OTYPER | _OPENDRAIN << _PIN))

#define EnablePortGpio(_PORT)  (RCC->IOPENR |= RCC_IOPENR_##_PORT##EN)
#define DisablePortGpio(_PORT)  (RCC->IOPENR &= (~RCC_IOPENR_##_PORT##EN))

// #define SetGpio(_PORT, _PIN)   (_PORT->BSRR = (volatile uint32)(_PORT->BSRR | (GPIO_BSRR_BS##_PIN##_Msk)))
// #define ResetGpio(_PORT, _PIN) (_PORT->BSRR = (volatile uint32)(_PORT->BSRR | (GPIO_BSRR_BR##_PIN##_Msk)))

#endif
