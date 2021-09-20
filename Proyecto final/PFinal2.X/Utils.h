/* 
 * File:   Utils.h
 * Author: jorge
 *
 * Created on 06 May 2021, 12:23
 */

#ifndef UTILS_H
#define	UTILS_H


//#define min(a,b) ((a)<(b)?(a):(b))
//#define max(a,b) ((a)>(b)?(a):(b))
//#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define PI 3.1415926535897932384626433832795


char digitToChar(int i);
char* toCharArray(int number, int* len_array);
void addString(char* a, char* b, int* current_i, int len_b);


#endif	/* UTILS_H */

