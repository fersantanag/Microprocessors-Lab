#include <Arduino.h>

#ifndef RECEIVER_H
#define RECEIVER_H

void initReceiver();
void initRX();
void computeRX();
void sendString(String s);

#endif /* RECEIVER_H */
