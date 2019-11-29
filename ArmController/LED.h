/*
 * LED.h
 *
 *  Created on: 28 Nov 2019
 *      Author: Xing Long
 */
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/* Driver Header files */
#include <ti/drivers/PWM.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "msp432_bluetooth.h"

#ifndef LED_H_
#define LED_H_

pthread_t ledThreadHandler;
void *ledThread(void *arg0);

#endif /* LED_H_ */
