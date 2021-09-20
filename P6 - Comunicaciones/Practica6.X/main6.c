/* 
 * File:   main6.c
 * Author: Fernando & Jorge
 *
 * Created on 26 March 2021, 18:48
 */

#include <xc.h>
#include "Pic32Ini.h"

#define PIN_RX 13
#define PIN_TX 7


int main(void) {
    
    // Establecemos como digitales los pines correspondientes
    ANSELB &= ~(1<<PIN_RX | 1<<PIN_TX);
    
    TRISA = 0;
    TRISB = 1<<PIN_RX;
    TRISC = 0;
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF; // Leds apagados
    
    // Conectamos U1RX con RB13
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x0;
    
    U1BRG = 32;             // 9600 baudios
    
    U1STAbits.URXEN = 1;    // Habilitamos el receptor
    U1MODE = 0x8000;        // Activamos la UART, formato 8N1
    
    InicializarReloj();
    
    
    char c;
    
    while(1) {
        
        // RECEPCIÓN
        
        if(U1STAbits.URXDA == 1) { // Vemos si hay recepción
        
            c = U1RXREG;    // Leemos el caracter
            
            // TRANSMISION
            U1STAbits.UTXEN = 1;    // Habilitamos el transmiso
            U1TXREG = c;
            while (U1STAbits.TRMT == 0) ;
            U1STAbits.UTXEN = 0;    // Deshabilitamos el transmisor
        
        }
        
    }  

    return 0;
}

