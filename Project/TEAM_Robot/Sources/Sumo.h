/*
 * sumo.h
 *
 *  Created on: 15.05.2017
 *      Author: Müller Urs
 *      Titel: Sumo Battle Task
 *      Intro FS2017 - Hochschule Luzern T & A
 */

#ifndef SOURCES_SUMO_H_
#define SOURCES_SUMO_H_

#include "RTOS.h"
#include "Platform.h"
#include "Drive.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "Tacho.h"
#include "Pid.h"
#include "Motor.h"
#include "Event.h"
#include "Keys.h"
#include "KeyDebounce.h"
#if PL_CONFIG_HAS_SHELL
#include "CLS1.h"
#endif
#include "Q4CLeft.h"
#include "Q4CRight.h"
#include "Shell.h"
#include "WAIT1.h"
#if PL_CONFIG_HAS_MOTOR
#include "Motor.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
#include "Reflectance.h"
#endif

typedef enum {
	SUMO_STATE_SEARCH_ROBOT,
	SUMO_STATE_SEARCH_BORDER,
	SUMO_STATE_ATTACKE,
	SUMO_STATE_DEFFENCE,
	SUMO_STATE_CALIBRATE,
	SUMO_STATE_OFF,
	SUMO_STATE_ERROR,
} Sumo_State;

/*!
 * \brief Returns the actual State of Sumo.
 * \return Actual State
 */
int32_t SUMO_GetState(void);

#if PL_CONFIG_HAS_SHELL
#include "CLS1.h"
/*!
 * \brief Parses a command
 * \param cmd Command string to be parsed
 * \param handled Sets this variable to TRUE if command was handled
 * \param io I/O stream to be used for input/output
 * \return Error code, ERR_OK if everything was fine
 */
uint8_t SUMO_ParseCommand(const unsigned char *cmd, bool *handled,
		const CLS1_StdIOType *io);
#endif/* PL_CONFIG_HAS_SHELL */

/*! \brief Start / Stops the module */
void SUMO_StartStopSumo(void);
void SUMO_StartSumo(void);
void SUMO_StopSumo(void);

/*! \brief Initialization of the module */
void SUMO_Init(void);

/*! \brief De-initialization of the module */
void SUMO_Deinit(void);

#endif /* SOURCES_SUMO_H_ */
