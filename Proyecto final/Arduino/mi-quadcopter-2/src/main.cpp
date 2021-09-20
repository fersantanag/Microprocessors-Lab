#include <Arduino.h>
#include "Receiver.h"
#include "main.h"


static int32_t current_time;
int16_t rcData[4];   // throttle [0, 1000], roll, pitch, yaw [-500, 500]

double acc[3];
double gy[3];
double angle[3];
double speed[3];

void setup() {
  
  // Initialize
  initReceiver();  
  Serial.begin(9600);
  
}

void loop() {
   
  current_time = micros();
  
  computeRX();

  Serial.println(rcData[THROTTLE]);  



}