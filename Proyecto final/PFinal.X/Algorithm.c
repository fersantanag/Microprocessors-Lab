#include <xc.h>
#include "Pic32Ini.h"

#include "Utils.h"
#include "types.h"
#include "Receiver.h"
#include "IMU.h"
#include "Sensors.h"
#include "EEPROM.h"
#include "Output.h"
#include "driver_uart.h"
#include "ultrasonidos.h"
#include "Algorithm.h"


uint32_t currentTime = 0;
uint16_t previousTime = 0;
uint16_t cycleTime = 0;     // this is the number in micro second to achieve a full loop, it can differ a little and is taken into account in the PID loop


uint16_t calibratingA = 0;  // the calibration is done in the main loop. Calibrating decreases at each cycle down to 0, then we enter in a normal mode.
uint16_t calibratingG;

int16_t  GPS_angle[2] = { 0, 0};                      // the angles that must be applied for GPS correction

int16_t  magHold; // [-180;+180]


int16_t  i2c_errors_count = 0;

global_conf_t global_conf;
conf_t conf;

alt_t alt;
att_t att;

flags_struct_t f;

#define FAILSAFE_DELAY     10                     // Guard time for failsafe activation after signal lost. 1 step = 0.1sec - 1sec in example
volatile int16_t failsafeCnt = 0;





// ******************
// rc functions
// ******************
#define ROL_LO  (1<<(2*ROLL))
#define ROL_CE  (3<<(2*ROLL))
#define ROL_HI  (2<<(2*ROLL))
#define PIT_LO  (1<<(2*PITCH))
#define PIT_CE  (3<<(2*PITCH))
#define PIT_HI  (2<<(2*PITCH))
#define YAW_LO  (1<<(2*YAW))
#define YAW_CE  (3<<(2*YAW))
#define YAW_HI  (2<<(2*YAW))
#define THR_LO  (1<<(2*THROTTLE))
#define THR_CE  (3<<(2*THROTTLE))
#define THR_HI  (2<<(2*THROTTLE))

int16_t rcData[RC_CHANS];    // interval [1000;2000]

uint8_t  rcOptions[CHECKBOXITEMS];
int16_t rcCommand[4];        // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW

int16_t lookupPitchRollRC[5];// lookup table for expo & RC rate PITCH+ROLL
uint16_t lookupThrottleRC[11];// lookup table for expo & mid THROTTLE



// **************
// gyro+acc IMU
// **************
int16_t gyroZero[3] = {0,0,0};

imu_t imu;

// *************************
// motor and servo functions
// *************************
int16_t axisPID[3];
int16_t motor[8];

// ************************
// EEPROM Layout definition
// ************************
static uint8_t dynP8[2], dynD8[2];




void annexCode() {
    

  static uint32_t calibratedAccTime;
  uint16_t tmp,tmp2;
  uint8_t axis,prop1,prop2;

  // PITCH & ROLL only dynamic PID adjustemnt,  depending on throttle value (or collective.pitch value for heli)
  #define DYN_THR_PID_CHANNEL THROTTLE
  prop2 = 128; // prop2 was 100, is 128 now
  if (rcData[DYN_THR_PID_CHANNEL]>1500) { // breakpoint is fix: 1500
    if (rcData[DYN_THR_PID_CHANNEL]<2000) {
      prop2 -=  ((uint16_t)conf.dynThrPID*(rcData[DYN_THR_PID_CHANNEL]-1500)>>9); //  /512 instead of /500
    } else {
      prop2 -=  conf.dynThrPID;
    }
  }

  for(axis=0;axis<3;axis++) {
    tmp = min(abs(rcData[axis]-MIDRC),500);
    if(axis!=2) { //ROLL & PITCH
      tmp2 = tmp>>7; // 500/128 = 3.9  => range [0;3]
      rcCommand[axis] = lookupPitchRollRC[tmp2] + ((tmp-(tmp2<<7)) * (lookupPitchRollRC[tmp2+1]-lookupPitchRollRC[tmp2])>>7);
      prop1 = 128-((uint16_t)conf.rollPitchRate*tmp>>9); // prop1 was 100, is 128 now -- and /512 instead of /500
      prop1 = (uint16_t)prop1*prop2>>7; // prop1: max is 128   prop2: max is 128   result prop1: max is 128
      dynP8[axis] = (uint16_t)conf.pid[axis].P8*prop1>>7; // was /100, is /128 now
      dynD8[axis] = (uint16_t)conf.pid[axis].D8*prop1>>7; // was /100, is /128 now
    } else {      // YAW
      rcCommand[axis] = tmp;
    }
    if (rcData[axis]<MIDRC) rcCommand[axis] = -rcCommand[axis];
  }
  tmp = constrain(rcData[THROTTLE],MINCHECK,2000);
  tmp = (uint32_t)(tmp-MINCHECK)*2559/(2000-MINCHECK); // [MINCHECK;2000] -> [0;2559]
  tmp2 = tmp/256; // range [0;9]
  rcCommand[THROTTLE] = lookupThrottleRC[tmp2] + (tmp-tmp2*256) * (lookupThrottleRC[tmp2+1]-lookupThrottleRC[tmp2]) / 256; // [0;2559] -> expo -> [conf.minthrottle;MAXTHROTTLE]
 

  if ( (calibratingA>0) || (calibratingG>0) ) { // Calibration phasis
    ; //LEDPIN_TOGGLE;
  } else {
    //if (f.ACC_CALIBRATED) { //LEDPIN_OFF;}
    //if (f.ARMED) {//LEDPIN_ON;}
    ;
  }



  if ( currentTime > calibratedAccTime ) {
    if (! f.SMALL_ANGLES_25) {
      // the multi uses ACC and is not calibrated or is too much inclinated
      f.ACC_CALIBRATED = 0;
      //LEDPIN_TOGGLE;
      calibratedAccTime = currentTime + 100000;
    } else {
      f.ACC_CALIBRATED = 1;
    }
  }


  /*
    #if !(defined(SERIAL_RX) && defined(PROMINI))  //Only one serial port on ProMini.  Skip serial com if SERIAL RX in use. Note: Spek code will auto-call serialCom if GUI data detected on serial0.
      serialCom();
    #endif

  */
  

}

