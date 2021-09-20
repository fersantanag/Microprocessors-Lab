/* 
 * File:   main3.c
 * Author: Fernando & Jorge
 *
 * Created on 26 March 2021, 10:23
 */

#include <xc.h>
#include "Pic32Ini.h"

#define PIN_RX 13

int main(void) {

    // Establecemos RB13 como digital y entrada
    ANSELB &= ~(1<<PIN_RX);
    
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
    SYSKEY = 0x0;
    
    U1BRG = 32;             // 9600 baudios
    
    U1STAbits.URXEN = 1;    // Habilitamos el receptor
    U1MODE = 0x8000;        // Activamos la UART, formato 8N1
    
    InicializarReloj();
    
    char c;
    
    while(1) {
        
        while(U1STAbits.URXDA == 0) ;   // Esperamos la recepción
        
        c = U1RXREG;    // Leemos el caracter
        
      
		LATCSET = 0xF;         // Apagamos los LEDs
        LATCCLR = (c & 0xF);  // Encendemos los correspondientes
    }
    
    return 0;
}

