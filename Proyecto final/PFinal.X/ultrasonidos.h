/**
 * @file ultrasonidos.h
 *
 * @author Fernando & Jorge
 */
// ------------------------------------------------------
#ifndef _ULTRASONIDOS_H
#define _ULTRASONIDOS_H

#define DIV 10

void InicializarTemp();
int InicializarUltra(void);
int getDistancia(void);
uint32_t micros(void);

#endif