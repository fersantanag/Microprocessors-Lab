#include "GyroAcc.h"
#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>


static int16_t ac_x_raw = 0;
static int16_t ac_y_raw = 0; 
static int16_t ac_z_raw = 0; 
static int16_t gy_x_raw; 
static int16_t gy_y_raw; 
static int16_t gy_z_raw;

static double ac_x;
static double ac_y;
static double ac_z;
static double gy_x;
static double gy_y;
static double gy_z;

static double ac_angle_x;
static double ac_angle_y;

static double gy_angle_x = 0;
static double gy_angle_y = 0;
static double gy_angle_z = 0;

static double ac_speed_x = 0;
static double ac_speed_y = 0;
static double ac_speed_z = 0;

static double angle_x = 0;
static double angle_y = 0;

long previous_time = 0;


MPU6050 mpu;


bool initGyroAcc() {

    Wire.begin();
    mpu.initialize();

    // Configuramos +- 2000 grados / s y +-16g

    if(!mpu.testConnection())
        return false;

    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);    // +- 16g
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);    // 2000 grados / s

    mpu.CalibrateAccel();
    mpu.CalibrateGyro(); 
       

    return true;

}

void computeIMU() {

    // Obtain new values
    mpu.getMotion6(&ac_x_raw, &ac_y_raw, &ac_z_raw, &gy_x_raw, &gy_y_raw, &gy_z_raw);

    // Change to readable units
    ac_x = ac_x_raw/16384.0*9.81;
    ac_y = ac_y_raw/16384.0*9.81;
    ac_z = ac_z_raw/16384.0*9.81;
    gy_x = gy_x_raw/16.38;
    gy_y = gy_y_raw/16.38;
    gy_z = gy_z_raw/16.38;


    double dt = (micros()-previous_time)/1000000.0;
    previous_time = micros();

    // Speed & Angle
    gy_angle_x = gy_angle_x + dt*gy_x;
    gy_angle_y = gy_angle_y + dt*gy_y;
    gy_angle_z = gy_angle_z + dt*gy_z;
    ac_speed_x = ac_speed_x + dt*ac_x;
    ac_speed_y = ac_speed_y + dt*ac_y;
    ac_speed_z = ac_speed_z + dt*ac_z;


    // Calculate angles
    ac_angle_y = atan(-ac_x/sqrt(pow(ac_y, 2)+pow(ac_z, 2)))*57.295779;
    ac_angle_x = atan(ac_y/sqrt(pow(ac_x, 2)+pow(ac_z, 2)))*57.295779;
    
    angle_x = 0.98*(angle_x+gy_x*dt) + 0.02*ac_angle_x;
    angle_y = 0.98*(angle_y+gy_y*dt) + 0.02*ac_angle_y;


}

void getAcc(double* ac_vec) {
    ac_vec[0] = ac_x;
    ac_vec[1] = ac_y;
    ac_vec[2] = ac_z;
}

void getGyro(double* gy_vec) {
    gy_vec[0] = gy_x;
    gy_vec[1] = gy_y;
    gy_vec[2] = gy_z;
}

void getAngle(double* angle) {
    angle[0] = angle_x;
    angle[1] = angle_y;
}

void getSpeed(double* speed) {
    speed[0] = ac_speed_x;
    speed[1] = ac_speed_y;
    speed[2] = ac_speed_z;
}

void getGyroAngle(double* angle) {
    angle[0] = gy_angle_x;
    angle[1] = gy_angle_y;
    angle[2] = gy_angle_z;
}
