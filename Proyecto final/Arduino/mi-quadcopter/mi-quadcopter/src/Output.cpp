#include "Arduino.h"
#include "types.h"
#include "Algorithm.h"
#include "Output.h"

#define PIN_F_L 3
#define PIN_F_R 10
#define PIN_B_L 11
#define PIN_B_R 9

void initOutput() {
    pinMode(PIN_F_L, OUTPUT);
    pinMode(PIN_F_R, OUTPUT);
    pinMode(PIN_B_L, OUTPUT);
    pinMode(PIN_B_R, OUTPUT);
}


void writeMotors() {
    //Serial.println(motor[0]);
    analogWrite(PIN_F_L, motor[3]>>3);
    analogWrite(PIN_F_R, motor[1]>>3);
    analogWrite(PIN_B_L, motor[2]>>3);
    analogWrite(PIN_B_R, motor[0]>>3);
}

void mixTable() {
  int16_t maxMotor;
  uint8_t i;
  #if defined(DYNBALANCE)
    return;
  #endif
  #define PIDMIX(X,Y,Z) rcCommand[THROTTLE] + axisPID[ROLL]*X + axisPID[PITCH]*Y + YAW_DIRECTION * axisPID[YAW]*Z
  #define SERVODIR(n,b) ((conf.servoConf[n].rate & b) ? -1 : 1)

  /****************                   main Mix Table                ******************/
 
    motor[0] = PIDMIX(-1,+1,-1); //REAR_R
    motor[1] = PIDMIX(-1,-1,+1); //FRONT_R
    motor[2] = PIDMIX(+1,+1,+1); //REAR_L
    motor[3] = PIDMIX(+1,-1,-1); //FRONT_L
 
  /****************                normalize the Motors values                ******************/
    maxMotor=motor[0];
    for(i=1; i< NUMBER_MOTOR; i++)
      if (motor[i]>maxMotor) maxMotor=motor[i];
    for(i=0; i< NUMBER_MOTOR; i++) {
      if (maxMotor > MAXTHROTTLE) // this is a way to still have good gyro corrections if at least one motor reaches its max.
        motor[i] -= maxMotor - MAXTHROTTLE;
      motor[i] = constrain(motor[i], conf.minthrottle, MAXTHROTTLE);
      if ((rcData[THROTTLE] < MINCHECK) && !f.BARO_MODE)
      #ifndef MOTOR_STOP
        motor[i] = conf.minthrottle;
      #else
        motor[i] = MINCOMMAND;
      #endif
      if (!f.ARMED)
        motor[i] = MINCOMMAND;
    }

 
}