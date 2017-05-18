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
  SUMO_SET_MODE,
  SUMO_GET_MODE,
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

#if PL_CONFIG_HAS_MOTOR
static bool WhiteLineDetected(void) {
  REF_LineKind line;

  line = REF_GetLineKind();
  return line!=REF_LINE_FULL;
}
#endif

#if PL_CONFIG_HAS_EVENTS

/*
 *
 */
void Sumo_EventHandler(EVNT_Handle event) {

  switch(event) {

  case EVNT_SW1_PRESSED: //button short pressed
  {

	  /*! \todo handle events */
  }
  break;
  case EVNT_SW1_LPRESSED: //button long pressed
  {

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


/*
 *
 */
//line detectLine(){
//
//
//}

static void SumoTask(void *pvParameters) {
  portTickType xLastWakeTime;

  (void)pvParameters;
  xLastWakeTime = xTaskGetTickCount();
  for(;;) {
//    while (GetCmd()==ERR_OK) { /* returns ERR_RXEMPTY if queue is empty */
//      /* process incoming commands */
//    }
    //do something always here
#if PL_CONFIG_HAS_EVENTS
		EVNT_HandleEvent(Sumo_EventHandler, TRUE);
#endif

    if (SUMO_State==SUMO_STATE_SEARCH) {
    	/*! \todo  */
    } else if (SUMO_State==SUMO_STATE_ATTACKE) {

    	if (RTOS_ButtonPressed) {
          RTOS_ButtonPressed = FALSE;
    #if PL_CONFIG_HAS_MOTOR
          MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 10);
          MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 10);
    #endif
        }
    #if PL_CONFIG_HAS_MOTOR
        if (WhiteLineDetected()) {
          MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
          MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
        }
    #endif
    	/*! \todo  */
    } else if (SUMO_State==SUMO_STATE_DEFFENCE) {
    	/*! \todo  */
    } else if (SUMO_State==SUMO_STATE_CALIBRATE) {
    	/*! \todo  */
    } else if (SUMO_State==SUMO_STATE_OFF) {
      /* do nothing */
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
  if (Sumo_Queue==NULL) {
    for(;;){} /* out of memory? */
  }
  FRTOS1_vQueueAddToRegistry(Sumo_Queue, "Sumo");
  if (FRTOS1_xTaskCreate(SumoTask, "Sumo", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
    for(;;){} /* error */
  }
}

#endif /* PL_CONFIG_HAS_SUMO */
