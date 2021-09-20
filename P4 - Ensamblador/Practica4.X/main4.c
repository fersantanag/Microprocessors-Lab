/* 
 * File:   main4.c
 * Author: Jorge & Fernando
 *
 * Created on February 5, 2020
 */

#include <xc.h>
#include "Pic32Ini.h"
#include "retardo.h"
#define PIN_LED 0

int main(void) {
    
    InicializarReloj();
    
    // Pines del LED como digital
    ANSELC &= ~(1 << PIN_LED);
    LATA = 0; // En el arranque dejamos todas las salidas a 0
    LATB = 0;
    LATC = 0xF; //apagados al empezar
    
     // Como salidas
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    while(1){
        Retardo(1000); //0.5s  para que la frecuencia de parpadeo = 1Hz
        LATC ^= 1 << PIN_LED; // Se invierte el LED
    }
}