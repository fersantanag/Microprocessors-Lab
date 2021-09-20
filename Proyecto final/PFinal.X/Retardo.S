
#include <xc.h>
    
    
    .text # sección de código
    
    # Se define el nombre de la función global para poder llamarla desde C
    .global Retardo
    .ent Retardo # Introduzce el símbolo Retardo en el código para depuración
    
Retardo:
	
	addi v0, zero, 0
	# Se recibe el periodo en ms en a0
	beq a0, zero, Fin1  # return 1 si periodo == 0
	slti t0, a0, 3356   # to = 1 si periodo < 3356
	beqz t0, Fin1  # return 1 si periodo >= 3356
	
	# T2CON = 0
	la t1, T2CON
	sw zero, 0(t1)
	# IFS0bits.T2IF = 0
	la t4, IFS0
	addi t9, zero, 0x200# t9 = 1 << 9
	nor t9, t9, zero	# t9 = not (1<<9(
	lw t0, 0(t4)
	and t0, t0, t9		# IFS0 &= ~(1<<9)
	sw t0, 0(t4)
	# TMR2 = 0
	la t2, TMR2
	sw zero , 0(t2)
	
	# PR2 = 4999
	addi t0, zero, 4999
	la t3, PR2
	sw t0, 0(t3)
	
	# Encendemos
	addi t0, zero, 0x8000
	sw t0, 0(t1)		# T2CON = 1<<15
	
	
Buc:	beqz a0, FBuc
	# IFS0bits.T2IF = 0
	addi t9, zero, 0x200	# t9 = 1 << 9
	nor t9, t9, zero	# t9 = not (1<<9(
	lw t0, 0(t4)
	and t0, t0, t9		# IFS0 &= ~(1<<9)
	sw t0, 0(t4)
	# TMR2 = 0
	sw zero, 0(t2)
	
	# Esperamos 1ms
Esp:	lw t0, 0(t4)		# t0 = IFS0
	andi t0, t0, 0x200	# t0 = IFS0 & (1<<9)
	beqz t0, Esp
	nop
	addi a0, a0, -1
	j Buc
	nop
	
	# T2CON = 0
FBuc:	sw zero, 0(t1)
    
	j Fin0
	nop
	
Fin1:	addi v0, zero, 1
Fin0:	jr ra
    
    .end Retardo