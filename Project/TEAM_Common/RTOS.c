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
#if PL_CONFIG_HAS_MOTOR_TACHO
  #include "Tacho.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
  #include "Reflectance.h"
#endif





void RTOS_ButtonEvent(void) {
  RTOS_ButtonPressed = TRUE;
}

//static void RTOS_BlinkyTask(void *pvParameters) {
//	for (;;) {
//		LEDPin1_NegVal();
//		WAIT1_Waitms(100); /* just wait for some arbitrary time .... */
//	}
//}


#if 0
static void KillMe(void* param) {
  LED1_On(); /* LED of death! */
  vTaskDelete(NULL); /* suicide! */
}
#endif

static void AppTask(void* param) {
	const int *whichLED = (int*) param;
	bool start = FALSE;

	REF_LineKind
	RTOS_ButtonPressed = FALSE;
	for (;;) {
		if (*whichLED == 1) {
			LED1_Neg();
		} else if (*whichLED == 2) {
			LED2_Neg();
		}
#if PL_CONFIG_HAS_MOTOR_TACHO
    TACHO_CalcSpeed();
#endif
#if PL_CONFIG_HAS_KEYS
		//KEY_Scan();
		KEYDBNC_Process();
#endif
#if PL_CONFIG_HAS_EVENTS
		EVNT_HandleEvent(APP_EventHandler, TRUE);
#endif
#if PL_CONFIG_HAS_LINE_MAZE

#endif


		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void RTOS_Init(void) {
	/*! \todo Create tasks here */
	static const int led1 = 1;
	//static const int led2 = 2;

	EVNT_SetEvent(EVNT_STARTUP); /* set startup event */
	/*! \todo Create tasks here */
	if (xTaskCreate(AppTask, (signed portCHAR *) "App",
			500 / sizeof(StackType_t), (void*) &led1, tskIDLE_PRIORITY + 1,
			NULL) != pdPASS) {
		for (;;) {
		} /* error case only, stay here! */
	}
#if 0
	if (xTaskCreate(AppTask, (signed portCHAR *)"App2", 100/sizeof(StackType_t), (void*)&led2, tskIDLE_PRIORITY, NULL) != pdPASS) {
		for(;;) {} /* error case only, stay here! */
	}
#endif
#if 0
	if (xTaskCreate(KillMe, (signed portCHAR *)"KillMe", 100/sizeof(StackType_t), NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
		for(;;) {} /* error case only, stay here! */
	}
#endif
}

void RTOS_Deinit(void) {
	/* nothing needed for now */
}

void RTOS_Run(void) {
	/*Use a function named RTOS Run() which will set up your tasks, and
	 then starts the scheduler.*/
	//RTOS_Init(); don't need here
}

#endif /* PL_CONFIG_HAS_RTOS */
