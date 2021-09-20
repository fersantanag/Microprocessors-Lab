
#ifndef TYPES_H
#define TYPES_H

enum rc {
  ROLL,
  PITCH,
  YAW,
  THROTTLE,
  AUX1,
  AUX2,
  AUX3,
  AUX4,
  AUX5,
  AUX6,
  AUX7,
  AUX8
};

enum pid {
  PIDROLL,
  PIDPITCH,
  PIDYAW,
  PIDALT,
  PIDPOS,
  PIDPOSR,
  PIDNAVR,
  PIDLEVEL,
  PIDMAG,
  PIDVEL,     // not used currently
  PIDITEMS
};

enum box {
  BOXARM,
  BOXANGLE,
  BOXHORIZON,
  BOXMAG,
  CHECKBOXITEMS
};

struct pid_ {
  uint8_t P8;
  uint8_t I8;
  uint8_t D8;
};

struct servo_conf_ {  // this is a generic way to configure a servo, every multi type with a servo should use it
  int16_t min;        // minimum value, must be more than 1020 with the current implementation
  int16_t max;        // maximum value, must be less than 2000 with the current implementation
  int16_t middle;     // default should be 1500
  int8_t  rate;       // range [-100;+100] ; can be used to ajust a rate 0-100% and a direction
};

typedef struct {
  int16_t  accSmooth[3];
  int16_t  gyroData[3];
  int16_t  magADC[3];
  int16_t  gyroADC[3];
  int16_t  accADC[3];
} imu_t;

typedef struct {
  int32_t  EstAlt;             // in cm
  int16_t  vario;              // variometer in cm/s
} alt_t;

typedef struct {
  int16_t angle[2];            // absolute angle inclination in multiple of 0.1 degree    180 deg = 1800
  int16_t heading;             // variometer in cm/s
} att_t;

typedef struct {
  uint8_t OK_TO_ARM :1 ;
  uint8_t ARMED :1 ;
  uint8_t ACC_CALIBRATED :1 ;
  uint8_t ANGLE_MODE :1 ;
  uint8_t HORIZON_MODE :1 ;
  uint8_t MAG_MODE :1 ;
  uint8_t BARO_MODE :1 ;

  uint8_t SMALL_ANGLES_25 :1 ;

  uint8_t GPS_mode: 2;               // 0-3 NONE,HOLD, HOME, NAV (see GPS_MODE_* defines

} flags_struct_t;

typedef struct {
  uint8_t currentSet;
  int16_t accZero[3];
  int16_t magZero[3];
  uint16_t flashsum;
  uint8_t checksum;      // MUST BE ON LAST POSITION OF STRUCTURE !
} global_conf_t;

typedef struct {
  pid_    pid[PIDITEMS];
  uint8_t rcRate8;
  uint8_t rcExpo8;
  uint8_t rollPitchRate;
  uint8_t yawRate;
  uint8_t dynThrPID;
  uint8_t thrMid8;
  uint8_t thrExpo8;
  int16_t angleTrim[2];
  uint16_t activate[CHECKBOXITEMS];
  uint8_t powerTrigger1;
  int16_t minthrottle;
  uint8_t yawCollPrecomp;
  uint16_t yawCollPrecompDeadband;
  uint8_t  checksum;      // MUST BE ON LAST POSITION OF CONF STRUCTURE !
} conf_t;


#endif