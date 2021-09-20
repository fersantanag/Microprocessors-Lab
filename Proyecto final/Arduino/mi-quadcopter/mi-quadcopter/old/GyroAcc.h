

#ifndef GYROACC_H
#define GYROACC_H

bool initGyroAcc();
void computeIMU();

void getAcc(double* ac_vec);
void getGyro(double* gy_vec);
void getAngle(double* angle);

void getGyroAngle(double* angle);
void getSpeed(double* speed);

#endif /* GYROACC_H */

