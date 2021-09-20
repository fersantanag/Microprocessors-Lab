#include "Utils.h"

#define PIN_TRIG 9 //RA9
#define PIN_ECHO 14 //RB14

void reverse(char* array, int n);




char digitToChar(int t) {
    if (t >= 48 && t<= 57)
        return (char) (t-48);
    return '\0';
}

/*
 * Convierte un entero de máximo 4 cifras a caracteres.
 
char* toChar(int n) {
    char resultado[4] = {0, 0, 0, 0};
    if (n == 0) {
        resultado[0] = '0';
        return resultado;
    }
    int tmp;
    for (int i=0; i<4; i++) {
        if (n == 0)
            break;
        tmp = n%10;
        resultado[4-1-i] = digitToChar(tmp);
        n = n/10;
    }
    return resultado;
}*/

char* toCharArray(int number, int* len_array) {
    
    static char arr[6];
    
    if (number == 0) {
        arr[0] = '0';
        arr[1] = '\0';
        *len_array = 1;
        return arr;
    }
    
    int neg = 0;
    if (number < 0) {
        neg = 1;
        number = -number;
    }
    
    int length = 0;
    int tmp  = number;
    while (tmp != 0) {
        length++;
        tmp /= 10;
    }
    
    *len_array = length + neg;
    
    int i = 0;
    
    if (neg) {
        arr[0] = '-';
        i++;
    }
    
    do {
        arr[i] = (number % 10) + '0';
        number /= 10;
        i++;
    } while (number != 0);
           
    reverse(&arr[neg], length);
    arr[length+neg] = '\0';
            
    return arr;
}

// Function to reverse elements of an array
void reverse(char* array, int n)
{
    char aux[n];
    int i;
    for (i = 0; i < n; i++) {
        aux[n - 1 - i] = array[i];
    }
    for (i = 0; i < n; i++) {
        array[i] = aux[i];
    }
}

void addString(char* a, char* b, int* current_i, int len_b) {
    int i;
    for (i = 0; i<len_b; i++) {
        a[*current_i+i] = b[i];
    }
    *current_i += len_b;
}

void Retardo (int retardo_ms) {
    
    int start_time = micros();
    while(micros()-start_time<1000*retardo_ms);
    
}

