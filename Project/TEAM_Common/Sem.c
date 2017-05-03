/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

#include "Platform.h" /* interface to the platform */
#if PL_CONFIG_HAS_SEMAPHORE
#include "FRTOS1.h"
#include "Sem.h"
#include "LED.h"

static xSemaphoreHandle sem = NULL;

static void vSlaveTask(void *pvParameters) {
	while (sem == NULL) {
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	for (;;) {
		/*! \todo Implement functionality */
		if (xSemaphoreTake(sem, portMAX_DELAY) == pdPASS) { /* block on semaphore */
			//LED1_Neg();
		}
	}

	static void vMasterTask(void *pvParameters) {
		/*! \todo send semaphore from master task to slave task */
		while (sem == NULL) {
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		for (;;) {
			/*! \todo Implement functionality */
			(void) xSemaphoreGive(sem); /* give control to other task */
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}

	void SEM_Deinit(void) {
	}

	/*! \brief Initializes module */
	void SEM_Init(void) {
		if (xTaskCreate(vMasterTask, "Master", configMINIMAL_STACK_SIZE, NULL,
				tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			for (;;) {
			} /* error */
		}
	}
#endif /* PL_CONFIG_HAS_SEMAPHORE */
