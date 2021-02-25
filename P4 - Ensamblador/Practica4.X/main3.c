#include <xc.h>
#define PIN_PULSADOR 5

int main(void)
{
    int pulsador;
    
    TRISC = 0;  // Configuramos todo como output
    LATC = 1;   // El LED es activo a nivel bajo
    TRISB = 1 << PIN_PULSADOR;  // Pin de pulsador como input
    
    while(1){
        // Se lee el estado del pulsador
        pulsador = (PORTB>>PIN_PULSADOR) & 1;
        if(pulsador == 0){
            //LATC &= ~1;
            asm("   lui $v0, 0xBF88"); //0xBF88 = -16504
            asm("   lw $v1, 25136($v0)");
            asm("   addiu $a0, $0, -2");
            asm("   and $v1, $v1, $a0");
            asm("   sw $v1, 25136($v0)");
            
        }else{
            LATC |= 1;
        }
    }
    
}