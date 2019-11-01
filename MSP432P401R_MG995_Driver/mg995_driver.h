/*
 * mg995_driver.h
 *
 *  Created on: Nov 1, 2019
 *      Author: Ted
 *
 * ==============================
 *              MG995
 * ==============================
 *
 * REFO Clock rate of 128Khz
 * - PWM_PERIOD Value of 2559 (2660) in CCR0 to generate
 *   a 50Hz / 20ms period.
 *
 *  Servo Angle   |     Pulse     |  Duty Cycle Value
 *  ---------------------------------------------------
 *  0 degrees     =     0.5ms     =   PWM_PERIOD - 64
 *  45 degrees    =     1.0ms     =   PWM_PERIOD - 128
 *  90 degrees    =     1.5ms     =   PWM_PERIOD - 192
 *  135 degrees   =     2.0ms     =   PWM_PERIOD - 256
 *  180 degrees   =     2.5ms     =   PWM_PERIOD - 320
 *
 *
 *  Formula to translate 0 to X degree to Software PWM value
 *  ((256 / (X degree range of servo)) * degree to turn to) + CCR_OFFSET
 *
 *  E.g. 0 to 180 degrees to Software PWM value
 *  ((256 / (180)) * degree to turn to) + 64
 *
 */

#ifndef MG995_DRIVER_H_
#define MG995_DRIVER_H_

#include <stdio.h>
#include <stdbool.h>

#define PWM_PERIOD 2559
#define CCR_OFFSET = 64

bool s1Active, s2Active, s3Active, s4Active, s5Active, s6Active;
volatile uint16_t s1PulseValue, s2PulseValue, s3PulseValue, s4PulseValue, s5PulseValue, s6PulseValue;
volatile uint16_t servoDegree[6];

void SPWM_init(void);

uint16_t getServoDegree(uint16_t servoID);
void setServoDegree(uint16_t servoID, uint16_t degree);
void TA0_0_IRQHandler(void);
void TA0_N_IRQHandler(void);



#endif /* MG995_DRIVER_H_ */
