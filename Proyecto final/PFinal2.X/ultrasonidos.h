/**
 * @file ultrasonidos.h
 *
 * @author Fernando & Jorge
 */
// ------------------------------------------------------
#ifndef _ULTRASONIDOS_H
#define _ULTRASONIDOS_H

//#define PIN_TRIG 9  //RA9
#define PIN_TRIG 5  //RC5

//#define PIN_ECHO 14 //RB14
#define PIN_ECHO 4 //RC4

void InicializarTemp();
int InicializarUltra(void);
void iniciarMedida();
int getDistancia(void);
int32_t micros(void);

#endif