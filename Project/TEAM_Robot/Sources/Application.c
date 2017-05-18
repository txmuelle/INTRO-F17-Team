/**
 * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "WAIT1.h"
#include "CS1.h"
#include "Keys.h"
#include "KeyDebounce.h"
#include "KIN1.h"
#include "Trigger.h"
#include "Motor.h"
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_BOARD_IS_ROBO_V2
  #include "PORT_PDD.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
  #include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LCD_MENU
  #include "LCD.h"
#endif

  /*temporary Global Variables for debug*/
  int cntr = 0;

  /******/

#if PL_CONFIG_HAS_EVENTS
void APP_EventHandler(EVNT_Handle event) {
  /*! \todo handle events */
  switch(event) {
  case EVNT_STARTUP:
  {
	  int i;
	  CLS1_SendStr("Welcome Master. I'm starting up and be ready for you soon.\r\n",SHELL_GetStdio()->stdOut);
#if PL_CONFIG_HAS_BUZZER
	  BUZ_PlayTune(BUZ_TUNE_WELCOME);
#endif
	  for(i=0; i < 3; i++){

	        LED2_Neg();        //blinking the LED on Startup
	        WAIT1_Waitms(100);
	  }/*for*/
	  LED2_Off();
  }
    break;

  /*only turn on for debug purpose*/
  case EVNT_LED_HEARTBEAT:{
	  TRG_AddTick();
     // LED1_On();
     // WAIT1_Waitms(500);
	 // LED1_Off();
  }
  break;
  case EVNT_SW1_PRESSED: //button short pressed
  {

//	    MOT_MotorDevice *MothandleLeft = MOT_GetMotorHandle(MOT_MOTOR_LEFT);
//	    MOT_MotorDevice *MothandleRight = MOT_GetMotorHandle(MOT_MOTOR_RIGHT);
//	    MOT_SetSpeedPercent(MothandleLeft, 100);
//	    MOT_SetSpeedPercent(MothandleRight, 100);

	    cntr++;
	    BUZ_PlayTune(BUZ_TUNE_BUTTON);
	    CLS1_printf("You pushed the button short %d times \r\n",cntr ,SHELL_GetStdio()->stdOut);
		/*blink LED1*/
// 		LED1_On();
// 		WAIT1_Waitms(500);
// 		LED1_Off();
  }
  break;
  case EVNT_SW1_LPRESSED: //button long pressed
  {
//	    MOT_MotorDevice *MothandleLeft = MOT_GetMotorHandle(MOT_MOTOR_LEFT);
//	    MOT_MotorDevice *MothandleRight = MOT_GetMotorHandle(MOT_MOTOR_RIGHT);
//	    MOT_SetSpeedPercent(MothandleLeft, 0);
//	    MOT_SetSpeedPercent(MothandleRight, 0);

	  BUZ_PlayTune(BUZ_TUNE_BUTTON_LONG);
	  CLS1_printf("You pushed the button long \r\n", SHELL_GetStdio()->stdOut);
  }
  break;
  case EVNT_SW1_RELEASED:
	  CLS1_printf("You released the button \r\n",cntr ,SHELL_GetStdio()->stdOut);

  default:
    break;
   } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

static const KIN1_UID RoboIDs[] = {
  /* 0: L20, V2 */ {{0x00,0x03,0x00,0x00,0x67,0xCD,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 1: L21, V2 */ {{0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 2: L4, V1  */ {{0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}}, /* revert right motor */
  /* 3: L23, V2 */ {{0x00,0x0A,0x00,0x00,0x67,0xCD,0xB8,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert left & right motor */
  /* 4: L11, V2 */ {{0x00,0x19,0x00,0x00,0x67,0xCD,0xB9,0x11,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert right encoder, possible damaged motor driver? */
  /* 5: L4, V1 */  {{0x00,0x0B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}},
  /* 6: L19, V2 */   {{0x00,0x29,0x00,0x00,0x67,0xCD,0xB4,0x51,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /*Robo von Urs revert left motor*/
};

static void APP_AdoptToHardware(void) {
  KIN1_UID id;
  uint8_t res;

  res = KIN1_UIDGet(&id);
  if (res!=ERR_OK) {
    for(;;); /* error */
  }
#if PL_CONFIG_HAS_MOTOR
  if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[6])) { /* L19 von Urs */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[0])) { /* L20 */
    (void)Q4CRight_SwapPins(TRUE);
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[3])) { /* L23 */
    (void)Q4CRight_SwapPins(TRUE);
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[4])) { /* L11 */
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[5])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  }
#endif
#if PL_CONFIG_HAS_QUADRATURE && PL_CONFIG_BOARD_IS_ROBO_V2
  /* pull-ups for Quadrature Encoder Pins */
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 10, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 10, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 11, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 11, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 16, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 16, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 17, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 17, PORT_PDD_PULL_ENABLE);
#endif
}

#include "CLS1.h"

void APP_Start(void) {
#if PL_CONFIG_HAS_RTOS
#if configUSE_TRACE_HOOKS /* FreeRTOS using Percepio Trace */
  #if TRC_CFG_RECORDER_MODE==TRC_RECORDER_MODE_SNAPSHOT
    vTraceEnable(TRC_START); /* start streaming */
  #elif TRC_CFG_RECORDER_MODE==TRC_RECORDER_MODE_STREAMING
  //  vTraceEnable(TRC_INIT); /* Streaming trace, start streaming */
  #endif
#endif /* configUSE_TRACE_HOOKS */
#endif
  PL_Init();
#if PL_CONFIG_HAS_EVENTS
  EVNT_SetEvent(EVNT_STARTUP);
#endif
#if PL_CONFIG_HAS_SHELL && CLS1_DEFAULT_SERIAL
  CLS1_SendStr((uint8_t*)"Hello World!\r\n", CLS1_GetStdio()->stdOut);
#endif
  APP_AdoptToHardware();
#if PL_CONFIG_HAS_RTOS
  vTaskStartScheduler(); /* start the RTOS, create the IDLE task and run my tasks (if any) */
  /* does usually not return! */
#else
#if PL_CONFIG_HAS_EVENTS
  EVNT_SetEvent(EVNT_STARTUP);
#endif

/*intit commands*/
  __asm volatile("cpsie i"); //Turn on interrupts
  //EVNT_Init();
  BUZ_Init();

/*main for loop*/
  for(;;) {

	  /*Eventhandler*/
	  EVNT_HandleEvent(APP_EventHandler, TRUE);
	  KEY_Scan();

	  /*write your Code here*/
	  WAIT1_Waitms(100); /* just wait for some arbitrary time .... */



  }

#endif
}


