/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "Keys.h"
#include "KeyDebounce.h"

static void RTOS_BlinkyTask(void *pvParameters) {
	for (;;) {
		LEDPin1_NegVal();
	}
}

static void RTOS_mainTask(void *pvParameters) {
	for (;;) {
		/*Eventhandler*/
		EVNT_HandleEvent(APP_EventHandler, TRUE);
		/*Key scanning*/
		KEY_Scan();

		/*write your Code here*/
		vTaskDelay(pdMS_TO_TICKS(100)); /* just wait for some arbitrary time .... */
	}
}

void RTOS_Init(void) {
	EVNT_SetEvent(EVNT_STARTUP); /* set startup event */
	/*! \todo Create tasks here */
	BaseType_t res;
	xTaskHandle taskHndl;

	if (xTaskCreate(RTOS_mainTask, (signed portCHAR *) "mainTask", 100,
			(void*) NULL,
			tskIDLE_PRIORITY, NULL) != pdPASS) {
		printf("ERROR!");
		/* error case only, stay here! */
	}

	res = xTaskCreate(RTOS_BlinkyTask, "Blinky", configMINIMAL_STACK_SIZE + 50,
			(void *) NULL, tskIDLE_PRIORITY, &taskHndl);
	if (res != pdPASS) {
		printf("ERROR!");
	}

}

void RTOS_Deinit(void) {
	/* nothing needed for now */
}

void RTOS_Run(void) {
	/*Use a function named RTOS Run() which will set up your tasks, and
	 then starts the scheduler.*/
	RTOS_Init();

}

#endif /* PL_CONFIG_HAS_RTOS */
