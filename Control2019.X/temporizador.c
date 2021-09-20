/* 
 * File:   temporizador.c
 * Author: jorge
 *
 * Created on 15 April 2021, 17:09
 */

#include <xc.h>

static int ticks = 0;

int getMillis() {
    return ticks;
}

void InicializarTemporizador() {
    
    T2CON = 0;
    
    TMR2 = 0;
    IFS0bits.T2IF = 0;
    PR2 = 4999;        // Fórmula periodo: 5MHz*t_espera / (prescalado)-1
    
    // Interrupciones
    IEC0bits.T2IE = 1;
    IPC2bits.T2IP = 2;
    IPC2bits.T2IS = 0;
    
    // Activamos temp
    T2CON = 0x8000;
    
}

__attribute__ ((vector(_TIMER_2_VECTOR), interrupt(IPL2SOFT), nomips16))
void InterrupcionTemporizador2() {
    IFS0bits.T2IF = 0;
    ticks++;
}



