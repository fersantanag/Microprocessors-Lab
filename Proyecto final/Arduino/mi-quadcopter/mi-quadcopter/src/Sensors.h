
#ifndef SENSORS_H
#define SENSORS_H


void ACC_getADC ();
void Gyro_getADC ();


void initSensors();

#define ACC_1G 512

#define GYRO_SCALE (4 / 16.4 * PI / 180.0 / 1000000.0)      // Why is there a 4?

#define ACCZ_25deg   (int16_t)(ACC_1G * 0.90631) // 0.90631 = cos(25deg) (cos(theta) of accZ comparison)
#define ACC_VelScale (9.80665f / 10000.0f / ACC_1G)




#endif /* SENSORS_H */