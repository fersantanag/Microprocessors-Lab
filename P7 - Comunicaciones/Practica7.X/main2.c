/* 
 * File:   main2.c
 * Author: Fernando & Jorge
 *
 * Created on 8 April 2021, 12:48
 */

#include <xc.h>
#include "Pic32Ini.h"
#include "driver_uart.h"


#define PIN_RX 13
#define PIN_TX 7


int main(void) {

    // Configuramos como digitales
    ANSELB &= ~(1<<PIN_RX | 1<<PIN_TX);
    
    // El pulsador y el rx son entradas
    TRISA = 0;
    TRISB = 1<<PIN_RX;
    TRISC = 0;
    
    // LEDs apagados
    LATA = 0;
    LATB = 1<<PIN_TX;   // Transmisor inhabilitado
    LATC = 0;
            
    // Conectamos U1RX y U1TX
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x0;
    
    // Inicializar UART
    InicializarUART1(9600);
    
    
    InicializarReloj();
    
    // Iniciamos interrupciones
    INTCONbits.MVEC = 1;
    asm(" ei");
    
    char c;
    char s[2];
    s[1] = '\0';
    
    while(1) {
        
        c = getcUART();
        if (c != '\0') {
           s[0] = c;
           putsUART(s); 
        }
        
           
    }
    
    return 0;
}




