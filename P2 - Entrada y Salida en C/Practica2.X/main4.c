/* 
 * File:   main4.c
 * Author: Fernando y Jorge
 *
 * Created on 05 February 2021, 11:04
 */

#include <xc.h>

#define PIN_PULSADOR 5

int main(void)
{
	int pulsador;
    
	TRISC = 0xFFF0; // 4 LEDs as output
	LATC |= 0xF; // LEDs as OFF

	TRISB = 0xFFFF; // Button is input
	
	while (1) {
        
		// We read button state
		pulsador = (PORTB >> PIN_PULSADOR) & 1;
        
        // We turn LEDs on when button is pressed and off when not
		if(pulsador == 0){
			LATC &= ~0xF;
		}else{
			LATC |= 0xF;
		}
        
	}
}