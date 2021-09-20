#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Receiver.h"
#include "Algorithm.h"

#define TAM_COLA 100

#define MAX_TIME 1000000

typedef struct {
    char cola[TAM_COLA];
    int8_t icab;
    int8_t icol;
} cola_t;


#define PIN_RX 2
#define PIN_TX 7

SoftwareSerial phone(PIN_RX, PIN_TX); // RX, TX
cola_t cola;
//static int16_t *rc; // Throtle, Roll, Pitch, Yaw

static uint32_t previous_time = 0;

static int16_t last_command_start = 0;
static int16_t last_command_end = 0;
static int16_t reading_command_start = 0;


void executeCommand();
void readCoordinates(String command, int8_t start_i, int16_t *x, int16_t *y);
String readNextCommand();

int checkIntegrity(String command);
int isNumber(char c);


void computeRX() {
    char c;
    while(phone.available()>0) {
        /*if (cola.icab+1 == cola.icol || (cola.icab+1 == TAM_COLA && cola.icol == 0)) {
            // No podemos leer
            executeCommand();   // char lost
        } else {
            c = phone.read();
            cola.cola[cola.icab] = c;
            cola.icab++;
            if (cola.icab == TAM_COLA)
                cola.icab = 0;
            if (c == '\n')
                executeCommand();

        }*/
        c = phone.read();
        cola.cola[cola.icab] = c;
        cola.icab++;
        if (cola.icab == TAM_COLA)
            cola.icab = 0;
        if (c == 'S')
            reading_command_start = cola.icab;
        if (c == '\n') {
            last_command_end = cola.icab-1;
            if (last_command_end == -1)
                last_command_end = TAM_COLA-1;
            last_command_start = reading_command_start;
            executeCommand();            
        }
        
    }
    /*if (micros()-previous_time > MAX_TIME && f.ARMED)
        go_disarm();*/
}

void initReceiver() {
    pinMode(PIN_RX, INPUT);
    pinMode(PIN_TX, OUTPUT);
    phone.begin(9600);
    cola.icab = 0;
    cola.icol = 0;
}

void executeCommand() {
    String command = readNextCommand();
    if (command.charAt(0) == 'D' && !f.ARMED) {
        go_arm();
        return;
    }
    if (command.charAt(0) == 'd' && f.ARMED) {
        go_disarm();
        return;
    }
    if (command.charAt(0) == 'G' && !f.ARMED) {
        calibratingG = 512;
        return;
    }
    //previous_time = micros();
    /*Serial.print("Executing command: ");
    Serial.println(command);
    int integrity = checkIntegrity(command);
    Serial.print("Integrity: ");
    Serial.println(integrity);*/
    if (!checkIntegrity(command))
        return;
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

/*String readNextCommand() {
    String command = "";
    char c;
    int start = 0;
    while (true) {
        c = cola.cola[cola.icol];
        cola.icol++;
        if (cola.icol == TAM_COLA)
            cola.icol = 0;
        if (start == 0 && c == 'S') {
            start = 1;
            continue;
        }
        if (!start) 
            continue;
        if (c == '\n')
            break;
        command += c;
    }
    return command;
}*/

String readNextCommand() {
    /*Serial.println("Reading command");
    Serial.print("Start: ");
    Serial.println(last_command_start);
    Serial.print("End: ");
    Serial.println(last_command_end);*/
    String command = "";
    char c;
    int i = last_command_start;
    while (i != last_command_end) {
        c = cola.cola[i];
        command += c;
        i++;
        if (i == TAM_COLA)
            i = 0;
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

int checkIntegrity(String command) {
    if (!((command.charAt(0) == 'R' && command.charAt(1) == 'P') || 
        (command.charAt(0) == 'T' && command.charAt(1) == 'Y')))
        return 0;
    if (command.charAt(2) != 'X')
        return 0;
    int i = 3;
    while (i<7) {
        if (!isNumber(command.charAt(i)))
            break;
        i++;
    }
    if (command.charAt(i) != 'Y')
        return 0;
    i++;
    int tmp = i+4;
    while (i<tmp) {
        if (i == command.length()-1 && isNumber(command.charAt(i)));
            return 1;
        if (!isNumber(command.charAt(i)))
            break;
        i++;
    }
    return 0;
}

int isNumber(char c) {
    int i = c;
    if (i >= 48 && i <= 57)
        return 1;
    return 0;
}

void sendString(String s) {
    phone.print(s);
}