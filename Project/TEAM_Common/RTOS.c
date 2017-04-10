/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "LED.h"

static void BlinkyTask(void *pvParameters) {
	for (;;) {
		LEDPin1_NegVal();
	}
}

void RTOS_Init(void) {
	/*! \todo Create tasks here */
	BaseType_t res;
	xTaskHandle taskHndl;
	res = xTaskCreate(BlinkyTask, "Blinky", configMINIMAL_STACK_SIZE + 50,
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
