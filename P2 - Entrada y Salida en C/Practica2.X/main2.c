/* 
 * File:   main2.c
 * Author: Fernando y Jorge
 *
 * Created on 05 February 2021, 11:00
 */

#include <xc.h>

int main(void) {
    
    TRISC = 0xFFF0; // 4 LEDs as output 11111111110000
    LATC = ~0xF; // LEDs on

    while (1);

    return 0;
}
