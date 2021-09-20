/* 
 * File:   main4.c
 * Author: Fernando & Jorge
 *
 * Created on 26 March 2021, 10:32
 */

#include <xc.h>
#include "Pic32Ini.h"


#define PIN_TX 7
#define PIN_PULSADOR 5


int main(void) {
    
    // Establecemos los pines como digitales
    ANSELB &= ~(1<<PIN_TX | 1<<PIN_PULSADOR);
    
    // Todo salidas, excepto el pulsador
    TRISA = 0;
    TRISB = 1<<PIN_PULSADOR;
    TRISC = 0;
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF; // Leds apagados
    
    // Conectamos U1TX con RB7
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB7R = 1;
    SYSKEY = 0x1CA11CA1;
    
    U1BRG = 32;
    
    U1MODE = 0x8000;    // Activamos la UART, formato 8N1
    
    InicializarReloj();

    int puls_act, puls_ant = (PORTB >> PIN_PULSADOR) & 1;
    
    char cadena[] = "Hola Mundo\r\n";
    int i;
    char c;
    
    while(1) {
        
        puls_act = (PORTB >> PIN_PULSADOR) & 1;
        
        if (puls_act < puls_ant) {  // Flanco de subida
            
            U1STAbits.UTXEN = 1;    // Habilitamos el transmisor
            
            i = 0;
            c = cadena[i];
            while(c != '\0') {
                
                U1TXREG = c;
                while (U1STAbits.TRMT == 0) ;
                
                i++;
                c = cadena[i];
            }
            
            U1STAbits.UTXEN = 0;    // Deshabilitamos el transmisor
            
        }
        
        puls_ant = puls_act;
        
    }

    return 0;
}

