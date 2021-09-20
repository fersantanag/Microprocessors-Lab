

#include <xc.h>
    
    
    .text # secci?n de c?digo
    
    # Se define el nombre de la funci?n global para poder llamarla desde C
    .global apagarLed
    .ent apagarLed # Introduzce el s?mbolo Retardo en el c?digo para depuraci?n
    
apagarLed:
    
    # Comprobamos n_led >= 0
    slti t0, a0, 0
    bne t0, zero, Fin
    
    # Comprobamos n_led <= 3
    li t0, 3
    slt t0, t0, a0
    bne t0, zero, Fin
    
    
    la t1, LATC
    lw t2, 0(t1)
    
    addi t0, zero, 1
    sllv t0, t0, a0
        
    or t2, t2, t0
    sw t2, 0(t1)  
    
Fin:
    
    jr ra
    
    .end apagarLed
    