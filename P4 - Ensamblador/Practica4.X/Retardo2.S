
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
	
	# PRESCALADO
	addi t1, zero, 0    # i = 0
	addi t3, zero, 65535# t3 = 65535
BPre:	slti t0, t1, 8	    # t0 = 1 si i<8
	beqz t0, FBPre
	
	# Comprobamos si es 7
	addi t0, zero, 7
	bne t1, t0, No7
	nop
	addi t1, zero, 8
	
No7:	addi t2, zero, 5000 # t2 = 5000
	mul t2, t2, a0	    # t2 = 5000*retardo_ms
	sll t2, t2, t1	    # t2 = 5000*retardo_ms/2^i
	addi t2, t2, -1	    # t2 = periodo
	slt t1, t3, t2	    # t1 = 1 si retardo > 65535
	beq t1, zero, FBPre # break si periodo <= 65535
	nop
	addi t1, t0, 1	    # i++
	j BPre
	
FBPre:	addi t2, t2, 65535  # t2 = periodo
	addi t0, zero, 8
	bne t1, t0, No8
	nop
	addi t1, zero, 7    # i = 7 si i == 8
	
	# Configuración del temporizador
	# i = t1 / periodo = t2
	
No8:	la t3, T2CON	    # t3 = &T2CON
	sh zero, 0(t3)	    # T2CON = 0
	la t4, TMR4	    # t4 = &TMR2
	sh zero, 0(t4)	    # TMR2 = 0
	la t5, PR2	    # tr5 = &PR2
	sh zero, 0(t5)	    # PR2 = 0
	
	la t6, IFS0	    # t6 = &IFS0
	addi t0, zero, 512  # t0 = 1<<9
	lw t7, 0(t6)	    # t7 = IFS0
	or t0, t0, t7
	sw t0, 0(t6)	    # 
	
	# Configuramos TCKPS y encdendemos
	addi t0, zero, 32768# t0 = 1<<15
	sll t1, t1, 4	    # t1 = tckps << 4
	or t0, t0, t1
	sh t0, 0(t3)	    # T2CON = 1<<15 or tckps<<4
	
	# Esperar
	addi t0, zero, 0
BTemp:	sh t0, 0(t6)	    # t0 = IFS0
	srl t0, t0, 9	    # t0 = IFS0 >> 9
	and t0, t0, 1	    # t0 = (IFS0>>9) & 1
	beq t0, zero, BTemp
	nop
	
	# Apagar temporizador
	sh zero, 0(t3)
	
	j Fin0
	nop
	
Fin1:	addi v0, zero, 1
Fin0:	jr ra
    
    .end Retardo