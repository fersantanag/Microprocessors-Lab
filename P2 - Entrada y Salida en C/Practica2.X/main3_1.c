/* 
 * File:   main3_1.c
 * Author: Fernando y Jorge
 *
 * Created on 05 February 2021, 11:02
 */

# include <xc.h>

int main(void)
{
	TRISC = 0xFFF0; // 4 LEDs as output
	LATC &= ~0x4; // RC3 1111111011
    LATC |= 0xB;
	
	while (1);
    
	return 0;
}

