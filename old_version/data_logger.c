#include <msp430.h> 
#include <stdint.h>
#include <string.h>
unsigned int lixo;

void Init_GPIO();
void Software_Trim();                       // Software Trim to get the best DCOFTRIM value
#define MCLK_FREQ_MHZ 8                     // MCLK = 8MHz

#define BUFFER_SIZE 100
#define TRUE 1
#define FALSE 0
#define COMMBUFFERLENGH 16
volatile int  TXBufferEmpty0, TXBufferEmpty1, ptrWrCH0, ptrWrCH1, ptrRdCH0, ptrRdCH1 ;
char bufferRxCH0[COMMBUFFERLENGH], bufferRxCH1[COMMBUFFERLENGH];
long long i;
char linha[16];

uint16_t adc_buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;
static const uint32_t user_interval_time = 100;

char myGetChar (int canal){
    char dado;
    if(canal==0){
    if (ptrRdCH0 == ptrWrCH0)
        return (0);
    dado = bufferRxCH0[ptrRdCH0];
    ptrRdCH0++;
    ptrRdCH0 &=COMMBUFFERLENGH;
    return(dado);
    }else {
        if (ptrRdCH1 == ptrWrCH1)

            return (0);
        dado = bufferRxCH1[ptrRdCH1];
        ptrRdCH1++;
        ptrRdCH1 &=COMMBUFFERLENGH-1;
        return(dado);
    }
}

void  myGetLine (char * buffer, int maxBuffer, int canal){
    char dado;
    int ptr = 0;
    do {
        dado = myGetChar(canal);
        if (dado){
          buffer[ptr++] = dado;
           maxBuffer--;
           if (dado=='\r'){
               buffer[ptr]=0;
               return;
           }
        }


    }
    while (maxBuffer);
    buffer[ptr]=0;
}

void setup_uart() {

      PM5CTL0 &= ~LOCKLPM5;                    // Disable the GPIO power-on default high-impedance mode
                                               // to activate 1previously configured port settings

      __bis_SR_register(SCG0);                 // disable FLL
      CSCTL3 |= SELREF__REFOCLK;               // Set REFO as FLL reference source
      CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3;// DCOFTRIM=3, DCO Range = 8MHz
      CSCTL2 = FLLD_0 + 243;                  // DCODIV = 8MHz
      __delay_cycles(3);
      __bic_SR_register(SCG0);                // enable FLL
      Software_Trim();                        // Software Trim to get the best DCOFTRIM value

      CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                               // default DCODIV as MCLK and SMCLK source

      P2OUT |= BIT3;
      P2REN |= BIT3;
      P2IES |= BIT3;
      P2IE |= BIT3;

      P2IFG &= ~(BIT3);

      // Configure UART pins
      P1SEL0 |= BIT6 | BIT7;                    // set 2-UART pin as second function
      P4SEL0 |= BIT2 | BIT3;                    // set 2-UART pin as second function

      // Configure UART
      UCA0CTLW0 |= UCSWRST;
      UCA1CTLW0 |= UCSWRST;

      UCA0CTLW0 |= UCSSEL__SMCLK;
      UCA1CTLW0 |= UCSSEL__SMCLK;

      // Baud Rate calculation
      // 8000000/(16*9600) = 52.083
      // Fractional portion = 0.083
      // User's Guide Table 17-4: UCBRSx = 0x49
      // UCBRFx = int ( (52.083-52)*16) = 1
      UCA0BR0 = 52;                             // 8000000/16/9600
      UCA1BR0 = 52;                             // 8000000/16/9600
      UCA0BR1 = 0x00;
      UCA1BR1 = 0x00;
      UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;
      UCA1MCTLW = 0x4900 | UCOS16 | UCBRF_1;

      UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
      UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
      UCA0IE |= UCRXIE | UCTXIE;                         // Enable USCI_A0 RX interrupt
      UCA1IE |= UCRXIE | UCTXIE;                         // Enable USCI_A0 RX interrupt
      TXBufferEmpty0 = TRUE;
      TXBufferEmpty1 = TRUE;
}

void setup() {
    //Configurar clock

    //Configurar pinos

    //Configurar ADC

    //Configurar comunicação serial
}

uint16_t capture_sample() {
    // Inicia a conversão ADC
    ADCCTL0 |= ADCENC | ADCSC;

    // Aguarda a conversão terminar
    while (ADCCTL1 & ADCBUSY);

    // Lê o valor convertido
    uint16_t adc_value = ADCMEM0;

    // Mapeia o valor de 0 a 4095 (valor máximo do ADC) para 0 a 1500 mV
    uint16_t mv_value = (adc_value * 1500) / 4095;

    return mv_value;
}