void setup() {
    
  //SerialOpen(0,SERIAL0_COM_SPEED);

  //Serial.begin(115200);
    
    
   //InicializarUltra();

  /*LEDPIN_PINMODE;
  POWERPIN_PINMODE;
  BUZZERPIN_PINMODE;
  STABLEPIN_PINMODE;
  POWERPIN_OFF;*/
  initOutput();
  readGlobalSet();

  global_conf.currentSet=0;
  /* while(1) {                                                    // check settings integrity
    readEEPROM();                                               // check current setting integrity
    if(global_conf.currentSet == 0) break;                      // all checks is done
    global_conf.currentSet--;                                   // next setting for check
  }
  readGlobalSet();  */                            // reload global settings for get last profile number

   readEEPROM();                                 // load setting data from last used profile
  //blinkLED(2,40,global_conf.currentSet+1);          

  initReceiver();
  //initSensors();
  
  previousTime = micros();

  calibratingG = 512;
  //calibratingB = 200;  // 10 seconds init_delay + 200 * 25 ms = 15 seconds before ground pressure settles

  /************************************/


  f.SMALL_ANGLES_25=1; // important for gyro only conf

  //Serial.println("Setup finished");

}

void go_arm() {
  if(calibratingG == 0
    && f.ACC_CALIBRATED 
    ) {
    if(!f.ARMED && !f.BARO_MODE) { // arm now!
      f.ARMED = 1;
      magHold = att.heading;
    }
  } else if(!f.ARMED) { 
    //blinkLED(2,255,1);
    //SET_ALARM(ALRM_FAC_ACC, ALRM_LVL_ON);
    ;
  }
}

void go_disarm() {
  if (f.ARMED) {
    f.ARMED = 0;
  }
}


