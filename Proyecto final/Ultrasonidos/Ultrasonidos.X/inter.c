
#include <xc.h>
#define PIN_PULSADOR 5
#define PIN_TRIG 9 //RA9
#define PIN_ECHO 14 //RB14
int mide = 0; // Compartida con la interrupci´on del temporizador 2

int main(void) {
    int puls_ant, puls_act;
    int t_eco, distancia;
    
    ANSELB &= ~(1 << PIN_PULSADOR |  1 << PIN_ECHO); // Pines digitales
    ANSELA &= ~(1 << PIN_TRIG);
    ANSELC &= ~(0xF); // Pines de los LEDs digitales
    
    LATB = 0;
    LATC = 0xF; // Apago los LEDs 
    
    TRISA = 0;
    TRISB = 1 << PIN_PULSADOR | 1 << PIN_ECHO;
    TRISC = 0;
    
    // Se inicializa el temporizador 2
    T2CON = 0;
    PR2 = 49; // 1ms 5 MHz (div = 1)
    TMR2 = 0;
    IFS0 &= ~(1 <<9); // Se borra el flag del timer 2
    IEC0 |= (1 <<9); // Se habilita su interrupción
    IPC2 &= ~(0x1F); // Se ponen a 0 la prioridad y subprioridad de T2
    IPC2 |= 2 << 2; // Prioridad Timer 2 = 2
    IPC2 |= 1; // Subprioridad Timer 2 = 1
    T2CON = 0x8000; // Timer 2 ON
    
    // Ahora inicializamos el temporizador 5
    T5CON = 0;
    TMR3 = 0;
    IFS0 &= ~(1 <<14); // Se borra el flag del timer 3
    T5CON = 0x8090; // Timer ON, TGATE = 1, TCKPS = 1 (prescalado 2)
    SYSKEY = 0xAA996655; // Se desbloquean los registros
    SYSKEY = 0x556699AA; // de configuraci´on.
    T5CKR = 1; // T5CK conectado al echo = RPB14
    SYSKEY = 0x1CA11CA1; // Se vuelven a bloquear.
    INTCONbits .MVEC = 1; // Se pone en modo multivector
    
    
    asm(" ei"); // y se habilitan las interrupciones
    puls_ant = (PORTB >> PIN_PULSADOR)&1;
    
    while (1) {
        puls_act = (PORTB >> PIN_PULSADOR)&1;
        if ((puls_ant != puls_act) && (puls_act == 0)) {
            asm(" di"); // Zona cr?tica: variable mide compartida con interrup
            mide = 1;
            asm(" ei");
            // Espero el final del pulso
            while (IFS0bits.T5IF == 0);
            IFS0bits.T5IF = 0;// Borro el flag           
            distancia = TMR3 / 147;
            TMR3 = 0; // Lo borro para la siguiente medida
            // Pongo a 1 los LEDs para apagarlos todos
            LATASET = 0xF;
            // Ahora pongo a cero los LEDs que corresponda encender de los 4 bits
            LATACLR = distancia & 0xF;
        }
        puls_ant = puls_act;
    }
}
    
__attribute__((vector(8), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer2(void) {
    IFS0bits.T2IF = 0;
    
    if (mide) {
        LATASET = 1 << PIN_TRIG; // Se pone a 1 el pin trig. 
        // En la siguiente interrupcion se podra a 0
        mide = 0;
    }
    else{
        LATACLR = 1 << PIN_TRIG;
    }
}
