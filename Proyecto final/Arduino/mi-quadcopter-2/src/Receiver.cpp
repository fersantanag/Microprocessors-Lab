#include <Arduino.h>
#include <SoftwareSerial.h>
#include "main.h"
#include "Receiver.h"

#define TAM_COLA 100

typedef struct {
    char cola[TAM_COLA];
    int8_t icab;
    int8_t icol;
} cola_t;


#define PIN_RX 6
#define PIN_TX 7

SoftwareSerial phone(PIN_RX, PIN_TX); // RX, TX
cola_t cola;
//static int16_t *rc; // Throtle, Roll, Pitch, Yaw

void executeCommand();
void readCoordinates(String command, int8_t start_i, int16_t *x, int16_t *y);
String readNextCommand();

void initReceiver() {
    initRX();
}

void computeRX() {
    char c;
    while(phone.available()>0) {
        if (cola.icab+1 == cola.icol || (cola.icab+1 == TAM_COLA && cola.icol == 0)) {
            // No podemos leer
        } else {
            c = phone.read();
            cola.cola[cola.icab] = c;
            cola.icab++;
            if (cola.icab == TAM_COLA)
                cola.icab = 0;
            if (c == '\n')
                executeCommand();

        }
    }
}

void initRX() {
    pinMode(PIN_RX, INPUT);
    pinMode(PIN_TX, OUTPUT);
    phone.begin(9600);
    cola.icab = 0;
    cola.icol = 0;
}

void executeCommand() {
    String command = readNextCommand();
    delay(2);
    int16_t x[1];
    int16_t y[1];
    *x = 0;
    *y = 0;
    if (command.charAt(0) == 'T' && command.charAt(1) == 'Y') {
        readCoordinates(command, 2, x, y);
        rcData[YAW] = *x+1000;
        rcData[THROTTLE] = 2000-*y;
        /*rc[3] = *x-500;
        if (*y>500)
            *y = 500;
        rc[0] = (500-*y)*2;*/
    } else if (command.charAt(0) == 'R' && command.charAt(1) == 'P') {
        readCoordinates(command, 2, x, y);
        /*rc[1] = *x-500;
        rc[2] = 500-*y;*/
        rcData[ROLL] = *x+1000;
        rcData[PITCH] = 2000-*y;
    } 
}

String readNextCommand() {
    String command = "";
    char c;
    while (true) {
        c = cola.cola[cola.icol];
        cola.icol++;
        if (cola.icol == TAM_COLA)
            cola.icol = 0;
        if (c == '\n')
            break;
        command += c;
    }
    return command;
}

void readCoordinates(String command, int8_t start_i, int16_t x[], int16_t y[]) {
    uint8_t c, i=start_i+1;
    x[0] = 0;
    // Read X
    while(true) {
        c = command.charAt(i);
        i++;
        if (c == 'Y')
            break;
        x[0] = x[0] * 10;
        x[0] = x[0] + (c-48);
    }
    // Read Y
    *y = 0;
    while (i<command.length()) {
        c = command.charAt(i);
        
        i++;
        *y = *y * 10;
        *y = *y + (c-48);
    }
} 