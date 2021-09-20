
#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "types.h"

#define RC_CHANS 8
#define MINCHECK 1100
#define MAXCHECK 1900
#define MINTHROTTLE 1150 // (*) (**)
#define MAXTHROTTLE 2000
#define NUMBER_MOTOR     4
#define MINCOMMAND  1000

/* tail precomp from collective */
#define YAW_COLL_PRECOMP 10           // (*) proportional factor in 0.1. Higher value -> higher precomp effect. value of 10 equals no/neutral effect
#define YAW_COLL_PRECOMP_DEADBAND 120 // (*) deadband for collective pitch input signal around 0-pitch input value


// default POSHOLD control gains
#define POSHOLD_P              .15
#define POSHOLD_I              0.0
#define POSHOLD_IMAX           20        // degrees

#define POSHOLD_RATE_P         3.4
#define POSHOLD_RATE_I         0.14      // Wind control
#define POSHOLD_RATE_D         0.053     // try 2 or 3 for POSHOLD_RATE 1
#define POSHOLD_RATE_IMAX      20        // degrees

// default Navigation PID gains
#define NAV_P                  2.5
#define NAV_I                  0.33      // Wind control
#define NAV_D                  0.083      //
#define NAV_IMAX               20        // degrees


#define YAW_DIRECTION 1

#define LOOP_TIME 2800/DIV

#define MIDRC 1500

extern uint16_t calibratingA;
extern uint16_t calibratingG;

extern int16_t  i2c_errors_count;
extern global_conf_t global_conf;
extern conf_t conf;


extern imu_t imu;

extern alt_t alt;
extern att_t att;


extern int16_t gyroZero[3];

extern flags_struct_t f;

extern int16_t axisPID[3];
extern int16_t motor[8];

extern int16_t rcData[RC_CHANS];
extern int16_t rcCommand[4];
extern int16_t lookupPitchRollRC[5];
extern uint16_t lookupThrottleRC[11];



void annexCode();
void go_arm();
void go_disarm();



#endif /* ALGORITHM_H */