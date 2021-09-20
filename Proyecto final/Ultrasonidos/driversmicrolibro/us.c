#include "xc.h"
#include "us.h"

#define PIN_LED0 0
#define PIN_LED1 1
#define PIN_LED3 3
#define PIN_PULSADOR 5
#define PULSACIONES 5

static int pulsaciones = 0;
static int ticks = 0;

int getPulsaciones() { return pulsaciones; }
int getTicks() { return ticks; }
void setPulsaciones(int _pulsaciones) { pulsaciones = _pulsaciones; }
void setTicks(int _ticks) { ticks = _ticks; }


void InicializarTimer2(void) {
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 19530;        //1*5E6/256
    
    IPC2bits.T2IP = 2;  //Priority = 2
    IPC2bits.T2IS = 0;  //Subpriority = 2
    IFS0bits.T2IF = 0;  //Flag a cero
    IEC0bits.T2IE = 1;  //Enable interrupt
    
    T2CON = 0x8070;     //Inicializamos con prescaler = 256    
    
}

__attribute__((vector(_TIMER_2_VECTOR), interrupt(IPL2SOFT),  nomips16))
void InterrupcionTimer2(void) {
    IFS0bits.T2IF = 0;  //Eliminamos el flag
    pulsaciones = 0;
    LATCINV = 1<<PIN_LED3;
    if (ticks!=0)
        ticks--;
    else
        LATCSET = 1<<PIN_LED1;
}
