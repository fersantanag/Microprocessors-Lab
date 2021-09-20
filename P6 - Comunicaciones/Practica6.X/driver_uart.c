/* 
 * File:   driver_uart.c
 * Author: Fernando & Jorge
 *
 * Created on 26 March 2021, 13:19
 */

#include <xc.h>
#include "driver_uart.h"

#define TAM_CAD 100

static char c_rx = '\0';
static int i_c_tx = 0;
static char c_tx[TAM_CAD];

void InicializarUART() {
    // Interrupciones
    IFS1bits.U1RXIF = 0;
    IEC1bits.U1RXIE = 1;
    IFS1bits.U1TXIF = 0;
    
    IPC8bits.U1IP = 3;      // Prioridad 3
    IPC8bits.U1IS = 1;      // Subprioridad 1
    
    
    // Inicializar UART
    U1BRG = 32;
    U1STAbits.URXISEL = 0;   // 1 char interrumpe
    U1STAbits.UTXISEL = 2;   // Se interrupme al estar vacñia
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;
    U1MODE = 0x8000;         // Activamos la UART
}

__attribute__ ((vector(32), interrupt(IPL3SOFT), nomips16))
void InterrupcionUART1(void) {
    
    if(IFS1bits.U1RXIF == 1) {
        c_rx = U1RXREG;
        IFS1bits.U1RXIF = 0;
    }
    
    if(IFS1bits.U1TXIF == 1) {
        if(c_tx[i_c_tx] == '\0') {
            IEC1bits.U1TXIE = 0;
            i_c_tx = 0;
            // c_tx[i_c_tx] = '\0'; // Para que sirve esto?
        } else {
            U1TXREG = c_tx[i_c_tx];
            i_c_tx++;
        }
        IFS1bits.U1RXIF = 0;
    }
}

char getcUART() {
    char c_ret = c_rx;
    c_rx = '\0';
    return c_ret;
}

void putsUART(char *ps) {
    char *pc;
    pc = c_tx;
    while(*ps != '\0') {
        *pc = *ps;
        pc++;
        ps++;
    }
    *pc = *ps;  // Hay que añadir \0 al final de la cadena, para el transmisor
    IEC1bits.U1TXIE = 1;
}