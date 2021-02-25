/* 
 * File:   main5.c
 * Author: Jorge & Fernando
 *
 * Created on February 5, 2020
 */

#include <xc.h>
#include "retardo.h"

int Retardo (uint16_t retardo_ms){
    //CALCULO SI SE PUEDO GENERAR RETARDO CON 16 BITS
    int periodo;
    int i;
    
    //si el retardo es 0 o superior al que podemos generar con el máximo preesc
    //return 1 y acaba la función
    if(retardo_ms==0)
        return 1;
    if(retardo_ms>=3355.5)
        return 1;
    
    //bucle para probar con los distintos preescalados
    for(i=0; i<8 ; i++){
        if (i==7)
            i=8;
        //el preescalado 7 es en realidad 2^8=256
        
        periodo = (5000*retardo_ms/(1 << i)) -1; //(ret/200ns*2^TCKPS)-1
        if(periodo<=65535){
            break; //Puedo realizar retardo, salgo del bucle
        }
    }
    if(i==8)
        i = 7;
    //recordamos que aunque i=8 para poder conseguir preesc 256
    //en realidad es el preescalado 7
    
    
    // GENERO UN RETARDO
    
    T2CON=0; //Parar el temporizador
    TMR2=0; //Cuenta a 0 
    IFS0bits.T2IF = 0; // Se borra el bit de fin de cuenta
    PR2=periodo;     

    T2CONbits.TCKPS=i; //Ponemos preescaler  
    T2CONbits.ON= 1; //ENCENDEMOS

    while(IFS0bits.T2IF==0); //Espera fin de la cuenta
    
    T2CONbits.ON=0; //APAGAMOS
    return 0;    
    
}


