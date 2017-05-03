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
#include "KeyDebounce.h"
#include "CLS1.h"
#include "KIN1.h"
#include "RTOS.h"
#include "Shell.h"
#if PL_CONFIG_HAS_KEYS
#include "Keys.h"
#endif
#if PL_CONFIG_HAS_SHELL
#include "CLS1.h"
#include "Shell.h"
#include "RTT1.h"
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
#include "LCDMenu.h"
#endif

LCDMenu_MenuItem menu;

#if PL_CONFIG_HAS_EVENTS

static void BtnMsg(int btn, const char *msg) {
#if PL_CONFIG_HAS_SHELL
#if PL_CONFIG_HAS_SHELL_QUEUE
	uint8_t buf[48];

	UTIL1_strcpy(buf, sizeof(buf), "Button pressed: ");
	UTIL1_strcat(buf, sizeof(buf), msg);
	UTIL1_strcat(buf, sizeof(buf), ": ");
	UTIL1_strcatNum32s(buf, sizeof(buf), btn);
	UTIL1_strcat(buf, sizeof(buf), "\r\n");
	SHELL_SendString(buf);
#else
	CLS1_SendStr("Button pressed: ", CLS1_GetStdio()->stdOut);
	CLS1_SendStr(msg, CLS1_GetStdio()->stdOut);
	CLS1_SendStr(": ", CLS1_GetStdio()->stdOut);
	CLS1_SendNum32s(btn, CLS1_GetStdio()->stdOut);
	CLS1_SendStr("\r\n", CLS1_GetStdio()->stdOut);
#endif
#endif
}

void APP_EventHandler(EVNT_Handle event) {
	/*! \todo handle events */
	switch (event) {
	case EVNT_STARTUP: {
		int i;
		for (i = 0; i < 5; i++) {
			LED1_Neg();
			WAIT1_Waitms(50);
		}
		LED1_Off();
	}
#if PL_CONFIG_HAS_BUZZER
		(void)BUZ_PlayTune(BUZ_TUNE_WELCOME);
#endif
		break;
	case EVNT_LED_HEARTBEAT:
		//LED2_Neg();
		break;
#if PL_CONFIG_NOF_KEYS>=1
	case EVNT_SW1_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_RIGHT, &menu);
		BtnMsg(1, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
#if PL_CONFIG_HAS_BUZZER
		(void)BUZ_PlayTune(BUZ_TUNE_BUTTON);
#endif
		break;
	case EVNT_SW1_LPRESSED:
		BtnMsg(1, "long pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
#if PL_CONFIG_HAS_BUZZER
		(void)BUZ_PlayTune(BUZ_TUNE_BUTTON);
#endif
		break;
	case EVNT_SW1_RELEASED:
		BtnMsg(1, "released");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
#if PL_CONFIG_HAS_BUZZER
		(void)BUZ_PlayTune(BUZ_TUNE_BUTTON);
#endif
		break;
#endif
#if PL_CONFIG_NOF_KEYS>=2
	case EVNT_SW2_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_LEFT, &menu);
		BtnMsg(2, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
		break;
#endif
#if PL_CONFIG_NOF_KEYS>=3
	case EVNT_SW3_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_DOWN, &menu);
		BtnMsg(3, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
		break;
#endif
#if PL_CONFIG_NOF_KEYS>=4
	case EVNT_SW4_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_ENTER, &menu);
		BtnMsg(4, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
		break;
#endif
#if PL_CONFIG_NOF_KEYS>=5
	case EVNT_SW5_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_UP, &menu);
		BtnMsg(5, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
		break;
#endif
#if PL_CONFIG_NOF_KEYS>=6
	case EVNT_SW6_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_INCREMENT, &menu);
		BtnMsg(6, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
		break;
#endif
#if PL_CONFIG_NOF_KEYS>=7
	case EVNT_SW7_PRESSED:
		LCDMenu_OnEvent(LCDMENU_EVENT_DECREMENT, &menu);
		BtnMsg(7, "pressed");
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
		break;
#endif
	default:
		break;
	} /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

#if PL_CONFIG_HAS_MOTOR /* currently only used for robots */
static const KIN1_UID RoboIDs[] = {
	/* 0: L20, V2 */{ {0x00,0x03,0x00,0x00,0x67,0xCD,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
	/* 1: L21, V2 */{ {0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
	/* 2: L4, V1  */{ {0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}}, /* revert right motor */
	/* 3: L23, V2 */{ {0x00,0x0A,0x00,0x00,0x67,0xCD,0xB8,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert left & right motor */
	/* 4: L11, V2 */{ {0x00,0x19,0x00,0x00,0x67,0xCD,0xB9,0x11,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert right encoder, possible damaged motor driver? */
	/* 5: L4, V1 */{ {0x00,0x0B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}},
};
#endif

static void APP_AdoptToHardware(void) {
	KIN1_UID id;
	uint8_t res;

	res = KIN1_UIDGet(&id);
	if (res != ERR_OK) {
		for (;;)
			; /* error */
	}
#if PL_CONFIG_HAS_MOTOR
	if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* L4 */
		MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
		(void)Q4CLeft_SwapPins(TRUE);
		(void)Q4CRight_SwapPins(TRUE);
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

void APP_Start(void) {
	int cntr;

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
#if PL_CONFIG_HAS_RTOS
	RTOS_Run();
	//SHELL_Init();
	//Labs: Tasks
	vTaskStartScheduler(); /* start the RTOS, create the IDLE task and run my tasks (if any) */
	/* does usually not return! */

#else

	//__asm volatile("cpsie i"); /* enable interrupts */
	cntr = 0;
	for(;;) {

		//CLS1_SendStr("hello ", CLS1_GetStdio()->stdOut);
		//CLS1_SendNum32s(cntr, CLS1_GetStdio()->stdOut);
		//CLS1_SendStr("\r\n", CLS1_GetStdio()->stdOut);
		//cntr++;
		//WAIT1_Waitms(50); /* just wait for some arbitrary time .... */
		//LED1_Neg();
		//KEY_Scan(); /* poll keys */
		//KEYDBNC_Process();
		//EVNT_HandleEvent(APP_EventHandler, TRUE);

	}
#endif
}
