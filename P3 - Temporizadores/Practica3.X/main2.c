/* 
 * File:   main2.c
 * Author: Jorge & Fernando
 *
 * Created on February 5, 2020
 */

#include <xc.h>
#include "Pic32Ini.h"
#define PIN_LED 0

int main(void) {

    // Pines del LED como digital

    InicializarReloj();

    ANSELC &= ~(1 << PIN_LED);

    LATA = 0; // En el arranque dejamos todas las salidas a 0
    LATB = 0;
    LATC = 0xF; // Excepto el LED, que es activo nivel bajo

    // Como salidas
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;

    T2CON = 0; //Parar el temporizador
    TMR2 = 0; //Cuenta a 0 
    IFS0bits.T2IF = 0; // Se borra el bit de fin de cuenta
    PR2 = 39061; //0.5/(64*200ns)-1=39061.5
    T2CON = 0x8060;

    while (1) {
        while (IFS0bits.T2IF == 0); // Espera el fin del temporizador

        IFS0bits.T2IF = 0; //Cada vez que llegamos a fin temporrizador
        LATC ^= 1 << PIN_LED; // Se invierte el LED

    }
}
