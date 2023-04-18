# Project From Embeded Systems Discipline UFRPE

## To run this files you will need

* A MSP430FR2355
* Code Composer Studio 12
* Create a new project in Code Composer Studio

## Project Description (in Portuguese)

* Desenvolver um gerador de sinal configurável, com frequência de operação, amplitude e tipo de forma de onda selecionável por meio de comando recebido pela porta serial
* Formas de onda a serem geradas
  * Senoidal
  * Triangular
  * Quadrada
* Frequencia de operação no intervalo entre 1 e 10Khz
* Amplitude de pico da onda entre 0 e 1500 mV
* Sugestão de comandos: Sin 10 100 (senoide, 10Khz, 100 mV), Tri 3 500 (triagular, 3 Khz, 500 mV)
* Implementar um data logger configurável quanto ao intervalo de tempo entre capturas (freq. de operação), com 2 canais, que faça a captura de sinal na forma de um buffer circular com as 100 últimas amostras
* Os valores devem ser registrados em mV, no intervalo entre 0 e 1500 mV
* A leitura do buffer capturado se dará também por meio de comando recebido pela serial
* Sugestão de comando: Rd 0 (leitura do buffer 0), Rd 1 (leitura do buffer 1)
* Tanto o Gerador quanto o Data Logger devem dispor de um led que permaneça piscando a 1Hz para indicar que o dispositivo está funcionando corretamente
* Juntar os projetos do gerador e do data logger em um único projeto