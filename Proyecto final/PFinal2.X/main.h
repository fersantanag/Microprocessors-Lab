
#ifndef MAIN_H
#define	MAIN_H

#define LEDS_AZUL 2
#define LEDS_ROJO 3

#define PIN_RX 13   //RB13
#define PIN_TX 7    //RB7

#define PIN_RX_2 8  // RB8
#define PIN_TX_2 9  // RB9

enum rc {
    ROLL,
    PITCH,
    YAW,
    THROTTLE
};

extern int armed;
extern int police_mode;
extern int16_t motor[4];
extern int16_t rcData[4];
extern int32_t gyro[3];
extern int16_t calibratingG;


void go_arm();
void go_disarm();


#endif	/* MAIN_H */

