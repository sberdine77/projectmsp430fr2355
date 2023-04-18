#include <msp430.h> 
/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR235x Demo - Timer0_B3, Toggle P1.0, CCR0 Count Mode ISR, DCO SMCLK
//
//  Description: Toggle P1.0 using software and TB_0 ISR. Timer0_B is
//  configured for continuous mode, thus the timer overflows when TBR counts
//  to CCR0. In this example, CCR0 is loaded with 50000.
//  ACLK = n/a, MCLK = SMCLK = TBCLK = default DCO = ~1MHz
//
//           MSP430FR2355
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |           P1.0|-->LED
//
//
//   Darren Lu
//   Texas Instruments Inc.
//   Oct. 2016
//   Built with IAR Embedded Workbench v6.50 & Code Composer Studio v6.2
//******************************************************************************
#include <msp430.h>
#include <stdio.h>
unsigned int lixo;

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;                     // Stop WDT


    // Configure GPIO
    P1DIR |= BIT0 | BIT2 | BIT3;                                // P1.0 output
    P1REN |= BIT0 | BIT2 | BIT3;
    P1OUT |= BIT0 | BIT2 | BIT3;                                // P1.0 high

    P1SEL0 &=~BIT6;
    P1SEL1 |=BIT6;
    P1REN |= BIT6;
    P1DIR |= BIT6;

    P3SEL0 |= BIT4 | BIT0;
    P3DIR |= BIT4 | BIT0;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    /*
     *    bits configuration for TB0CTL register
     *
            TBCLGRP_0 //Each TBxCLn latch loads independently
            TBCLGRP_1
            TBCLGRP_2
            TBCLGRP_3
            | // Counter length
            CNTL_0 // 16-bit, TBxR(max) = 0FFFFh
            CNTL_1 //12-bit, TBxR(max) = 0FFFh
            CNTL_2 //10-bit, TBxR(max) = 03FFh
            CNTL_3 // 8-bit, TBxR(max) = 0FFh
            | // Timer_B clock source select
            TBSSEL_0 //TBxCLK
            TBSSEL_1 //ACLK
            TBSSEL_2 //SMCLK
            TBSSEL_3 //INCLK
            | // input divider. These bits, along with the TBIDEX bits, select the divider for the input clock
            ID_0 // Div 1
            ID_1 // Div 2
            ID_2 // Div 4
            ID_3 // Div 8
            | // Mode control. Set MC = 00h when Timer_B is not in use to conserve power.
            MC_0 // Stop mode: Timer is halted
            MC_1 // Up mode: Timer counts up to TBxCL0
            MC_2 // Continuous mode: Timer counts up to the value set by CNTL
            MC_3 // Up/down mode: Timer counts up to TBxCL0 and down to 0000h
            | TBCLR // Timer_B clear. Setting this bit clears TBxR, the clock divider logic (the divider setting remains unchanged), and the count direction
            | // Timer_B interrupt enable.
            TBIE_0 // interrupt disable.
            TBIE_1 // interrupt enable.
    */

    TB0CTL |= TBCLGRP_0 | CNTL_0 | TBSSEL_2 | ID_3 | MC_1 | TBIE_1;


  /*
   *   bits configuration for TB0CCTLn register
   *
            CM_0 // No capture
            CM_1 // Capture on rising edge
            CM_2 // Capture on falling edge
            CM_3 // Capture on both rising and falling edges
             |  // Capture/compare input select.
            CCIS_0 // CCIxA
            CCIS_1 // CCIxB
            CCIS_2 // GND
            CCIS_3 // VCC
             | // Synchronize capture source
             SCS_0 //  Asynchronous capture
             SCS_1 //  Synchronous capture
             | //Compare latch load.
             CLLD_0 // TBxCLn loads on write to TBxCCRn
             CLLD_1 // TBxCLn loads when TBxR counts to 0
             CLLD_2 // TBxCLn loads when TBxR counts to 0 (up or continuous mode). TBxCLn loads when TBxR counts to TBxCL0 or to 0 (up/down mode).
             CLLD_3 // TBxCLn loads when TBxR counts to TBxCL
             | // Capture mode
             CAP_0 // Compare mode
             CAP_1 // Capture mode
             | // Output mode
             OUTMOD_0 //  OUT bit value
             OUTMOD_1 //  Set
             OUTMOD_2 //  Toggle/reset
             OUTMOD_3 //  Set/reset
             OUTMOD_4 //  Toggle
             OUTMOD_5 //  Reset
             OUTMOD_6 //  Toggle/set
             OUTMOD_7 //  Reset/set
             | // Capture/compare interrupt enable.
             CCIE_0 // Interrupt disabled
             CCIE_1 //  Interrupt enabled
             | // Output. For output mode 0, this bit directly controls the state of the outpu
             OUT_0 // Output low
             OUT_1 // Output high
             | // Capture overflow. This bit indicates a capture overflow occurred. COV must be reset with software
             COV_0 // No capture overflow occurred
             COV_1 // Capture overflow occurred
  */

    TB0CCTL0 |= CM_0 | CCIE_1;
    TB0CCTL1 |= CM_0 | CAP_0 | OUTMOD_3 | OUT_1;

             TB0CCR0 = 55000;
             TB0CCR1 = 55000;




    __bis_SR_register(GIE);           // Enable interrupts

    while (1){
      __no_operation();                             // For debug
      //P1OUT ^=BIT0;
    }
}

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
    P1OUT ^= BIT0;
}

 //Timer B1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer_B1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) Timer_B1 (void)
#else
#error Compiler not supported!
#endif
{
    //lixo = TB0IV;
    //P1OUT ^= BIT3;
    printf("Teste B1");
}
