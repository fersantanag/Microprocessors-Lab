#include <xc.h>
#include "Utils.h"
#include "types.h"
#include "Algorithm.h"
#include "Output.h"

#define PIN_F_L 6   // RC6 -> OC4
#define PIN_F_R 8   // RC8 -> OC2
#define PIN_B_L 9   // RC9 -> OC3
#define PIN_B_R 7   // RC7 -> OC1

void initOutput() {
    
 
    
    TRISC &= ~(1<<PIN_F_L);   
    TRISC &= ~(1<<PIN_F_R);   
    TRISC &= ~(1<<PIN_B_L);   
    TRISC &= ~(1<<PIN_F_R);   
    
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPC6R = 0x5;    // C6 a OC4
    RPC7R = 0x5;    // C7 a OC1
    RPC8R = 0x5;    // C8 a OC2
    RPC9R = 0x5;    // C9 a OC3
    SYSKEY = 0;
    
    // T2
    T2CON = 0;
    TMR2 = 0;
    PR2 = 10203;    // 5000000/490-1
    T2CON = 0x8000; // Activamos
    
    // OC1
    OC1CON = 0;
    OC1R = 5000;
    OC1RS = 5000;
    OC1CON = 0x8006;
    
    // OC2
    OC2CON = 0;
    OC2R = 5000;
    OC2RS = 5000;
    OC2CON = 0x8006;
    
    // OC3
    OC3CON = 0;
    OC3R = 5000;
    OC3RS = 5000;
    OC3CON = 0x8006;
    
    // OC4
    OC4CON = 0;
    OC4R = 5000;
    OC4RS = 5000;
    OC4CON = 0x8006;
    
}


void writeMotors() {
    //Serial.println(motor[0]);
    /*analogWrite(PIN_F_L, motor[3]>>3);
    analogWrite(PIN_F_R, motor[1]>>3);
    analogWrite(PIN_B_L, motor[2]>>3);
    analogWrite(PIN_B_R, motor[0]>>3);*/
    
    OC4RS = (int) (motor[3]*5);
    OC2RS = (int) (motor[1]*5);
    OC3RS = (int) (motor[2]*5);
    OC1RS = (int) (motor[0]*5);
    
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