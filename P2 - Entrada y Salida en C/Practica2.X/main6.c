/* 
 * File:   main6.c
 * Author: Fernando y Jorge
 *
 * Created on 05 February 2021, 12:39
 */

#include <xc.h>

#define PIN_PULSADOR 5

int main ( void )
{
	int pulsador_ant, pulsador_act;
    int contador = 0;
	
	TRISC = 0xFFF0 ; // 4 LEDs as output
	LATC |= 0xE ; // LEDs are OFF except first one
    LATC &= ~1; // We set first LED ON
	
	TRISB = 0xFFFF; // Button RB5 is input 1111111111011111
	
	pulsador_ant = (PORTB >> PIN_PULSADOR) & 1;

	//1111111111X10000  -> 000011111111111X -> 000000000000000X

	while (1) {
        
		// We read buttons state
		pulsador_act = (PORTB >> PIN_PULSADOR) & 1;
        
		if((pulsador_act != pulsador_ant) && (pulsador_act==0)) {
            
            // Falling edge in button, which means it has just been pressed.
            // Each time the button is pressed the one LED ON will change in order
            
            if(contador == 3)
                contador = 0;
            else
                contador++;
            
            LATC = (~(1 << contador)) & (0xF);
            
		}

		pulsador_ant = pulsador_act;
        
	}
    
}
