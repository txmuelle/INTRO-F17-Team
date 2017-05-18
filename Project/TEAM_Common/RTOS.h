/**
 * \file
 * \brief RTOS interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module the main tasks of the RTOS.
 */

#ifndef RTOS_H_
#define RTOS_H_

#include "Platform.h"

#if PL_CONFIG_HAS_RTOS

static bool RTOS_ButtonPressed = FALSE;
void RTOS_ButtonEvent(void);

/*! \brief just blinking LEDs */
static void RTOS_BlinkyTask(void *pvParameters);

/*! \brief Initializes the RTOS module */
void RTOS_Init(void);

/*! \brief De-Initializes the RTOS module */
void RTOS_Deinit(void);

void RTOS_Run(void);

/*! \brief App Task */
static void AppTask(void* param);

/*! \brief Not used KillMe Task */
void KillMe(void* param);

#endif /* PL_CONFIG_HAS_RTOS */

#endif /* RTOS_H_ */
