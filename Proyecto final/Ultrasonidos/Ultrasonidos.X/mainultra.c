#include <xc.h>
#define PIN_TRIG 9 //RA9
#define PIN_ECHO 14 //RB14
#define TIEMPO 10000

int main(void) {
 
    ANSELB &= ~(1 << PIN_ECHO); // Pines digitales
    ANSELA &= ~(1 << PIN_TRIG);
    ANSELC &= ~(0xF); // Pines de los LEDs digitales
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF; // Apago los LEDs 
    
    TRISA = 0;
    TRISB = 1 << PIN_ECHO;
    TRISC = 0;
    
   
    
    // Se inicializa el temporizador 2 para enviar trigger 10micros
    /*T2CON = 0;
    PR2 = 49; // 1us 5 MHz (div = 1)
    TMR2 = 0;
    IFS0bits.T2IF=0; // Se borra el flag del timer 2
    IPC2 &= ~(0x1F); // Se ponen a 0 la prioridad y subprioridad de T2
    IEC0bits.T2IE=1; // Se habilita su interrupción
    IPC2bits.T2IP=2; // Prioridad Timer 2 = 2
    IPC2bits.T2IS=1; // Subprioridad Timer 2 = 1    
    T2CON = 0x8000;  // Timer 2 ON*/
    
    T2CON = 0;
    PR2 = 49; // 10 us a 5 MHz (div = 1)
    TMR2 = 0;
    IFS0 &= ~(1<<9); // Se borra el flag del timer 2
    IEC0 |= (1<<9); // Se habilita su interrupci´on
    IPC2 &= ~(0x1F); // Se ponen a 0 la prioridad y subprioridad de T2
    IPC2 |= 2<<2; // Prioridad Timer 2 = 2
    IPC2 |= 1; // Subprioridad Timer 2 = 1
    T2CON = 0x8000; // Timer 2 ON
    
    // Se inicializa el temporizador 5 para medir echo
    T5CON = 0;
    TMR5 = 0;
    IFS0bits.T5IF=0; // Se borra el flag del timer 5
    IEC0bits.T5IE=1;
    IPC5bits.T5IP=3;
    IPC5bits.T5IS=1;
    T5CON = 0x8090; // Timer ON, TGATE = 1, TCKPS = 1 (prescalado 2)
            
    //remapeamos timer 5
    SYSKEY = 0xAA996655; // Se desbloquean los registros
    SYSKEY = 0x556699AA; // de configuración.
    T5CKR = 1; // T5CK conectado al echo = RPB14
    SYSKEY = 0x1CA11CA1; // Se vuelven a bloquear.
    
    
    INTCONbits.MVEC = 1; // Se pone en modo multivector
    asm(" ei"); // y se habilitan las interrupciones
    
    int16_t dist;
    while (1) {
        dist=getDistancia();
        //LATCSET = 0xF;
        //LATCCLR = dist & 0xF;
        if(dist<10)
           LATCCLR = 0xF;
        else LATCSET = 0xF;
    }
}

static uint32_t ticks=0;
static uint16_t distancia;
   
__attribute__((vector(8), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer2(void) {
    IFS0bits.T2IF = 0;
    
    ticks++;
    LATACLR = 1 << PIN_TRIG; // Se pone a 0 el pin trig.
    
    if(ticks>TIEMPO)
    {
        LATASET = 1 << PIN_TRIG;// Se pone a 1 el pin trig.
        ticks=0;
    }
}

__attribute__((vector(20), interrupt(IPL3SOFT), nomips16))
void InterrupcionTimer5(void) {
    IFS0bits.T5IF = 0;
    distancia=TMR5/147;
    TMR5=0;
    }

int getDistancia() {
    uint32_t _distancia;
    asm("di");
    _distancia=distancia;
    asm("ei");
    return _distancia;
}