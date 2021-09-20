/* 
 * File:   main4.c
 * Author: jorge
 *
 * Created on 05 March 2021, 14:34
 */

#include <xc.h>
#include "Pic32Ini.h"
#include "temporizador.h"

#define PIN_LED0 0
#define PIN_LED1 1
#define PIN_LED3 3
#define PIN_PULSADOR 5
#define PULSACIONES 5



int main(void) {
    
    // Definimos los pins como digitales
    ANSELC &= ~(1<<PIN_LED0 | 1<<PIN_LED1 | 1<<PIN_LED3);
    ANSELB &= ~(1<<PIN_PULSADOR);

    // Definimos las salidas a 0 excepto el LED
    LATA = 0;
    LATB = 0;
    LATC = 0xF;
    
    // Definimos los puertos como salida excepto el pulsador
    TRISA = 0;
    TRISB = 1<<PIN_PULSADOR;
    TRISC = 0;
    
    InicializarReloj();
    
    // Inicializamos el reloj
    InicializarTimer2();
    
    // Iniciamos las interrupciones
    INTCONbits.MVEC = 1;
    asm("   ei");
    
    // Bucle
    int puls_act, puls_ant;
    puls_ant = (PORTB>>PIN_PULSADOR) & 1;
    while(1) {
        puls_act = (PORTB>>PIN_PULSADOR) & 1;
        
        //  Configuramos RC0 según el pulsador
        asm("   di");
        if(puls_act == 0)
            LATC &= ~(1<<PIN_LED0);
        else
            LATC |= 1<<PIN_LED0;
        asm("   ei");
        
        // Si hay flanco incrementamos la variable global
        if(puls_act>puls_ant) {
            setPulsaciones(getPulsaciones()+1);
        }
        
        // Si se llega al límite
        
        if(getPulsaciones() > PULSACIONES) {
            LATC &= ~(1<<PIN_LED1);
            setTicks(4);
        }
            
        
        puls_ant = puls_act;
    }
    
    return 0;
}




        
      

