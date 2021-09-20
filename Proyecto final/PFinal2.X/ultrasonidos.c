#include <xc.h>
#include "main.h"
#include "ultrasonidos.h"


#define TIEMPO 10000


static int32_t ticks = 0;
static int mide = 0;
static int midiendo = 0;
static int16_t distancia = 0;


void InicializarTemp() {
    
    /*T1CON = 0;
    PR1 = 4; // 1 us a 5 MHz (div = 1)
    TMR1 = 0;
    
    IFS0bits.T1IF = 0; // Se borra el flag del timer 1
    IEC0bits.T1IE = 1; // Se habilita su interrupcion
    IPC1bits.T1IP = 3;
    IPC1bits.T1IS = 1;
    
    T1CON = 0x8000; // Timer 1 ON*/
    
    T1CON = 0;
    PR1 = 49;
    TMR1 = 0;
    
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC1bits.T1IP = 4;
    IPC1bits.T1IS = 1;
    
    T1CON = 0x8000;
   
    
}

int InicializarUltra(void) {
 
    
    // Se inicializa el temporizador 5 para medir echo
    T5CON = 0;
    TMR5 = 0;
    IFS0bits.T5IF=0; // Se borra el flag del timer 5
    IEC0bits.T5IE=1;
    IPC5bits.T5IP=5;
    IPC5bits.T5IS=0;
    T5CON = 0x8090; // Timer ON, TGATE = 1, TCKPS = 1 (prescalado 2)
            
    //remapeamos timer 5
    SYSKEY = 0xAA996655; 
    SYSKEY = 0x556699AA; 
    //T5CKR = 1; // T5CK conectado al echo = RPB14
    T5CKR = 7; // T5CK conectado al echo = RC4
    SYSKEY = 0x1CA11CA1;

    
    
}


void iniciarMedida() {
    if (!midiendo) {
        mide = 1;
        midiendo = 1;
    }
    
}

static int last_ticks = 0;

__attribute__((vector(_TIMER_1_VECTOR), interrupt(IPL4SOFT), nomips16))
void InterrupcionTimer1(void) {
    IFS0bits.T1IF = 0;
    ticks++;
    
    LATCCLR = 1<<PIN_TRIG;
    if (mide) {
        LATCSET = 1<<PIN_TRIG;
        mide = 0;
    }
    
    if (police_mode && ticks-last_ticks>25000) {
        last_ticks = ticks;
        LATBINV = 1<<LEDS_AZUL | 1<<LEDS_ROJO;
    } 
    
    if (!police_mode) {
        last_ticks = 0;
        LATBCLR = 1<<LEDS_AZUL | 1<<LEDS_ROJO;
    }
        
}

__attribute__((vector(_TIMER_5_VECTOR), interrupt(IPL5SOFT), nomips16))
void InterrupcionTimer5(void) {
    IFS0bits.T5IF = 0;
    distancia=TMR5/147;
    TMR5=0;
    midiendo = 0;
}

int getDistancia(void) {
    /*if (distancia >= 0 && distancia <= 15) {
        LATCSET = 0xF;
        LATCCLR = distancia;
    }*/
    return distancia;
}

int32_t micros(void) {
    return ticks;
}


