/* 
 * File:   main5.c
 * Author: Fernando y Jorge
 *
 * Created on 05 February 2021, 11:05
 */

#include <xc.h>

#define PIN_PULSADOR 5

int main ( void )
{
	int pulsador_ant, pulsador_act;
	int contador=0;
	
	TRISC = 0xFFF0; // 4 LEDs as output
	LATC |= 0xF; // LEDs as OFF
	
	TRISB = 0xFFFF; // Button RB5 is input 1111111111011111
	
	pulsador_ant = (PORTB >> PIN_PULSADOR) & 1;
    
	
	//1111111111X10000  -> 000011111111111X -> 000000000000000X
    
    
	while (1) {
        
		// We read buttons state
		pulsador_act = (PORTB >> PIN_PULSADOR) & 1;
        
		if((pulsador_act != pulsador_ant) && (pulsador_act==0)) {
            
            // Falling edge in button, which means it has just been pressed.

            // We will show the number of times button has been pressed with the LEDs
            // When all LEDS are on, we will turn all off.
            if((LATC & 0xF) == 0x0){
                LATC |= 0xF;
            }else{
                contador++;
                LATC =~ contador;
            }
            
		}

		pulsador_ant = pulsador_act;
        
	}
    
}