#include <msp430.h> 
#include <stdint.h>

#define BUFFER_SIZE 100

uint16_t adc_buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;
static const uint32_t user_interval_time = 100;

void setup() {
    //Configurar clock

    //Configurar pinos

    //Configurar ADC

    //Configurar comunicação serial
}

uint16_t capture_sample() {
    // Inicia a conversão ADC
    ADC12CTL0 |= ADC12ENC | ADC12SC;

    // Aguarda a conversão terminar
    while (ADC12CTL1 & ADC12BUSY);

    // Lê o valor convertido
    uint16_t adc_value = ADC12MEM0;

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


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	return 0;
}
