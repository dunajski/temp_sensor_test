#include "types.h"
#include "stm32g071xx.h"
#include "timer.h"
#include "gpio.h"

typedef enum
{
  MASTER_RESET_PULL_DOWN_TIME   = 480,
  MASTER_RX_PRESENCE_WAIT_TIME  = 480,
  DS_WAIT_AFTER_RESET_MIN_TIME  = 15,
  DS_WAIT_AFTER_RESET_MAX_TIME  = 60,
  DS_PRESENCE_PULSE_MIN_TIME    = 60,
  DS_PRESENCE_PULSE_MAX_TIME    = 240,
} EPulseLen;

typedef enum
{
  MASTER_TX_RESET_PULSE_START = 0x00,
  MASTER_TX_RESET_PULSE,
  MASTER_RX_WAIT_FOR_PRESENECE,
  SENSOR_PRESENCE_PULSE_START,
  MASTER_TX_COMMAND,
} TSensorState;

TSensorState state;

static uint8 STOP_MEASURE_TEMP = FALSE;

void SetDqPinAsOutput(void)
{
  SetGpioMode(TEMPSENSOR_DQ_PORT, TEMPSENSOR_DQ, _OUTPUT);
}

void SetDqPinAsInput(void)
{
  SetGpioMode(TEMPSENSOR_DQ_PORT, TEMPSENSOR_DQ, _INPUT);
}

void SetDqPinHigh(void)
{
  GPIOA->BSRR = GPIO_BSRR_BS9;
}

void SetDqPinLow(void)
{
  GPIOA->BSRR = GPIO_BSRR_BR9;
}

void ds18b20(void)
{
  // variable to make easier debuging
  if (STOP_MEASURE_TEMP) return;

  switch (state)
  {
    case MASTER_TX_RESET_PULSE_START:
      RestartTimer();
      SetDqPinAsOutput();
      SetDqPinLow();
      state = MASTER_TX_RESET_PULSE;
    break;

    case MASTER_TX_RESET_PULSE:
      if (CheckIsTimeElapsed(MASTER_RESET_PULL_DOWN_TIME))
      {
        SetDqPinAsInput();
        RestartTimer();
        state = MASTER_RX_WAIT_FOR_PRESENECE;
      }
    break;

    case MASTER_RX_WAIT_FOR_PRESENECE:
      // DS18B20 wait 15-60 us and then should pull-down line
      if (!CheckIsTimeElapsed(DS_WAIT_AFTER_RESET_MIN_TIME)) break;

      // after at least 15 us DS18B20 can pull line to ground
      if (!(GPIOA->IDR & GPIO_IDR_ID9))
      {
        state = SENSOR_PRESENCE_PULSE_START;
        RestartTimer();
        break;
      }

      // No sensors found after 480 us, restart machine
      if (CheckIsTimeElapsed(MASTER_RX_PRESENCE_WAIT_TIME))
      {
        state = MASTER_TX_RESET_PULSE_START;
        StopTimer();
        break;
      }
    break;

    case SENSOR_PRESENCE_PULSE_START:
      // now it should be pulled down for a 60 to 240 us
      if ((GPIOA->IDR & GPIO_IDR_ID9) && CheckIsTimeElapsed(DS_PRESENCE_PULSE_MIN_TIME))
      {
        // pull-down by DS18B20 done properly, now we can talk
        state = MASTER_TX_COMMAND;
        RestartTimer();
        break;
      }

      // if pin is pulled up before min time something went wrong, restart
      if (GPIOA->IDR & GPIO_IDR_ID9)
      {
        STOP_MEASURE_TEMP = TRUE;
        state = MASTER_TX_RESET_PULSE_START;
        break;
      }
    break;

    case MASTER_TX_COMMAND:
      // FIXME when press key restart machine
      // no need debouncing
      if (!(GPIOC->IDR & GPIO_IDR_ID13))
      {
        state = MASTER_TX_RESET_PULSE_START;
        GPIOA->ODR ^= GPIO_ODR_OD5;
        RestartTimer();
      }
    break;
    default:
    break;
  }
}