void delay(uint32_t ms) {
    // Configura o timer A para gerar uma interrupção a cada 1ms
    TB0CCTL0 = CCIE;                // Habilita interrupção do comparador de captura/compare 0
    TB0CCR0 = 1000 - 1;             // Valor de comparação para 1ms
    TB0CTL = TBSSEL__ACLK | MC__UP; // Configura o timer A com clock ACLK (32768 Hz) e modo UP

    // Aguarda o tempo especificado pelo usuário
    uint32_t i = 0;
    for (i = ms; i != 0; i--) {
        __bis_SR_register(LPM3_bits | GIE); // Entra em modo de baixo consumo com interrupção habilitada
    }

    // Desativa o timer A
    TB0CTL = MC__STOP;
}

void capture_loop() {
    adc_buffer[buffer_index] = capture_sample();
    // Incrementa o índice do buffer circular
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;

    // Aguarda intervalo de tempo especificado pelo usuário
    delay(user_interval_time);
}

void serial_event() {

}


/**
 * main.c
 */
int main(void)
{
    char dado;

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Configure GPIO
    Init_GPIO();

    setup_uart();

    __bis_SR_register(GIE);         // Enter LPM3, interrupts enabled //LPM3_bits|
      while (1){
          myPuts("Hello Word 0\n\r", 0);
          myPuts("Hello Word 1\n\r", 1);
          for (i=0; i<100000; i++)
          __no_operation();                         // For debugger
          do {
              __no_operation();                         // For debugger
              myGetLine (linha, 16, 1);

              char * comando = linha;

              //myPuts(comando, 1);

              runCommand(comando);

              dado=strcmp (linha, "sair\n\r");

          } while(dado);

      }

	return 0;
}



void Software_Trim()
{
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do
    {
        CSCTL0 = 0x100;                         // DCO Tap = 256
        do
        {
            CSCTL7 &= ~DCOFFG;                  // Clear DCO fault flag
        }while (CSCTL7 & DCOFFG);               // Test DCO fault flag

        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);// Wait FLL lock status (FLLUNLOCK) to be stable
                                                           // Suggest to wait 24 cycles of divided FLL reference clock
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;                   // Read CSCTL0
        csCtl1Read = CSCTL1;                   // Read CSCTL1

        oldDcoTap = newDcoTap;                 // Record DCOTAP value of last time
        newDcoTap = csCtl0Read & 0x01ff;       // Get DCOTAP value of this time
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;// Get DCOFTRIM value

        if(newDcoTap < 256)                    // DCOTAP < 256
        {
            newDcoDelta = 256 - newDcoTap;     // Delta value between DCPTAP and 256
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }
        else                                   // DCOTAP >= 256
        {
            newDcoDelta = newDcoTap - 256;     // Delta value between DCPTAP and 256
            if(oldDcoTap < 256)                // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }

        if(newDcoDelta < bestDcoDelta)         // Record DCOTAP closest to 256
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);                      // Poll until endLoop == 1

    CSCTL0 = csCtl0Copy;                       // Reload locked DCOTAP
    CSCTL1 = csCtl1Copy;                       // Reload locked DCOFTRIM
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) PORT_2 (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(P2IV,P2IV_16)) {
    case P2IV_0 :
        _no_operation();
        break;
    case P2IV_2 :
        _no_operation();
        break;
    case P2IV_4 :
        _no_operation();
        break;
    case P2IV_6 :
        _no_operation();
        break;
    case P2IV_8 :
        _no_operation();
        char * myString = "Teste\n\r";
        myPuts(myString, 1);
        break;
    case P2IV_10 :
        _no_operation();
        break;
    case P2IV_12 :
        _no_operation();
        break;
    case P2IV_14 :
        _no_operation();
        break;
    case P2IV_16 :
        _no_operation();
        break;

    default: break;
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        bufferRxCH0[ptrWrCH0] = UCA0RXBUF;
        ptrWrCH0++;
        ptrWrCH0 &=COMMBUFFERLENGH-1;
        if (ptrWrCH0==ptrRdCH0){
            ptrRdCH0++;
            ptrRdCH0 &=COMMBUFFERLENGH-1;
        }
//      while(!(UCA0IFG&UCTXIFG));
//      UCA0TXBUF = UCA0RXBUF;
      __no_operation();
      break;
    case USCI_UART_UCTXIFG:
        TXBufferEmpty0 = TRUE;
        break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    default: break;
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
                __no_operation();
                bufferRxCH1[ptrWrCH1] = UCA1RXBUF;
                __no_operation();
                ptrWrCH1++;
                ptrWrCH1 &=COMMBUFFERLENGH-1;
                if (ptrWrCH1==ptrRdCH1){
                    ptrRdCH1++;
                    ptrRdCH1 &=COMMBUFFERLENGH-1;
                }
        //      while(!(UCA0IFG&UCTXIFG));
        //      UCA0TXBUF = UCA0RXBUF;
      __no_operation();
      break;
    case USCI_UART_UCTXIFG:
        TXBufferEmpty1 = TRUE;
        break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    default: break;
  }
}



void Init_GPIO()
{
    P1DIR = 0xFF; P2DIR = 0xFF;
    P1REN = 0xFF; P2REN = 0xFF;
    P1OUT = 0x00; P2OUT = 0x00;
}

