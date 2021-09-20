#include <xc.h>
#include "Pic32Ini.h"
#include "Receiver.h"
#include "ultrasonidos.h"
#include "Output.h"
#include "Utils.h"
#include "Sensors.h"
#include "main.h"
#include "driver_uart2.h"




int armed = 0;
int police_mode = 0;
int16_t motor[4] = {1100, 1100, 1100, 1100};
int16_t rcData[4];
int32_t gyro[3];
int16_t calibratingG = 0;

void go_arm() {
    armed = 1;    
}

void go_disarm() {
    armed = 0;   
}



int main(void) {
    
    //ANSELA &= ~(1 << PIN_TRIG);
    //ANSELB &= ~((1<<PIN_RX) | 1<<(PIN_TX) | (1 << PIN_ECHO));
    ANSELB &= ~((1<<PIN_RX) | (1<<PIN_TX) | (1<<LEDS_AZUL) | (1<<LEDS_ROJO) | (1<<PIN_RX_2) | (1<<PIN_TX_2));
    ANSELC &= ~(0xF | (1<<PIN_F_L )| (1<<PIN_F_R) | (1<<PIN_B_L) | (1<<PIN_B_R) | (1<<PIN_ECHO) | (1<<PIN_TRIG)); // Pines de los LEDs digitales
    //ANSELC &= ~((1<<PIN_F_L )| (1<<PIN_F_R) | (1<<PIN_B_L) | (1<<PIN_B_R));
    
    TRISA = 0;
    //TRISB = 1<<PIN_RX | 1 <<PIN_ECHO;
    TRISB = (1<<PIN_RX) | (1<<PIN_RX_2);
    TRISC = 1<<PIN_ECHO;
    
    LATA = 0;
    LATB = 1<<PIN_TX;
    LATC = 0;
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    U2RXR = 4;  // U1RX a RB8
    RPB9R = 2;  // U1TX a RB9
    SYSKEY = 0x0;

    initOutput();         // Timer 2 (no IRS) y OC 1-4
    
    //initSensors();
    InicializarReloj();
   
    InicializarTemp();      // Timer 1 y su IRS (Prioridad: 4)
    initReceiver();         // UART 1 y su IRS  (Prioridad: 3)
    //InicializarUART2(9600);
    InicializarUltra();     // Timer 5 y su IRS (Prioridad: 5)
    
    

    
    // Iniciamos interrupciones
    INTCONbits.MVEC = 1;
    asm(" ei");
    
    char c;
    char s[2];
    s[1] = '\0';
    while (1) {
        int32_t start_time = micros();
        while (micros()-start_time<10000);
       
        
        /*c = getcUART2();
        if (c != '\0') {
            s[0] = c;
            sendString(s);
            c = getcUART2();
        }*/
        
        computeRX();
        iniciarMedida();
        int len = 0;
        int d = getDistancia();
        /*putsUART("Motor: \0");
        putsUART(toCharArray(motor[0], &len));
        */
        //putsUART(toCharArray(d, &len));
        /*Gyro_getADC();
        LATCSET = 0xF;
        LATCCLR = gyro[YAW] & 0xF;*/
        /*c = getcUART2();
        while (c != '\0') {
            s[0] = c;
            sendString(s);
            c = getcUART2();
        }*/
        
        computeMotors();
        writeMotors();
        sendInfo();
        putsUART("\n\0");
    }
    

    return 0;
}

