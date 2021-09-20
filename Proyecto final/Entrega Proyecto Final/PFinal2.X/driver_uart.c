/* 
 * File:   driver_uart.c
 * Author: Fernando & Jorge
 *
 * Created on  March 2021, 13:19
 */

#include <xc.h>
#include "driver_uart.h"

#define TAM_COLA 100

typedef struct {
    char array[TAM_COLA];
    int cab;
    int col;
} cola_t;

static cola_t rx;
static cola_t tx;



void InicializarUART1(int baudios) {
    

    rx.cab = 0;
    rx.col = 0;
    tx.cab = 0;
    tx.col = 0;

    // Interrupciones
    IFS1bits.U1RXIF = 0;
    IEC1bits.U1RXIE = 1;
    IFS1bits.U1TXIF = 0;

    IPC8bits.U1IP = 3; // Prioridad 3
    IPC8bits.U1IS = 2; // Subprioridad 2

    U1MODE = 0;
    // Velocidad
    double brg;
    if (baudios > 38400) {
        U1MODEbits.BRGH = 1;
        brg = 5000000 / (4 * baudios) - 1;
    } else {
        brg = 5000000 / (16 * baudios) - 1;
    }
    if (baudios == 115200)
        U1BRG = 10;
    else
        U1BRG = (int) brg;

    // Inicializar UART

    U1STAbits.URXISEL = 0; // 1 char interrumpe
    U1STAbits.UTXISEL = 2; // Se interrupme al estar vacia
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;
    U1MODEbits.ON = 1; // Activamos la UART
    
    
}

__attribute__((vector(32), interrupt(IPL3SOFT), nomips16))
void InterrupcionUART1(void) {

    if (IFS1bits.U1RXIF == 1) {
        if ((rx.cab + 1 == rx.col) ||
                (rx.cab + 1 == TAM_COLA && rx.col == 0)) {
            // La cola está llena
        } else {
            rx.array[rx.cab] = U1RXREG;
            rx.cab++;
            if (rx.cab == TAM_COLA) {
                rx.cab = 0;
            }
        }
        IFS1bits.U1RXIF = 0;
    }

    if (IFS1bits.U1TXIF == 1) {
        // Se extrae un carácter de la cola y se envía
        if (tx.col != tx.cab) { // Hay datos nuevos
            U1TXREG = tx.array[tx.col];
            tx.col++;
            if (tx.col == TAM_COLA) {
                tx.col = 0;
            }
        } else { // Se ha vaciado la cola.
            IEC1bits.U1TXIE = 0; // Para evitar bucle sin fin
        }
        IFS1bits.U1TXIF = 0;
    }
}

char getcUART() {
    char c;
    if (rx.col != rx.cab) { // Hay datos nuevos
        c = rx.array[rx.col];
        rx.col++;
        if (rx.col == TAM_COLA) {
            rx.col = 0;
        }
    } else { // no ha llegado nada
        c = '\0';
    }
    return c;
}

void putsUART(char *ps) {
    while (*ps != '\0') {
        if ((tx.cab + 1 == tx.col) ||
                (tx.cab + 1 == TAM_COLA && tx.col == 0)) {
            // La cola está llena. 
            break;
        } else {
            tx.array[tx.cab] = *ps; // Copia el carácter en la cola
            ps++; // Apunto al siguiente carácter de la cadena
            tx.cab++;
            if (tx.cab == TAM_COLA) {
                tx.cab = 0;
            }
        }
    }
    // Se habilitan las interrupciones del transmisor para
    // comenzar a enviar
    IEC1bits.U1TXIE = 1;
}