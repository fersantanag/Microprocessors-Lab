#include <xc.h>

int main ( void )
{
    
    TRISC = 0xFFF0;
    PORTC &= ~0x0F;
    while (1)
        ;
    return 0; 
    
}
