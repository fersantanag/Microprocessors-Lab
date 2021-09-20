#include <xc.h>
#include "main.h"
#include "Utils.h"
#include "Output.h"





void initOutput() {
    
 
    
    /*TRISC &= ~(1<<PIN_F_L);   
    TRISC &= ~(1<<PIN_F_R);   
    TRISC &= ~(1<<PIN_B_L);   
    TRISC &= ~(1<<PIN_F_R);  */ 
    
    
    
    
    // T2
    T2CON = 0;
    TMR2 = 0;
    PR2 = 10203;    // 5000000/490-1
    IEC0bits.T2IE = 0;
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
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPC6R = 0x5;    // C6 a OC4
    RPC7R = 0x5;    // C7 a OC1
    RPC8R = 0x5;    // C8 a OC2
    RPC9R = 0x5;    // C9 a OC3
    SYSKEY = 0;
    
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
    
    /*OC4RS = 5000;
    OC2RS = 5000;
    OC3RS = 5000;
    OC1RS = 5000;*/
    
}

void computeMotors() {
    
    if (!armed) {
        motor[F_L] = 1000;
        motor[F_R] = 1000;
        motor[B_L] = 1000;
        motor[B_R] = 1000;
        return;
    }
    
    // Receives rcData
    int add_roll, add_pitch, add_yaw, th;
    th = rcData[THROTTLE];
    add_roll = rcData[ROLL]-1500;
    add_pitch = rcData[PITCH]-1500;
    add_yaw = rcData[YAW]-1500;
    
    motor[F_L] = constrain(th+add_roll-add_pitch-add_yaw, 1000, 2000);
    motor[F_R] = constrain(th-add_roll-add_pitch+add_yaw, 1000, 2000);
    motor[B_L] = constrain(th+add_roll+add_pitch+add_yaw, 1000, 2000);
    motor[B_R] = constrain(th-add_roll+add_pitch-add_yaw, 1000, 2000);
    
    
    
    
}