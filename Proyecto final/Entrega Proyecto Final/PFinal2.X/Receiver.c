#include <xc.h>
#include "driver_uart.h"
#include "main.h"
#include "ultrasonidos.h"
#include "Utils.h"
#include "Output.h"
#include "Receiver.h"

#define TAM_COLA 100

#define MAX_TIME 1000000

typedef struct {
    char cola[TAM_COLA];
    int8_t icab;
    int8_t icol;
} cola_t;


//SoftwareSerial phone(PIN_RX, PIN_TX); // RX, TX
cola_t cola;
//static int16_t *rc; // Throtle, Roll, Pitch, Yaw

static uint32_t previous_time = 0;

static int16_t last_command_start = 0;
static int16_t last_command_end = 0;
static int16_t reading_command_start = 0;


void executeCommand();

void readCoordinates(char* command, int len_command, int8_t start_i, int16_t x[], int16_t y[]);
char* readNextCommand(int* len_command);
int checkIntegrity(char* command, int len_command);
int isNumber(char c);


void computeRX() {
    char c;
    while(1) {
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
        c = getcUART();
        if (c == '\0')
            break;
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
    //pinMode(PIN_RX, INPUT);
    //pinMode(PIN_TX, OUTPUT);
    //phone.begin(115200);
    InicializarUART1(9600);
    cola.icab = 0;
    cola.icol = 0;
}

void executeCommand() {
    int len_command;
    char* command = readNextCommand(&len_command);
    if (command[0] == 'D' && !armed) {
        go_arm();
        return;
    }
    if (command[0] == 'd' && armed) {
        go_disarm();
        return;
    }
    if (command[0] == 'P') {
        LATBSET = 1<<LEDS_ROJO;
        police_mode = 1;
        return;
    }
    if (command[0] == 'p') {
        police_mode = 0;
        return;
    }
    if (command[0] == 'Z') {
        putsUART("Z\0");
        return;
    }
        
    /*if (command[0]== 'G' && !f.ARMED) {
        calibratingG = 512;
        return;
    }*/
    //previous_time = micros();
    /*Serial.print("Executing command: ");
    Serial.println(command);
    int integrity = checkIntegrity(command);
    Serial.print("Integrity: ");
    Serial.println(integrity);*/
    if (!checkIntegrity(command, len_command))
        return;
    int16_t x[1];
    int16_t y[1];
    *x = 0;
    *y = 0;
    if (command[0] == 'T' && command[1] == 'Y') {
        readCoordinates(command, len_command, 2, x, y);
        rcData[YAW] = *x+1000;
        rcData[THROTTLE] = 2000-*y;
        /*rc[3] = *x-500;
        if (*y>500)
            *y = 500;
        rc[0] = (500-*y)*2;*/
    } else if (command[0] == 'R' && command[1] == 'P') {
        readCoordinates(command, len_command, 2, x, y);
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

char* readNextCommand(int* len_command) {
    /*Serial.println("Reading command");
    Serial.print("Start: ");
    Serial.println(last_command_start);
    Serial.print("End: ");
    Serial.println(last_command_end);*/
    static char command[100];
    char c;
    int j = 0;
    int i = last_command_start;
    while (i != last_command_end) {
        c = cola.cola[i];
        command[j] = c;
        j++;
        i++;
        if (i == TAM_COLA)
            i = 0;
    }
    if (last_command_end > last_command_start)
        *len_command = last_command_end-last_command_start;
    else
        TAM_COLA-1-(last_command_start-last_command_end);
    return command;
}

void readCoordinates(char* command, int len_command, int8_t start_i, int16_t x[], int16_t y[]) {
    uint8_t c, i=start_i+1;
    x[0] = 0;
    // Read X
    while(1) {
        c = command[i];
        i++;
        if (c == 'Y')
            break;
        x[0] = x[0] * 10;
        x[0] = x[0] + (c-48);
    }
    // Read Y
    *y = 0;
    while (i<len_command) {
        c = command[i];
        
        i++;
        *y = *y * 10;
        *y = *y + (c-48);
    }
}

int checkIntegrity(char* command, int len_command) {
    if (!((command[0] == 'R' && command[1] == 'P') || 
        (command[0] == 'T' && command[1] == 'Y')))
        return 0;
    if (command[2] != 'X')
        return 0;
    int i = 3;
    while (i<7) {
        if (!isNumber(command[i]))
            break;
        i++;
    }
    if (command[i] != 'Y')
        return 0;
    i++;
    int tmp = i+4;
    while (i<tmp) {
        if (i == len_command-1 && isNumber(command[i]));
            return 1;
        if (!isNumber(command[i]))
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

void sendString(char* s) {
    putsUART(s);
}

void sendInfo() {
    
    char s[100];
    int i = 0;
    int tmp, len_tmp;
    char* tmp_s;
    
    // Giroscopio
    
    
    
    // Distancia
    tmp = constrain(getDistancia(), 0, 200);
    i = 0;
    s[0] = '*';
    s[1] = 'G';
    i += 2;
    tmp_s = toCharArray(tmp, &len_tmp);
    addString(s, tmp_s, &i, len_tmp);
    s[i] = '*';
    s[i+1] = '\0';
    sendString(s);
    
    // Motor 1
    tmp = constrain(motor[F_L], 1000, 2000);
    i = 0;
    s[0] = '*';
    s[1] = 'Q';
    i += 2;
    tmp_s = toCharArray(tmp, &len_tmp);
    addString(s, tmp_s, &i, len_tmp);
    s[i] = '*';
    s[i+1] = '\0';
    sendString(s);
    
    tmp = constrain(motor[F_R], 1000, 2000);
    i = 0;
    s[0] = '*';
    s[1] = 'W';
    i += 2;
    tmp_s = toCharArray(tmp, &len_tmp);
    addString(s, tmp_s, &i, len_tmp);
    s[i] = '*';
    s[i+1] = '\0';
    sendString(s);
    
    tmp = constrain(motor[B_L], 1000, 2000);
    i = 0;
    s[0] = '*';
    s[1] = 'E';
    i += 2;
    tmp_s = toCharArray(tmp, &len_tmp);
    addString(s, tmp_s, &i, len_tmp);
    s[i] = '*';
    s[i+1] = '\0';
    sendString(s);
    
    tmp = constrain(motor[B_R], 1000, 2000);
    i = 0;
    s[0] = '*';
    s[1] = 'R';
    i += 2;
    tmp_s = toCharArray(tmp, &len_tmp);
    addString(s, tmp_s, &i, len_tmp);
    s[i] = '*';
    s[i+1] = '\0';
    sendString(s);
    
    
}