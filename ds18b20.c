#include "types.h"
#include "stm32g071xx.h"
#include "timer.h"

// write bit
// read bit
// reset sequence

typedef enum
{
  MASTER_PULL_DOWN = 480,
  MASTER_RX_PRESENCE_WAIT = 480,
  DS_PRESENCE_PULSE_MIN = 60,
  DS_PRESENCE_PULSE_MAX = 240,
  DS_WAIT_AFTER_RESET_MIN = 15,
  DS_WAIT_AFTER_REST_MAX = 60,
} EPulseLen;

typedef enum
{
  MASTER_TX_INIT = 0x00,
  MASTER_RX_INIT,
  DS_PRESENCE_PULSE_START,
} TSensorState;

TSensorState state;

static uint8 STOP_MEASURE_TEMP = FALSE;

void ds18b20(void)
{
  // variable to make easier debuging
  if (STOP_MEASURE_TEMP)
    return;

  switch (state)
  {
    case MASTER_TX_INIT:
      if (!IsTimerOn())
      {
        StartTimer();
        // TODO MASTER pull-down for min. 480 us
        break;
      }

      if (CheckIsTimeElapsed(MASTER_PULL_DOWN))
      {
        // TODO MASTER release pull-down and go to RX
        RestartTimer();
        state = MASTER_RX_INIT;
        break;
      }
    break;
    case MASTER_RX_INIT:
      // No sensors found after 480 us, restart machine
      if (CheckIsTimeElapsed(MASTER_RX_PRESENCE_WAIT))
      {
        state = MASTER_TX_INIT;
        StopTimer();
        break;
      }

      // DS18B20 wait 15-60 us and then pull-down line
      if (!CheckIsTimeElapsed(DS_WAIT_AFTER_RESET_MIN)) break;

      // TODO Check that GPIO is pull-down by DS18B20, RestartTimer and change state

    break;

    case DS_PRESENCE_PULSE_START:
      // TODO check is GPIO pull-down by DS for 60-240 us
    break;
  }
}