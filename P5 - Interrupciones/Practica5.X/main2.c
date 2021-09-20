/* 
 * File:   main2.c
 * Author: Fernando  & Jorge
 *
 * Created on 05 March 2021, 14:20
 */

#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"

#define PIN_PULSADOR 5
#define PIN_LED0 0
#define PIN_LED3 3


void InicializarTimer2(void) {

     // Se configura el timer para que termine la cuenta en 1 ms
     T2CON = 0;
     TMR2 = 0;
     PR2 = 19530; // (1*5E6/256)-1

     IPC2bits.T2IP = 2; // Prioridad 2
     IPC2bits.T2IS = 0; // Subprioridad 0
     IFS0bits.T2IF = 0;   // Se borra el flag de interrupción por si estaba pendiente    
     IEC0bits.T2IE = 1;  // Por último, se habilita su interrupción

     T2CON = 0x8070; // Se cambia el estado del led 3

}


int main(void) {
    
    int pulsador;

    ANSELC &= ~(1 << PIN_PULSADOR);
    ANSELC &= ~(1 << PIN_LED3 | 1<<PIN_LED0);
    
    LATA = 0; // En el arranque dejamos todas las salidas a 0
    LATB = 0;
    LATC = 0xF; // Apago los LEDs de la placa
    
    TRISA = 0;
    TRISC = 0; //Todo output
    TRISB = 1 << PIN_PULSADOR; //PIN PULSADOR como entrada, resto salidas
    
    InicializarReloj();

    InicializarTimer2();

    // Una vez inicializados los periféricos, activamos las interrupciones
    INTCONbits.MVEC = 1; // Modo multivector
    asm(" ei"); // Interrupciones habilitadas

     while (1) {

         //Se lee estado del pulsador
         pulsador = (PORTB >> PIN_PULSADOR) & 1;

         asm(" di");
         if (pulsador == 0) {
             LATC &= ~(1<<PIN_LED0);
         } else {
             LATC |= 1<<PIN_LED0;
         }
         asm(" ei");


     }
    }

__attribute__((vector(_TIMER_2_VECTOR), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer2(void) {
    // Se borra el flag de interrupción para no volver a
    // entrar en esta rutina hasta que se produzca una nueva
    // interrupciÃ³n.
    IFS0bits.T2IF = 0;
    LATCINV = 1<<PIN_LED3;
}


