/*
 * msp432_sensors.h
 *
 *  Created on: 27 Nov 2019
 *      Author: Xing Long
 */

/* POSIX Header files */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

/* Driver Header files */
#include <ti/drivers/ADC.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Additional Modules  */
#include <msp432_bluetooth.h>

#ifndef MSP432_SENSORS_H_
#define MSP432_SENSORS_H_

pthread_t flexSensorThreadHandler, pressureSensorThreadHandler;
void *flexSensorThread(void *arg0);
void *pressureSensorThread(void *arg0);

#endif /* MSP432_SENSORS_H_ */
