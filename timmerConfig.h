/*
 * timmerConfig.h
 *
 *  Created on: May 2, 2023
 *      Author: savioberdine
 */

#ifndef TIMMERCONFIG_H_
#define TIMMERCONFIG_H_

unsigned int lixo;
volatile long mili = 0;

// Timer B0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer_B (void)
#else
#error Compiler not supported!
#endif
{
//    P1OUT ^= BIT0;
    lixo = TB0IV;
    mili++;
}

////Timer B1 interrupt service routine
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector = TIMER0_B1_VECTOR
//__interrupt void Timer_B1 (void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) Timer_B1 (void)
//#else
//#error Compiler not supported!
//#endif
//{
//   lixo = TB0IV;
//   mili++;
//   //printf("Teste B1");
//}

#endif /* TIMMERCONFIG_H_ */
