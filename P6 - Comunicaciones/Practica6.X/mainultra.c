
#include <xc.h>
#include "driver_uart.h"

#define PIN_TRIG 9 //RA9
#define PIN_ECHO 14 //RB14
#define TIEMPO 1000

#define PIN_RX 13
#define PIN_TX 7
#define PIN_PULSADOR 5

int main(void) {
    
    
    ANSELB &= ~(1<<PIN_RX | 1<<PIN_TX | 1<<PIN_PULSADOR);
    TRISB = 1<<PIN_PULSADOR | 1<<PIN_RX;TRISB = 1<<PIN_PULSADOR | 1<<PIN_RX;
    LATB = 1<<PIN_TX;   // Transmisor inhabilitado
    
        // Conectamos U1RX y U1TX
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x0;
    
    // Inicializar UART
    InicializarUART();
    
    
    InicializarReloj();
    
    
    
    
    ANSELB &= ~(1 << PIN_PULSADOR |  1 << PIN_ECHO); // Pines digitales
    ANSELA &= ~(1 << PIN_TRIG);
    ANSELC &= ~(0xF); // Pines de los LEDs digitales
    
    LATB = 0;
    LATC = 0xF; // Apago los LEDs 
    
    TRISA = 0;
    TRISB =  1 << PIN_ECHO;
    TRISC = 0;
    
    // Se inicializa el temporizador 2 para enviar trigger 10micros
    T2CON = 0;
    PR2 = 49; // 1us 5 MHz (div = 1)
    TMR2 = 0;
    IFS0bits.T2IF=0; // Se borra el flag del timer 2
    IPC2 &= ~(0x1F);// Se ponen a 0 la prioridad y subprioridad de T2
    IPC2bits.T2IP=3; // Prioridad Timer 2 = 2
    IPC2bits.T2IS=1; // Subprioridad Timer 2 = 1
    IEC0bits.T2IE=1;// Se habilita su interrupción
    T2CON = 0x8000; // Timer 2 ON
    
    // Se inicializa el temporizador 5 para medir echo
    T5CON = 0;
    TMR5 = 0;
    IFS0bits.T5IF = 0; // Se borra el flag del timer 5
    IPC5bits.T5IP=3;
    IPC5bits.T5IS=1;
    IEC0bits.T5IE=1;
            
    //remapeamos timer 5
    SYSKEY = 0xAA996655; // Se desbloquean los registros
    SYSKEY = 0x556699AA; // de configuración.
    T5CKR = 1; // T5CK conectado al echo = RPB14
    SYSKEY = 0x1CA11CA1; // Se vuelven a bloquear.
    T5CON = 0x8090; // Timer ON, TGATE = 1, TCKPS = 1 (prescalado 2)
    
    INTCONbits .MVEC = 1; // Se pone en modo multivector
    asm(" ei"); // y se habilitan las interrupciones
    
    uint16_t dist;
    char c;
    int puls_act, puls_ant = (PORTB >> PIN_PULSADOR) & 1;
    while (1) {
        LATCSET = 0xF;
        dist=getDistancia();
        LATCCLR = dist & 0xF;
        
        puls_act = (PORTB >> PIN_PULSADOR) & 1;
        
        if (puls_act < puls_ant) {  // Flanco de bajada
            putsUART(dist);            
        }
        
        c = getcUART();
        if(c != '\0') {
            LATC |= 0xF;         // Apagamos los LEDs
            LATC &= ~(c & 0xF);  // Encendemos los correspondientes
        }
        
        puls_ant = puls_act;
    }
        
        
        
        
        
        
    }
}
   static uint32_t ticks_ant=0;
   static uint32_t ticks;
   static uint16_t distancia;
   
__attribute__((vector(8), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer2(void) {
    IFS0bits.T2IF = 0;
    
    ticks ++;
    LATACLR = 1 << PIN_TRIG; // Se pone a 1 el pin trig.
    
    if(ticks-ticks_ant<TIEMPO)
    {
    LATASET = 1 << PIN_TRIG; 
    }
}

__attribute__((vector(20), interrupt(IPL3SOFT), nomips16))
void InterrupcionTimer5(void) {
    IFS0bits.T5IF = 0;
    distancia=TMR5/147;
    TMR5=0;
    }


int getTicks() {
    uint32_t _ticks;
    asm("di")
    _ticks=ticks;
    asm("ei")
    return _ticks;
}


int getDistancia() {
    uint32_t _distancia;
    asm("di")
    _distancia=distancia;
    asm("ei")
    return _distancia;
}
