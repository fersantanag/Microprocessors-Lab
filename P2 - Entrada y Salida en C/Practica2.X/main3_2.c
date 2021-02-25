/* 
 * File:   main3_2.c
 * Author: Fernando y Jorge
 *
 * Created on 05 February 2021, 11:03
 */

#include <xc.h>

int main(void)
{
	TRISC = 0xFFF0; // 4 LEDs as output
	LATC &= ~0xC; // RC3,RC2  111110011
    LATC |= 0x3;
    
	while (1);
    
	return 0;
}
