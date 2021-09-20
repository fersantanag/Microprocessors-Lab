#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"

#define PIN_PULSADOR 5
#define PIN_LED0 0
#define PIN_LED2 2
#define PIN_LED3 3


void InicializarTimer2(void) {
    
    // Se configura el timer para que termine la cuenta en 1 s
    T2CON = 0;
    TMR2 = 0;
    PR2 = 19530; // (1*5E6/256)-1
    
    IPC2bits.T2IP = 2;  // Prioridad 2
    IPC2bits.T2IS = 0;  // Subprioridad 0
    IFS0bits.T2IF = 0;  // Se borra el flag de interrupción por si estaba pendiente
    IEC0bits.T2IE = 1;  // y por último se habilita su interrupción
    
    T2CON = 0x8070; // Se arranca el timer con prescalado 7=256
}

void InicializarTimer3(void) {
    
    // Se configura el timer para que termine la cuenta en 0.5s
    T3CON = 0;
    TMR3 = 0;
    PR3 = 39061;        // (1*5E6/64)-1=39061.5
    
    IPC3bits.T3IP = 4;  // Prioridad 4
    IPC3bits.T3IS = 0;  // Subprioridad 0
    IFS0bits.T3IF = 0;  // Se borra el flag de interrupción por si estaba pendiente
    IEC0bits.T3IE = 1;  // y por último se habilita su interrupción
    
    T3CON = 0x8060;     // Se arranca el timer con prescalado 7=256
}

int main(void) {
    
    // Puertos digitales

    ANSELC &= ~(1 << PIN_PULSADOR);
    ANSELC &= ~(1 << PIN_LED0 | 1<<PIN_LED2 | 1<<PIN_LED3);
    
    LATA = 0; // En el arranque dejamos todas las salidas a 0
    LATB = 0;
    LATC = 0xF; // Apago los LEDs de la placa
    
    // IO.
    TRISA = 0;
    TRISC = 0; 
    TRISB = 1<<PIN_PULSADOR; //PIN PULSADOR como entrada, resto salidas

    // Inicializamos los timer
    InicializarTimer2();
    InicializarTimer3();

    // Activamos las iterrupciones
    INTCONbits.MVEC = 1; // Modo multivector
    asm(" ei"); // Interrupciones habilitadas
   
    
    
    int pulsador = (PORTB >> PIN_PULSADOR) & 1;
    
    while (1) {
        
        //Se lee estado del pulsador
        asm("   di");
        pulsador = (PORTB >> PIN_PULSADOR) & 1;
        if (pulsador == 0)
            LATC &= ~(1<<PIN_LED0);
        else
            LATC |= 1<<PIN_LED0;
        asm("   ei");
        
 
    }
}

__attribute__((vector(_TIMER_3_VECTOR), interrupt(IPL4SOFT), nomips16))
void InterrupcionTimer3(void) {
    // Se borra el flag de interrupción para no volver a
    // entrar en esta rutina hasta que se produzca una nueva
    // interrupción.
    IFS0bits.T3IF = 0;
    LATCINV = 1<<PIN_LED2;
}

__attribute__((vector(_TIMER_2_VECTOR), interrupt(IPL2SOFT), nomips16))    
void InterrupcionTimer2(void) {
    // Se borra el flag de interrupción para no volver a
    // entrar en esta rutina hasta que se produzca una nueva
    // interrupción.
    IFS0bits.T2IF = 0;
    LATCINV = 1<<PIN_LED3;

}


