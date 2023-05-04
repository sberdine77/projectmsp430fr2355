/*
 * stateManager.c
 *
 *  Created on: May 1, 2023
 *      Author: savioberdine
 */

#include <msp430.h>
#include <timmerConfig.h>

#define WAVE 0
#define LOGGER 1
#define SERIALREADER 2
#define LEDWAVE 3

int state = 0;

long ledCompare = 500;

void setupTimer() {
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

    //TB0CCTL0 |= CM_0 | CCIE_1;
    TB0CCTL1 |= CM_0 | CAP_0 | OUTMOD_3 | OUT_1;

             //TB0CCR0 = 55000;
             TB0CCR1 = 125;




    __bis_SR_register(GIE);           // Enable interrupts
}

void blink() {
    if(mili >= ledCompare) {
        ledCompare += 500;
        P1OUT ^= BIT3;
    }
}

void nextState() {
    if(state == LEDWAVE) {
        state = 0;
        return;
    }
    state += 1;
}
