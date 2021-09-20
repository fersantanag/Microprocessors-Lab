#include <xc.h>
#include "Utils.h"
#include "I2C.h"
#include "main.h"
#include "Sensors.h"


//#define GYRO_ORIENTATION(X, Y, Z) {imu.gyroADC[ROLL] =  Y; imu.gyroADC[PITCH] = -X; imu.gyroADC[YAW] = -Z;}
#define GYRO_ORIENTATION(X, Y, Z) {gyro[ROLL] =  -X; gyro[PITCH] = -Y ; gyro[YAW] = -Z;}

#define GYRO_DLPF_CFG   0 //Default settings LPF 256Hz/8000Hz sample



static uint8_t rawADC[6];
static int32_t gyroZero[3];

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
      g[axis] +=gyro[axis]; // Sum up 512 readings
      gyroZero[axis]=g[axis]>>9;
      if (calibratingG == 1) {
        ; // Avisar fin de la calibración
      }
    }
    calibratingG--;
  }
  
  
  for (axis = 0; axis < 3; axis++) {
    gyro[axis]  -= gyroZero[axis];
    //anti gyro glitch, limit the variation between two consecutive readings
    gyro[axis] = constrain(gyro[axis],previousGyroADC[axis]-800,previousGyroADC[axis]+800);
    previousGyroADC[axis] = gyro[axis];
  }

}





// ************************************************************************************************************
// I2C Gyroscope and Accelerometer MPU6050
// ************************************************************************************************************

#define MPU6050_ADDRESS     0x68 // address pin AD0 low (GND), default for FreeIMU v0.4 and InvenSense evaluation board
//#define MPU6050_ADDRESS     0x69 // address pin AD0 high (VCC)

static void Gyro_init() {
  i2c_writeReg(MPU6050_ADDRESS, 0x6B, 0x80);             //PWR_MGMT_1    -- DEVICE_RESET 1
  //Retardo(50);
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





void initS() {
  i2c_init();
  Gyro_init();
}

void initSensors() {
  /*uint8_t c = 5;
  while(c) { // We try several times to init all sensors without any i2c errors. An I2C error at this stage might results in a wrong sensor settings
    c--;
    initS();
    if (i2c_errors_count == 0) break; // no error during init => init ok
  }*/
    initS();
}