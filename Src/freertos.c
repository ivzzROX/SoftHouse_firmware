
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "led_task.h"
#include "relay_task.h"
#include "sensor_task.h"
#include "esp8266_task.h"
#include "bit_engine_task.h"

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

void StartDefaultTask(void *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void)
{
	Led_TaskInit();
	Relay_TaskInit();

	ESP_TaskInit();

	BitEngine_TaskInit();
	Sensor_TaskInit();

	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
}

void StartDefaultTask(void *argument)
{
  for(;;)
  {
    osDelay(1);
  }
}

