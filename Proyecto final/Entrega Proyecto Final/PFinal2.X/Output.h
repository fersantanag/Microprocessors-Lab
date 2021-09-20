

#ifndef OUTPUT_H
#define	OUTPUT_H

#define PIN_F_L 6   // RC6 -> OC4 -> motor[3] -> naranja
#define PIN_F_R 8   // RC8 -> OC2 -> motor[1] -> amarillo
#define PIN_B_L 9   // RC9 -> OC3 -> motor[2] -> verde
#define PIN_B_R 7   // RC7 -> OC1 -> motor[0] -> verde dorado

enum order_motors {
    B_R,
    F_R,
    B_L,
    F_L
};

void initOutput();
void writeMotors();
void computeMotors();


#endif	/* OUTPUT_H */

