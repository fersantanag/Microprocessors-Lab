
#include <xc.h>
#define PIN_TRIG 7 //RA9
#define PIN_ECHO 10 //RB14
#define TIEMPO 10000

int main(void) {
 
    ANSELA &= ~(1 << PIN_ECHO); // Pines digitales
    ANSELA &= ~(1 << PIN_TRIG);
    ANSELA &= ~(0xFFFF); // Pines de los LEDs digitales
    ANSELB &= ~(0xFFFF); // Pines de los LEDs digitales
    ANSELC &= ~(0xFFFF);
     
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    LATA = 0;
    LATB = 0;
    LATC = 0XFFF0; // Apago los LEDs 
    
    while (1) {

    }
}

