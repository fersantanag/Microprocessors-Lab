/* 
 * File:   main3.c
 * Author: jorge
 *
 * Created on 09 April 2021, 10:54
 */

#include <xc.h>
#include "Pic32Ini.h"
#include "driver_uart.h"

#define PIN_RX 13
#define PIN_TX 7

void resultError() {
    putsUART("Error\n");
}

int convertPin(char s) {
    if (s >= 48 && s <= 57)
        return s - 48;
    else if (s >= 65 && s <= 70)
        return s - 65 + 10;
    return -1;
}

void executeCommand(char* s) {

    // Comrpobar formato
    if (s[0] != 'P' || s[2] != ',' || s[4] != ',') {
        resultError();
        return;
    }
    
///PD///    
    if (s[1] == 'D') {

        // Comrpobar formato
        if (s[6] != ',' || s[8] != '\n') {
            resultError();
            return;
        }

        // Leer valores
        char puerto = s[3];
        int pin = convertPin(s[5]);

        if (pin == -1) {
            resultError();
            return;
        }

        int dir = convertPin(s[7]);

        if (dir != 0 && dir != 1) {
            resultError();
            return;
        }

        // Cambiar modo del pin
        if (puerto == 'A') {
            if(dir == 0)
            TRISACLR = 1 << pin;
            else
            TRISASET = 1 << pin;
        } else if (puerto == 'B') {
            if(dir == 0)
            TRISBCLR = 1 << pin;
            else
            TRISBSET = 1 << pin;
        } else if (puerto == 'C') {
            if(dir == 0)
            TRISCCLR = 1 << pin;
            else
            TRISCSET = 1 << pin;
        } else {
            resultError();
            return;
        }

        putsUART("OK\n");

///PI///        
    } else if (s[1] == 'I') {

        // Comrpobar formato
        if (s[6] != '\n') {
            resultError();
            return;
        }

        // Leer valores
        char puerto = s[3];
        int pin = convertPin(s[5]);

        if (pin == -1) {
            resultError();
            return;
        }


        // Leer le pin
        int v;
        if (puerto == 'A') {
            v = (PORTA >> pin) & 1;
        } else if (puerto == 'B') {
            v = (PORTB >> pin) & 1;
        } else if (puerto == 'C') {
            v = (PORTC >> pin) & 1;
        } else {
            resultError();
            return;
        }

        char s_ret[5];
        s_ret[0] = 'P';
        s_ret[1] = 'I';
        s_ret[2] = ',';
        s_ret[3] = v + 48;
        s_ret[4] = '\n';
        putsUART(s_ret);

///PO///    
    } else if (s[1] == 'O') {

        // Comrpobar formato
        if (s[6] != ',' || s[8] != '\n') {
            resultError();
            return;
        }

        // Leer valores
        char puerto = s[3];
        int pin = convertPin(s[5]);

        if (pin == -1) {
            resultError();
            return;
        }

        int valor = convertPin(s[7]);

        if (valor != 0 && valor != 1) {
            resultError();
            return;
        }

        // Poner el valor adecuado
        if (puerto == 'A') {
            if(valor == 0)
            LATACLR = 1 << pin;
            else
            LATASET = 1 << pin;
        } else if (puerto == 'B') {
            if(valor == 0)
            LATBCLR = 1 << pin;
            else
            LATBSET = 1 << pin;
        } else if (puerto == 'C') {
            if(valor == 0)
            LATCCLR = 1 << pin;
            else
            LATCSET = 1 << pin;
        } else {
            resultError();
            return;
        }

        putsUART("OK\n");


    } else {
        resultError();
    }
}

int main(void) {

    // Configuramos como digitales
    ANSELB &= ~(1 << PIN_RX | 1 << PIN_TX);

    // El pulsador y el rx son entradas
    TRISA = 0;
    TRISB = 1 << PIN_RX;
    TRISC = 0;

    // LEDs apagados
    LATA = 0;
    LATB = 1 << PIN_TX; // Transmisor inhabilitado
    LATC = 0xF;

    // Conectamos U1RX y U1TX
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x0;

    // Inicializar UART
    InicializarUART1(9600);

    InicializarReloj();

    // Iniciamos interrupciones
    INTCONbits.MVEC = 1;
    asm(" ei");

    char c;

    char s[10]; // El comando más largo ocupa 9
    int icab_s = 0;

    while (1) {

        c = getcUART();
        if (c != '\0') {

            s[icab_s] = c;
            icab_s++;

            if (c == '\n') {
                executeCommand(s);
                icab_s = 0;
            }
        }
    }
    return 0;
}









