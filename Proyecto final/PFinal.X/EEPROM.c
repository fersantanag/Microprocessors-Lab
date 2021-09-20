#include <xc.h>
#include "Utils.h"
#include "types.h"
#include "Algorithm.h"
#include "EEPROM.h"


void readGlobalSet() {
    global_conf.currentSet = 0;
    global_conf.accZero[ROLL] = 5000;    // for config error signalization
}

int readEEPROM() {
    uint8_t i;
    int8_t tmp;
    uint8_t y;


    global_conf.currentSet=0;
    LoadDefaults();                 // force load defaults 
    // 500/128 = 3.90625    3.9062 * 3.9062 = 15.259   1526*100/128 = 1192
    for(i=0;i<5;i++) {
        lookupPitchRollRC[i] = (1526+conf.rcExpo8*(i*i-15))*i*(int32_t)conf.rcRate8/1192;
    }
    for(i=0;i<11;i++) {
        tmp = 10*i-conf.thrMid8;
        y = conf.thrMid8;
        if (tmp>0) y = 100-y;
        lookupThrottleRC[i] = 100*conf.thrMid8 + tmp*( (int32_t)conf.thrExpo8*(tmp*tmp)/((uint16_t)y*y)+100-conf.thrExpo8 );       // [0;10000]
        lookupThrottleRC[i] = conf.minthrottle + (uint32_t)((uint16_t)(MAXTHROTTLE-conf.minthrottle))* lookupThrottleRC[i]/10000;  // [0;10000] -> [conf.minthrottle;MAXTHROTTLE]
    }

    return 1;    // setting is OK
}


void LoadDefaults() {
    uint8_t i;
 
    conf.pid[ROLL].P8     = 33;  conf.pid[ROLL].I8    = 30; conf.pid[ROLL].D8     = 23;
    conf.pid[PITCH].P8    = 33; conf.pid[PITCH].I8    = 30; conf.pid[PITCH].D8    = 23;
    conf.pid[PIDLEVEL].P8 = 90; conf.pid[PIDLEVEL].I8 = 10; conf.pid[PIDLEVEL].D8 = 100;

    conf.pid[YAW].P8      = 68;  conf.pid[YAW].I8     = 45;  conf.pid[YAW].D8     = 0;
    conf.pid[PIDALT].P8   = 64; conf.pid[PIDALT].I8   = 25; conf.pid[PIDALT].D8   = 24;

    conf.pid[PIDPOS].P8  = POSHOLD_P * 100;     conf.pid[PIDPOS].I8    = POSHOLD_I * 100;       conf.pid[PIDPOS].D8    = 0;
    conf.pid[PIDPOSR].P8 = POSHOLD_RATE_P * 10; conf.pid[PIDPOSR].I8   = POSHOLD_RATE_I * 100;  conf.pid[PIDPOSR].D8   = POSHOLD_RATE_D * 1000;
    conf.pid[PIDNAVR].P8 = NAV_P * 10;          conf.pid[PIDNAVR].I8   = NAV_I * 100;           conf.pid[PIDNAVR].D8   = NAV_D * 1000;
  
    conf.pid[PIDMAG].P8   = 40;

    conf.pid[PIDVEL].P8 = 0;      conf.pid[PIDVEL].I8 = 0;    conf.pid[PIDVEL].D8 = 0;

    conf.rcRate8 = 90; conf.rcExpo8 = 65;
    conf.rollPitchRate = 0;
    conf.yawRate = 0;
    conf.dynThrPID = 0;
    conf.thrMid8 = 50; conf.thrExpo8 = 0;
    for(i=0;i<CHECKBOXITEMS;i++) {conf.activate[i] = 0;}
    conf.angleTrim[0] = 0; conf.angleTrim[1] = 0;
    conf.powerTrigger1 = 0;


                 //if no servo defined then zero out the config variables to prevent passing false data to the gui.
//    for(i=0;i<8;i++) {
//        conf.servoConf[i].min = 0;
//        conf.servoConf[i].max = 0;
//        conf.servoConf[i].middle = 0;
//        conf.servoConf[i].rate = 0;
//      }

  //update_constants(); // this will also write to eeprom
  conf.minthrottle = MINTHROTTLE;
  conf.yawCollPrecomp = YAW_COLL_PRECOMP;
  conf.yawCollPrecompDeadband = YAW_COLL_PRECOMP_DEADBAND;


}