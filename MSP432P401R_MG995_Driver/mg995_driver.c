/*
 * mg995_driver.c
 *
 *  Created on: Nov 1, 2019
 *      Author: Ted
 *
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
 *
 *  Servo 1 - P3.7 (temp)
 *
 */

#include "msp.h"
#include "mg995_driver.h"

void SPWM_init(void){
    // Setup clock system
    CS->KEY = CS_KEY_VAL ;                      // Unlock CS module for register access
    CS->CTL0 = 0;                               // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_0;               // Set DCO to 1.5MHz (nominal, center of 1-2MHz range)

    CS->CTL1 = 0;                               // REset tuning parameters
    CS-> CLKEN |= CS_CLKEN_REFOFSEL;            // Set REFO nominal frequency to 128Khz
    // Select ACLK = SMCLK = MCLK = REFO (128Khz)
    CS->CTL1 = CS_CTL1_SELM__REFOCLK | CS_CTL1_SELS__REFOCLK | CS_CTL1_SELA__REFOCLK;  // Set ACLK, SMCLK, MCLK to REFO clock (32.768Khz)

    CS->KEY = 0;                                // Lock CS module from unintended accesses

    // Setup Timer A
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK|    // SMCLK
                    TIMER_A_CTL_MC_1 |          // Up mode
                    TIMER_A_CTL_CLR;            // Clear TAR
                    //TIMER_A_CTL_IE;           // Enable overflow interrupt
    // Setup TA0 to generate PWM period of 20ms
    TIMER_A0->CCR[0] = PWM_PERIOD;              // PWM Period = 1300/64000 = 20ms
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled

    //Enable global interrupt
    __enable_irq();

    // Connect TA0_0 and TA0_N interrupt request to NVIC
    NVIC->ISER[0] |= ((1 << ((TA0_0_IRQn) & 31)) | (1 << ((TA0_N_IRQn) & 31)));   //TA1_0_IRQn

    P3->DIR |= BIT7;
    P3->OUT &= ~BIT7;
    setServoDegree(1,70);
}
uint16_t getServoDegree(uint16_t servoID){
    if(servoID <= 6 && servoID >= 1){
        return servoDegree[servoID-1];
    }
    return 0;
}
void setServoDegree(uint16_t servoID, uint16_t degree){
    if (degree > 180){
        degree = 180;
    } else if (degree < 0){
        degree = 0;
    }
    int16_t servoValue = (uint16_t)(((256.0/180.0) * degree) + 64);
    switch(servoID)
    {
        case(1):
        {
            s1PulseValue = servoValue;
            servoDegree[0] = degree;
            s1Active = true;
        }
        case(2):
        {
            s2PulseValue = servoValue;
        }
        case(3):
        {
            s3PulseValue = servoValue;
        }
        case(4):
        {
            s4PulseValue = servoValue;
        }
        case(5):
        {
            s5PulseValue = servoValue;
        }
        case(6):
        {
            s6PulseValue = servoValue;
        }
    }
}


void TA0_0_IRQHandler(void)
{
    P1OUT ^= BIT0;
    P3OUT &= ~BIT7;
    if (s1Active == true)
    {
        TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled
        TIMER_A0->CCR[1] = PWM_PERIOD - s1PulseValue;
        s1Active = false;
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Reset Flag
}

void TA0_N_IRQHandler(void)
{
    switch (TIMER_A0->IV)
    {
        case BIT1:
        {
            P3OUT |= BIT7;
            TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;  // Reset Flag
        }
    }
    //TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled
    //TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;  // Reset Flag

}
