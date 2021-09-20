/* 
 * File:   main5.c
 * Author: Fernando & Jorge
 *
 * Created on 26 March 2021, 12:48
 */

#include <xc.h>
#include "Pic32Ini.h"
#include "driver_uart.h"


#define PIN_RX 13
#define PIN_TX 7
#define PIN_PULSADOR 5


int main(void) {

    // Configuramos como digitales
    ANSELB &= ~(1<<PIN_RX | 1<<PIN_TX | 1<<PIN_PULSADOR);
    
    // El pulsador y el rx son entradas
    TRISA = 0;
    TRISB = 1<<PIN_PULSADOR | 1<<PIN_RX;
    TRISC = 0;
    
    // LEDs apagados
    LATA = 0;
    LATB = 1<<PIN_TX;   // Transmisor inhabilitado
    LATC = 0xF;
            
    // Conectamos U1RX y U1TX
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x0;
    
    // Inicializar UART
    InicializarUART();
    
    
    InicializarReloj();
    
    // Iniciamos interrupciones
    INTCONbits.MVEC = 1;
    
    char c;
    char cadena[] = "Hola Mundo!";
    int puls_act, puls_ant = (PORTB >> PIN_PULSADOR) & 1;
    
    while(1) {
        puls_act = (PORTB >> PIN_PULSADOR) & 1;
        
        if (puls_act < puls_ant) {  // Flanco de bajada
            putsUART(cadena);            
        }
        
        c = getcUART();
        if(c != '\0') {
            LATC |= 0xF;         // Apagamos los LEDs
            LATC &= ~(c & 0xF);  // Encendemos los correspondientes
        }
        
        puls_ant = puls_act;
    }
    
    return 0;
}




