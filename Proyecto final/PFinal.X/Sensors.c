#include <xc.h>
#include "Utils.h"
#include "types.h"
#include "I2C.h"
#include "Algorithm.h"
#include "Sensors.h"
#include "ultrasonidos.h"


#define ACC_ORIENTATION(X, Y, Z)  {imu.accADC[ROLL]  = -X; imu.accADC[PITCH]  = -Y; imu.accADC[YAW]  =  Z;}
//#define GYRO_ORIENTATION(X, Y, Z) {imu.gyroADC[ROLL] =  Y; imu.gyroADC[PITCH] = -X; imu.gyroADC[YAW] = -Z;}
#define GYRO_ORIENTATION(X, Y, Z) {imu.gyroADC[ROLL] =  -X; imu.gyroADC[PITCH] = -Y ; imu.gyroADC[YAW] = -Z;}

#define GYRO_DLPF_CFG   0 //Default settings LPF 256Hz/8000Hz sample


static uint8_t rawADC[6];

/* I2C Function */

void i2c_getSixRawADC(uint8_t add, uint8_t reg) {
  i2c_read_reg_to_buf(add, reg, rawADC, 6);
}


// ****************
// GYRO common part
// ****************
void GYRO_Common() {
  static int16_t previousGyroADC[3] = {0,0,0};
  static int32_t g[3];
  uint8_t axis;


  if (calibratingG>0) {
    for (axis = 0; axis < 3; axis++) {
      if (calibratingG == 512) { // Reset g[axis] at start of calibration
        g[axis]=0;

      }
      g[axis] +=imu.gyroADC[axis]; // Sum up 512 readings
      gyroZero[axis]=g[axis]>>9;
      if (calibratingG == 1) {
        ; // Avisar fin de la calibración
      }
    }
    calibratingG--;
  }
  
  
  for (axis = 0; axis < 3; axis++) {
    imu.gyroADC[axis]  -= gyroZero[axis];
    //anti gyro glitch, limit the variation between two consecutive readings
    imu.gyroADC[axis] = constrain(imu.gyroADC[axis],previousGyroADC[axis]-800,previousGyroADC[axis]+800);
    previousGyroADC[axis] = imu.gyroADC[axis];
  }

}

// ****************
// ACC common part
// ****************
void ACC_Common() {
  static int32_t a[3];
  if (calibratingA>0) {
    calibratingA--;
    uint8_t axis;
    for (axis = 0; axis < 3; axis++) {
      if (calibratingA == 511) a[axis]=0;   // Reset a[axis] at start of calibration
      a[axis] +=imu.accADC[axis];           // Sum up 512 readings
      global_conf.accZero[axis] = a[axis]>>9; // Calculate average, only the last itteration where (calibratingA == 0) is relevant
    }
    if (calibratingA == 0) {
      global_conf.accZero[YAW] -= ACC_1G;   // shift Z down by ACC_1G and store values in EEPROM at end of calibration
      conf.angleTrim[ROLL]   = 0;
      conf.angleTrim[PITCH]  = 0;
      // writeGlobalSet(1); // write accZero in EEPROM
    }
  }

  imu.accADC[ROLL]  -=  global_conf.accZero[ROLL] ;
  imu.accADC[PITCH] -=  global_conf.accZero[PITCH];
  imu.accADC[YAW]   -=  global_conf.accZero[YAW] ;

}


// ************************************************************************************************************
// I2C Gyroscope and Accelerometer MPU6050
// ************************************************************************************************************

#define MPU6050_ADDRESS     0x68 // address pin AD0 low (GND), default for FreeIMU v0.4 and InvenSense evaluation board
//#define MPU6050_ADDRESS     0x69 // address pin AD0 high (VCC)

static void Gyro_init() {
  i2c_writeReg(MPU6050_ADDRESS, 0x6B, 0x80);             //PWR_MGMT_1    -- DEVICE_RESET 1
  Retardo(50);
  i2c_writeReg(MPU6050_ADDRESS, 0x6B, 0x03);             //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
  i2c_writeReg(MPU6050_ADDRESS, 0x1A, GYRO_DLPF_CFG);    //CONFIG        -- EXT_SYNC_SET 0 (disable input pin for data sync) ; default DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
  i2c_writeReg(MPU6050_ADDRESS, 0x1B, 0x18);             //GYRO_CONFIG   -- FS_SEL = 3: Full scale set to 2000 deg/sec
  // enable I2C bypass for AUX I2C
 
}

void Gyro_getADC () {
  i2c_getSixRawADC(MPU6050_ADDRESS, 0x43);
  GYRO_ORIENTATION( ((rawADC[0]<<8) | rawADC[1])>>2 , // range: +/- 8192; +/- 2000 deg/sec
                    ((rawADC[2]<<8) | rawADC[3])>>2 ,
                    ((rawADC[4]<<8) | rawADC[5])>>2 );
  GYRO_Common();
}

static void ACC_init () {
  i2c_writeReg(MPU6050_ADDRESS, 0x1C, 0x10);             //ACCEL_CONFIG  -- AFS_SEL=2 (Full Scale = +/-8G)  ; ACCELL_HPF=0   //note something is wrong in the spec.
  //note: something seems to be wrong in the spec here. With AFS=2 1G = 4096 but according to my measurement: 1G=2048 (and 2048/8 = 256)
  //confirmed here: http://www.multiwii.com/forum/viewtopic.php?f=8&t=1080&start=10#p7480

}

void ACC_getADC () {
  i2c_getSixRawADC(MPU6050_ADDRESS, 0x3B);
  ACC_ORIENTATION( ((rawADC[0]<<8) | rawADC[1])>>3 ,
                   ((rawADC[2]<<8) | rawADC[3])>>3 ,
                   ((rawADC[4]<<8) | rawADC[5])>>3 );
  ACC_Common();
}



void initS() {
  i2c_init();
  Gyro_init();
  ACC_init();
}

void initSensors() {
  uint8_t c = 5;
  while(c) { // We try several times to init all sensors without any i2c errors. An I2C error at this stage might results in a wrong sensor settings
    c--;
    initS();
    if (i2c_errors_count == 0) break; // no error during init => init ok
  }
}