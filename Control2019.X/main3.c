/* 
 * File:   main3.c
 * Author: jorge
 *
 * Created on 15 April 2021, 16:37
 */

#include <xc.h>
#include "EncenderLed.h"
#include "ApagarLed.h"
#include "Pic32Ini.h"
#include "temporizador.h"

#define PIN_PULSADOR 5


int main(void) {
    
    // Ponemos como digitales
    ANSELC &= ~(0xF);
    
    // IO
    TRISA = 0;
    TRISB = 1 << PIN_PULSADOR;
    TRISC = 0;
    
    // Estado
    LATA = 0;
    LATB = 0;
    LATC = 0xF;

    // Inicializaciones
    InicializarReloj();
    InicializarTemporizador();
    
    // Interrupciones
    INTCONbits.MVEC |= 1;
    asm(" ei");
    
    int n = 0;
    int puls_act, puls_ant = (PORTB >> PIN_PULSADOR) & 1;
    int tmp_ticks, last_ticks = 0;
    while (1) {
        puls_act = (PORTB >> PIN_PULSADOR) & 1;
        if (puls_act < puls_ant) {
            
            n++;
            if (n == 4) 
                n = 0;
            
            // Encendemos
            encenderLed(n);
            // Apagamos
            if (n == 0)
                apagarLed(3);
            else
                apagarLed(n-1);
            
            
        }
       
        
        puls_ant = puls_act;
        
        tmp_ticks = getMillis();
        if (tmp_ticks-last_ticks > 500) {
            LATCINV = 1<<n;
            last_ticks = tmp_ticks;
        }
        
    }
    
    
    
    

    return 0;
}