// ******** Main Loop *********
void loop () {


  static uint8_t rcDelayCommand; // this indicates the number of time (multiple of RC measurement at 50Hz) the sticks must be maintained to run or switch off motors
  static uint8_t rcSticks;       // this hold sticks position for command combos
  uint8_t axis,i;
  int16_t error,errorAngle;
  int16_t delta;
  int16_t PTerm = 0,ITerm = 0,DTerm, PTermACC, ITermACC;
  static int16_t lastGyro[2] = {0,0};
  static int16_t errorAngleI[2] = {0,0};
  static int32_t errorGyroI_YAW;
  static int16_t delta1[2],delta2[2];
  static int16_t errorGyroI[2] = {0,0};
 
  static uint16_t rcTime  = 0;
  //static int16_t initialThrottleHold;
  int16_t rc;
  int32_t prop = 0;


  if ((int16_t)(currentTime-rcTime) >0 ) { // 50Hz


    rcTime = currentTime + 20000/DIV;
    computeRX();
    // Failsafe routine - added by MIS


    // end of failsafe routine - next change is made with RcOptions setting

    // ------------------ STICKS COMMAND HANDLER --------------------
    // checking sticks positions
    uint8_t stTmp = 0;
    for(i=0;i<4;i++) {
      stTmp >>= 2;
      if(rcData[i] > MINCHECK) stTmp |= 0x80;      // check for MIN
      if(rcData[i] < MAXCHECK) stTmp |= 0x40;      // check for MAX
    }
    if(stTmp == rcSticks) {
      if(rcDelayCommand<250) rcDelayCommand++;
    } else rcDelayCommand = 0;
    rcSticks = stTmp;

    
    // perform actions    
    if (rcData[THROTTLE] <= MINCHECK) {            // THROTTLE at minimum
        errorGyroI[ROLL] = 0;
        errorGyroI[PITCH] = 0;
        errorGyroI_YAW = 0;
        errorAngleI[ROLL] = 0;
        errorAngleI[PITCH] = 0;
      if (conf.activate[BOXARM] > 0) {             // Arming/Disarming via ARM BOX
        if ( rcOptions[BOXARM] && f.OK_TO_ARM ) go_arm(); else if (f.ARMED) go_disarm();
      }
    }

    
    /*Serial.print(motor[0]);
    Serial.print(",");
    Serial.print(motor[1]);
    Serial.print(",");
    Serial.print(motor[2]);
    Serial.print(",");
    Serial.println(motor[3]);  */
    //Serial.println(f.ARMED);
    //Serial.println(rcSticks);
    //Serial.println(rcData[THROTTLE]);
    /*Serial.print(att.angle[0]);
    Serial.print(",");
    Serial.print(att.angle[1]);
    Serial.print(",");
    Serial.println(att.heading);*/
    /*Serial.print(f.ANGLE_MODE);
    Serial.print(",");
    Serial.println(f.HORIZON_MODE);*/

  
    /*char a[100];
    char *tmp;
    int tmp_len = 0;
    int current_i = 0;
    addString(a, "*G", &current_i, 2);
    tmp = toCharArray(imu.gyroData[ROLL], &tmp_len);
    addString(a, tmp, &current_i, tmp_len);
    addString(a, ",", &current_i, 1);
    tmp = toCharArray(imu.gyroData[PITCH], &tmp_len);
    addString(a, tmp, &current_i, tmp_len);
    addString(a, ",", &current_i, 1);
    tmp = toCharArray(imu.gyroData[YAW], &tmp_len);
    addString(a, "*", &current_i, 1);
    a[current_i] = '\0';
    
    sendString(a);*/
    
    putsUART("Hola\n\0");


    
    if(rcDelayCommand == 20) {
      /*Serial.println("Entering delay");
      Serial.println(f.ARMED);
      Serial.println(rcSticks);
      Serial.println(THR_LO + YAW_HI + PIT_CE + ROL_CE);*/

      

      if(f.ARMED) {                   // actions during armed
          if (conf.activate[BOXARM] == 0 && rcSticks == THR_LO + YAW_LO + PIT_CE + ROL_CE) go_disarm();    // Disarm via YAW
        
      } else {                        // actions during not armed
        i=0;
        if (rcSticks == THR_LO + YAW_LO + PIT_LO + ROL_CE) {    // GYRO calibration
          calibratingG=512;
        }
        #if defined(INFLIGHT_ACC_CALIBRATION)  
         else if (rcSticks == THR_LO + YAW_LO + PIT_HI + ROL_HI) {    // Inflight ACC calibration START/STOP
            if (AccInflightCalibrationMeasurementDone){                // trigger saving into eeprom after landing
              AccInflightCalibrationMeasurementDone = 0;
              AccInflightCalibrationSavetoEEProm = 1;
            }else{ 
              AccInflightCalibrationArmed = !AccInflightCalibrationArmed; 
              #if defined(BUZZER)
                if (AccInflightCalibrationArmed) SET_ALARM_BUZZER(ALRM_FAC_TOGGLE, ALRM_LVL_TOGGLE_2);
                else     SET_ALARM_BUZZER(ALRM_FAC_TOGGLE, ALRM_LVL_TOGGLE_ELSE);
              #endif
            }
         } 
        #endif
        
       
           if (conf.activate[BOXARM] == 0 && rcSticks == THR_LO + YAW_HI + PIT_CE + ROL_CE) go_arm();      // Arm via YAW
       
        
          else if (rcSticks == THR_HI + YAW_LO + PIT_LO + ROL_CE) calibratingA=512;     // throttle=max, yaw=left, pitch=min
        i=0;
        if      (rcSticks == THR_HI + YAW_CE + PIT_HI + ROL_CE) {conf.angleTrim[PITCH]+=2; i=1;}
        else if (rcSticks == THR_HI + YAW_CE + PIT_LO + ROL_CE) {conf.angleTrim[PITCH]-=2; i=1;}
        else if (rcSticks == THR_HI + YAW_CE + PIT_CE + ROL_HI) {conf.angleTrim[ROLL] +=2; i=1;}
        else if (rcSticks == THR_HI + YAW_CE + PIT_CE + ROL_LO) {conf.angleTrim[ROLL] -=2; i=1;}
        if (i) {
          //writeParams(1);
          rcDelayCommand = 0;    // allow autorepetition
         
        }
      }
    }


    
    #if defined(INFLIGHT_ACC_CALIBRATION)
      if (AccInflightCalibrationArmed && f.ARMED && rcData[THROTTLE] > MINCHECK && !rcOptions[BOXARM] ){ // Copter is airborne and you are turning it off via boxarm : start measurement
        InflightcalibratingA = 50;
        AccInflightCalibrationArmed = 0;
      }  
      if (rcOptions[BOXCALIB]) {      // Use the Calib Option to activate : Calib = TRUE Meausrement started, Land and Calib = 0 measurement stored
        if (!AccInflightCalibrationActive && !AccInflightCalibrationMeasurementDone){
          InflightcalibratingA = 50;
        }
      }else if(AccInflightCalibrationMeasurementDone && !f.ARMED){
        AccInflightCalibrationMeasurementDone = 0;
        AccInflightCalibrationSavetoEEProm = 1;
      }
    #endif

    uint16_t auxState = 0;
    for(i=0;i<4;i++)
      auxState |= (rcData[AUX1+i]<1300)<<(3*i) | (1300<rcData[AUX1+i] && rcData[AUX1+i]<1700)<<(3*i+1) | (rcData[AUX1+i]>1700)<<(3*i+2);

    for(i=0;i<CHECKBOXITEMS;i++)
      rcOptions[i] = (auxState & conf.activate[i])>0;

    // note: if FAILSAFE is disable, failsafeCnt > 5*FAILSAFE_DELAY is always false

      if ( rcOptions[BOXANGLE] || (failsafeCnt > 5*FAILSAFE_DELAY) ) { 
        // bumpless transfer to Level mode
        if (!f.ANGLE_MODE) {
          errorAngleI[ROLL] = 0; errorAngleI[PITCH] = 0;
          f.ANGLE_MODE = 1;
        }  
      } else {
        if(f.ANGLE_MODE){
          errorGyroI[ROLL] = 0; errorGyroI[PITCH] = 0;
        }
        f.ANGLE_MODE = 0;
      }
      if ( rcOptions[BOXHORIZON] ) {
        f.ANGLE_MODE = 0;
        if (!f.HORIZON_MODE) {
          errorAngleI[ROLL] = 0; errorAngleI[PITCH] = 0;
          f.HORIZON_MODE = 1;
        }
      } else {
        if(f.HORIZON_MODE){
          errorGyroI[ROLL] = 0;errorGyroI[PITCH] = 0;
        }
        f.HORIZON_MODE = 0;
      }

    if (rcOptions[BOXARM] == 0) f.OK_TO_ARM = 1;


    


    if (rcOptions[BOXMAG]) {
      if (!f.MAG_MODE) {
        f.MAG_MODE = 1;
        magHold = att.heading;
      }
    } else {
      f.MAG_MODE = 0;
    }
    

   
 
  } 

 
  while(1) {
    currentTime = micros();
    cycleTime = currentTime - previousTime;
    #if defined(LOOP_TIME)
      if (cycleTime >= LOOP_TIME) break;
    #else
      break;  
    #endif
  }
  previousTime = currentTime;

  //computeIMU();


  //*********************************** 
  // THROTTLE sticks during mission and RTH


  if (abs(rcCommand[YAW]) <70 && f.MAG_MODE) {
    int16_t dif = att.heading - magHold;
    if (dif <= - 180) dif += 360;
    if (dif >= + 180) dif -= 360;
    if (f.SMALL_ANGLES_25 || (f.GPS_mode != 0)) rcCommand[YAW] -= dif*conf.pid[PIDMAG].P8 >> 5;  //Always correct maghold in GPS mode
  } else magHold = att.heading;





  //**** PITCH & ROLL & YAW PID ****

  if ( f.HORIZON_MODE ) prop = min(max(abs(rcCommand[PITCH]),abs(rcCommand[ROLL])),512);

  // PITCH & ROLL
  for(axis=0;axis<2;axis++) {
    rc = rcCommand[axis]<<1;
    error = rc - imu.gyroData[axis];
    errorGyroI[axis]  = constrain(errorGyroI[axis]+error,-16000,+16000);       // WindUp   16 bits is ok here
    if (abs(imu.gyroData[axis])>640) errorGyroI[axis] = 0;

    ITerm = (errorGyroI[axis]>>7)*conf.pid[axis].I8>>6;                        // 16 bits is ok here 16000/125 = 128 ; 128*250 = 32000

    PTerm = mul(rc,conf.pid[axis].P8)>>6;
    
    if (f.ANGLE_MODE || f.HORIZON_MODE) { // axis relying on ACC
      // 50 degrees max inclination
      errorAngle         = constrain(rc + GPS_angle[axis],-500,+500) - att.angle[axis] + conf.angleTrim[axis]; //16 bits is ok here
      errorAngleI[axis]  = constrain(errorAngleI[axis]+errorAngle,-10000,+10000);                                                // WindUp     //16 bits is ok here

      PTermACC           = mul(errorAngle,conf.pid[PIDLEVEL].P8)>>7; // 32 bits is needed for calculation: errorAngle*P8 could exceed 32768   16 bits is ok for result

      int16_t limit      = conf.pid[PIDLEVEL].D8*5;
      PTermACC           = constrain(PTermACC,-limit,+limit);

      ITermACC           = mul(errorAngleI[axis],conf.pid[PIDLEVEL].I8)>>12;   // 32 bits is needed for calculation:10000*I8 could exceed 32768   16 bits is ok for result

      ITerm              = ITermACC + ((ITerm-ITermACC)*prop>>9);
      PTerm              = PTermACC + ((PTerm-PTermACC)*prop>>9);
    }

    PTerm -= mul(imu.gyroData[axis],dynP8[axis])>>6; // 32 bits is needed for calculation   

    delta          = imu.gyroData[axis] - lastGyro[axis];  // 16 bits is ok here, the dif between 2 consecutive gyro reads is limited to 800
    lastGyro[axis] = imu.gyroData[axis];
    DTerm          = delta1[axis]+delta2[axis]+delta;
    delta2[axis]   = delta1[axis];
    delta1[axis]   = delta;
 
    DTerm = mul(DTerm,dynD8[axis])>>5;        // 32 bits is needed for calculation

    axisPID[axis] =  PTerm + ITerm - DTerm;
  }

  //YAW
  #define GYRO_P_MAX 300
  #define GYRO_I_MAX 250

  rc = mul(rcCommand[YAW] , (2*conf.yawRate + 30))  >> 5;

  error = rc - imu.gyroData[YAW];
  errorGyroI_YAW  += mul(error,conf.pid[YAW].I8);
  errorGyroI_YAW  = constrain(errorGyroI_YAW, 2-((int32_t)1<<28), -2+((int32_t)1<<28));
  if (abs(rc) > 50) errorGyroI_YAW = 0;
  
  PTerm = mul(error,conf.pid[YAW].P8)>>6;
  #ifndef COPTER_WITH_SERVO
    int16_t limit = GYRO_P_MAX-conf.pid[YAW].D8;
    PTerm = constrain(PTerm,-limit,+limit);
  #endif
  
  ITerm = constrain((int16_t)(errorGyroI_YAW>>13),-GYRO_I_MAX,+GYRO_I_MAX);
  
  axisPID[YAW] =  PTerm + ITerm;
  
  mixTable();
  //if ( (f.ARMED) || ((!calibratingG) && (!calibratingA)) ) writeServos();
  writeMotors();


}



int main() {
    
    ANSELB &= ~(1<<13 | 1<<7);
    ANSELC &= ~(1<<6);   
    ANSELC &= ~(1<<7);   
    ANSELC &= ~(1<<8);   
    ANSELC &= ~(1<<9); 
    
    TRISA = 0;
    TRISB = 1<<13;
    TRISC = 0;
    LATA = 0;
    LATB = 0;
    LATC = 0;
    
    
    InicializarReloj();
    InicializarTemp();  
    setup();
    
    INTCONbits.MVEC = 1; // Se pone en modo multivector
    asm(" ei"); // y se habilitan las interrupciones
    
    //sendString("hola\0");
    while(1) {
        loop();
    }
    
    return 0;
}




