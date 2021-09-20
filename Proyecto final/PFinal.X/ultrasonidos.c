#include <xc.h>

#define PIN_TRIG 9  //RA9
#define PIN_ECHO 14 //RB14
#define TIEMPO 10000


static int32_t ticks = 0;


void InicializarTemp() {
    
    /*T1CON = 0;
    PR1 = 4; // 1 us a 5 MHz (div = 1)
    TMR1 = 0;
    
    IFS0bits.T1IF = 0; // Se borra el flag del timer 1
    IEC0bits.T1IE = 1; // Se habilita su interrupcion
    IPC1bits.T1IP = 3;
    IPC1bits.T1IS = 1;
    
    T1CON = 0x8000; // Timer 1 ON*/
    
    T3CON = 0;
    PR3 = 49;
    TMR3 = 0;
    
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;
    IPC3bits.T3IP = 2;
    IPC3bits.T3IS = 3;
    
    T3CON = 0x8000;
    
    
}

int InicializarUltra(void) {
 
    ANSELB &= ~(1 << PIN_ECHO); // Pines digitales
    ANSELA &= ~(1 << PIN_TRIG);
    ANSELC &= ~(0xF); // Pines de los LEDs digitales
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF; // Apago los LEDs 
    
    TRISA = 0;
    TRISB = 1 << PIN_ECHO;
    TRISC = 0;
       
    
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
    
    

    
    
}

static uint16_t distancia;


#define US_STOP     0
#define US_TRIGGER  1
#define US_MIDIENDO 2
static int us_estado = US_STOP;
static int mide = 0;
static int start_mide = 0;
   

__attribute__((vector(_TIMER_3_VECTOR), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer1(void) {
    //IFS0bits.T1IF = 0;
    IFS0bits.T3IF = 0;
    
    ticks++;
    
    /*if (mide) {
        if (us_estado == US_STOP) {
            LATASET = 1 << PIN_TRIG; // Se pone a 0 el pin trig.
            start_mide = ticks;
            us_estado = US_TRIGGER;
        }
        mide = 0;
    }
    
    
    if(us_estado == US_TRIGGER && ticks>start_mide) {
        LATACLR = 1 << PIN_TRIG;// Se pone a 1 el pin trig.
        us_estado = US_MIDIENDO;
    }*/
    
}

__attribute__((vector(20), interrupt(IPL3SOFT), nomips16))
void InterrupcionTimer5(void) {
    IFS0bits.T5IF = 0;
    distancia=TMR5/147;
    TMR5=0;
}

int getDistancia(void) {
    uint32_t _distancia;
    asm("di");
    _distancia=distancia;
    asm("ei");
    return _distancia;
}

int32_t micros(void) {
    return ticks;
}


