/* 
 * File:   main5.c
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

    // Pin del LED como digital
    ANSELC &= ~(1 << PIN_LED);
    
    // Establecemos todos los pines como salidas
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    // Salidas a 0
    LATA = 0;
    LATB = 0;
    LATC = 0xF; //apagados
    
   
    int i = 0;
    int cambio = 1;
    
    while(1){
        
        LATC ^= 1 << PIN_LED; // ENCENDEMOS
        Retardo(i);         
        LATC ^= 1 << PIN_LED; //APAGAMOS
        Retardo(20-i);      //esperamos 20ms-el ciclo que llevemos
        
        i += cambio;    //sumamos 1 en la ida, -1 en la vuelta
        
        if (i == 20 || i == 0)  //al final de los 20 ciclos para el cambio
            cambio *= -1;       //al finalizar la ida, 1*-1= -1 para la vuelta
                                //al finalizar la vuelta -1*-1=1 para la ida        
    }
}