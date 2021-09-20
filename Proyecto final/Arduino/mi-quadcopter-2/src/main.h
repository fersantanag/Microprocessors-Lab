#include "Arduino.h"

#ifndef MAIN_H
#define MAIN_H

extern int16_t rcData[4];


enum rc{
  ROLL,
  PITCH,
  YAW,
  THROTTLE
};

#endif