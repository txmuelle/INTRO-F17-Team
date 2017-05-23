/*
 * Sumo.c
 *
 *  Created on: 16.05.2017
 *      Author: Müller Urs
 *      Titel: Sumo Battle Task
 *      Intro FS2017 - Hochschule Luzern T & A
 */
#include "Sumo.h"

#if PL_CONFIG_HAS_SUMO

typedef enum {
	SUMO_SET_MODE, SUMO_GET_MODE,
} SUMO_Commands;

typedef struct {
	SUMO_Commands cmd;
	union {
		Sumo_State state; /* Sumo_SET_status */
	} u;
} SUMO_Command;

#define QUEUE_LENGTH      4 /* number of items in queue, that's my buffer size */
#define QUEUE_ITEM_SIZE   sizeof(SUMO_Command) /* each item is a single drive command */
static xQueueHandle Sumo_Queue;

Sumo_State SUMO_State;
static uint8 SumoRunFlag = FALSE;
int afterCalib = 0;
#if PL_CONFIG_HAS_MOTOR
static bool WhiteLineDetected(void) {
	REF_LineKind line;
	line = REF_GetLineKind();
	return line != REF_LINE_FULL;
}
#endif

#if PL_CONFIG_HAS_EVENTS

/*
 *
 */
void Sumo_EventHandler(EVNT_Handle event) {

	switch (event) {

	case EVNT_SW1_PRESSED: //button short pressed
	{

		RTOS_ButtonPressed = TRUE;
		/*! \todo handle events */
	}
		break;
	case EVNT_SW1_LPRESSED: //button long pressed
	{
		afterCalib = 1;

		/*! \todo handle events */
	}
		break;
	case EVNT_SW1_RELEASED: //button released
	{

		/*! \todo handle events */
	}

	default:
		break;
	} /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

/* \brief is Sumo running?
 * \return true if Sumo is running
 */
bool SUMO_IsRunningSumo(void) {
	return SumoRunFlag;
}

/*! \brief Start Sumo with 2.Strategic */
void SUMO_Start1Sumo(void) {
	SumoRunFlag = TRUE;
	/*ToDo*/

}

/*! \brief Start Sumo with 1.Strategic */
void SUMO_StartSumo(void) {
	if (!SumoRunFlag) {
		//Todo search Opponent here
		SumoRunFlag = TRUE;
		SUMO_State = SUMO_STATE_ATTACKE;
	} else {
		/*ERROR: Sumo is already running*/
	}

}

void SUMO_calibrate(void) {

}

/*! \brief Stops Sumo */
void SUMO_StopSumo(void) {

#if PL_CONFIG_HAS_MOTOR
	DRV_SetMode(DRV_MODE_SPEED);
	DRV_SetSpeed(0, 0); //stop Motors
#endif

	SumoRunFlag = FALSE;
	SUMO_State = SUMO_STATE_OFF;
}

static void SumoTask(void *pvParameters) {

	portTickType xLastWakeTime;

	(void) pvParameters;
	xLastWakeTime = xTaskGetTickCount();
	for (;;) {
//    while (GetCmd()==ERR_OK) { /* returns ERR_RXEMPTY if queue is empty */
//      /* process incoming commands */
//    }
		//do something always here
#if PL_CONFIG_HAS_EVENTS
		EVNT_HandleEvent(Sumo_EventHandler, TRUE);
#endif

		switch (SUMO_State) {

		case SUMO_STATE_ATTACKE: {

			if (RTOS_ButtonPressed) {
				RTOS_ButtonPressed = FALSE;
#if PL_CONFIG_HAS_MOTOR
				DRV_SetMode(DRV_MODE_SPEED);
				DRV_SetSpeed(800, 800);
				//Search opponent

#endif
			}
#if PL_CONFIG_HAS_MOTOR
			if (WhiteLineDetected()) {
				DRV_SetSpeed(0, 0);
				TURN_TurnAngle(180, NULL);
				if (DIST_NearFrontObstacle(50)) {
					TURN_TurnAngle(30, NULL);
					DRV_SetSpeed(2000, 2000);
				}
				DRV_SetMode(DRV_MODE_SPEED);
				DRV_SetSpeed(800, 800);
				if (DIST_NearFrontObstacle(50)) {
					TURN_TurnAngle(30, NULL);
				}
				DRV_SetMode(DRV_MODE_SPEED);
				DRV_SetSpeed(800, 800);
			}
#endif
		}
			break;
		case SUMO_STATE_SEARCH_OPPONENT: {
			/*! \todo  */

		}
			break;

		case SUMO_STATE_CALIBRATE: {

			if (RTOS_ButtonPressed && !(afterCalib == 1)) {
				setState(2);
				REF_CalibrateStartStop();
			}
			if (afterCalib == 1) {
				setState(3);
				REF_CalibrateStartStop();
				SUMO_State = SUMO_STATE_ATTACKE;
				afterCalib = 0;
			}
			/*! \todo  */
		}
			break;

		case SUMO_STATE_OFF: {

#if PL_CONFIG_HAS_MOTOR
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
			DRV_SetMode(DRV_MODE_SPEED);
			DRV_SetSpeed(0, 0);
#endif
			/* turn the Sumo off */
		}
			break;
		default:

#if PL_CONFIG_HAS_MOTOR
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 30);
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), -30);
#endif
		}
		FRTOS1_vTaskDelayUntil(&xLastWakeTime, 5/portTICK_PERIOD_MS);
	} /* for */
}

void SUMO_Deinit(void) {
	FRTOS1_vQueueDelete(Sumo_Queue);
}

void SUMO_Init(void) {
	SUMO_State = SUMO_STATE_OFF;

	Sumo_Queue = FRTOS1_xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	if (Sumo_Queue == NULL) {
		for (;;) {
		} /* out of memory? */
	}
	FRTOS1_vQueueAddToRegistry(Sumo_Queue, "Sumo");
	SUMO_State = SUMO_STATE_CALIBRATE;
	/*if (FRTOS1_xTaskCreate(SumoTask, "Sumo", configMINIMAL_STACK_SIZE, NULL,
	 tskIDLE_PRIORITY+2, NULL) != pdPASS) {
	 for (;;) {
	 } /* error */

	if (FRTOS1_xTaskCreate(SumoTask, "Sumo", configMINIMAL_STACK_SIZE+60, NULL,
			tskIDLE_PRIORITY+3, NULL) != pdPASS) {
		for (;;) {
		} /* error */

	}
}

#endif /* PL_CONFIG_HAS_SUMO */
