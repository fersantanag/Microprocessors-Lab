#include <Arduino.h>
#include "Receiver.h"
#include "GyroAcc.h"
#include "Output.h"

#define LOOP_CYCLE 6000

static int current_time;
static int16_t rc[4];   // throttle [0, 1000], roll, pitch, yaw [-500, 500]

double acc[3];
double gy[3];
double angle[3];
double speed[3];

void setup() {
  
  // Initialize
  initReceiver(rc);  
  initGyroAcc();
  Serial.begin(9600);
  
}

void loop() {
   
  current_time = micros();
  
  computeRX();
  computeIMU();


  if (Serial.available()>0) {

    char c = Serial.read();

    if (c == '1') {

      getAcc(acc);
      getGyro(gy);
      
      Serial.print("Acc X: ");
      Serial.println(acc[0]);
      Serial.print("Acc Y: ");
      Serial.println(acc[1]);
      Serial.print("Acc Z: ");
      Serial.println(acc[2]);
      Serial.print("Gy X: ");
      Serial.println(gy[0]);
      Serial.print("Gy Y: ");
      Serial.println(gy[1]);
      Serial.print("Gy Z: ");
      Serial.println(gy[2]);
      
    } else if (c == '2') {

      getGyroAngle(angle);

      Serial.print("Angle X: ");
      Serial.println(angle[0]);
      Serial.print("Angle Y: ");
      Serial.println(angle[1]);
      Serial.print("Angle Z: ");
      Serial.println(angle[2]);


    } else if (c == '3') {

      getSpeed(speed);

      Serial.print("Speed X: ");
      Serial.println(speed[0]);
      Serial.print("Speed Y: ");
      Serial.println(speed[1]);
      Serial.print("Speed Z: ");
      Serial.println(speed[2]);

    }
    

  } 

  while (micros()-current_time < 10)
    ;

}