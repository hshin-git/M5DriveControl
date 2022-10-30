#ifndef _DRIVER_H_
#define _DRIVER_H_
////////////////////////////////////////////////////////////////////////////////
// class Driver{}: ドライバ用クラス
//  setup(): 初期化処理
//  loop(): ループ処理
////////////////////////////////////////////////////////////////////////////////
#include "src/WebConfig.hpp"


class Driver {
public:
  static float GYRO[3]; /* Angular velocity (deg/sec) */
  static float ACCL[3]; /* Acceleration (G) */
  static float AHRS[3]; /* Attitude angle (deg) */
  static float TEMP;  /* Temperature (degC) */
  static int IN[4]; /* Input RC signal (usec) */
  static int FREQ[4]; /* Frequency of loop,conf,sbus,port (Hz) */
  int OUT[4]; /* Output RC signal (usec) */
  //
  virtual void setup() = 0;
  virtual void loop() = 0;
  virtual void debug() { 
    for (int i=0; i<2; i++) { 
      DEBUG.print("ch");DEBUG.print(i);DEBUG.print(":");
      DEBUG.print(IN[i]);DEBUG.print("->");DEBUG.println(OUT[i]);
    }; 
  };
};

float Driver::GYRO[3];
float Driver::ACCL[3];
float Driver::AHRS[3];
float Driver::TEMP;
int Driver::IN[4];
int Driver::FREQ[4];


// AJAX monitor
ParameterList ajaxConfig = {
  .body = {0},
  .conf = {
    PCONF_IREFTO("ch1",&Driver::IN[0],"usec",900,2100),
    PCONF_IREFTO("ch2",&Driver::IN[1],"usec",900,2100),
    PCONF_IREFTO("ch3",&Driver::IN[2],"usec",900,2100),
    PCONF_IREFTO("ch4",&Driver::IN[3],"usec",900,2100),
    PCONF_FREFTO("imu_pitch",&Driver::AHRS[0],"deg",-90,90),
    PCONF_FREFTO("imu_roll",&Driver::AHRS[1],"deg",-90,90),
    PCONF_FREFTO("imu_rate",&Driver::GYRO[2],"deg/sec",-360,360),
    PCONF_FREFTO("imu_temp",&Driver::TEMP,"degC",0,100),
    PCONF_IREFTO("freq_loop",&Driver::FREQ[0],"Hz",0,500),
    PCONF_IREFTO("freq_conf",&Driver::FREQ[1],"Hz",0,500),
    PCONF_IREFTO("freq_sbus",&Driver::FREQ[2],"Hz",0,500),
    PCONF_IREFTO("freq_port",&Driver::FREQ[3],"Hz",0,500),
    PCONF_END,
  },
};


// CH1 parameter
ParameterList ch1Config = {
  .conf = {
    PCONF_FETCH("ch1_min",1000,"usec","ajax",0,800,1200),
    PCONF_FETCH("ch1_mid",1500,"usec","ajax",0,1300,1700),
    PCONF_FETCH("ch1_max",2000,"usec","ajax",0,1800,2200),
    PCONF_RANGE("ch1_dead",5,"usec",0,50,1),
    PCONF_RANGE("ch1_freq",50,"Hz",50,400,50),
    PCONF_RANGE("ch1_lpf",1,"span",1,50,1),
    PCONF_END,
  },
};
#define CH1_MIN (ch1Config.getIval(0))
#define CH1_MID (ch1Config.getIval(1))
#define CH1_MAX (ch1Config.getIval(2))
#define CH1_DEAD (ch1Config.getIval(3))
#define CH1_FREQ (ch1Config.getIval(4))
#define CH1_SPAN (ch1Config.getIval(5))


// CH2 parameter
ParameterList ch2Config = {
  .conf = {
    PCONF_FETCH("ch2_min",1000,"usec","ajax",1, 800,1200),
    PCONF_FETCH("ch2_mid",1500,"usec","ajax",1,1300,1500),
    PCONF_FETCH("ch2_max",2000,"usec","ajax",1,1800,2000),
    PCONF_RANGE("ch2_dead",5,"usec",0,50,1),
    PCONF_RANGE("ch2_freq",50,"Hz",50,400,50),
    PCONF_RANGE("ch2_lpf",1,"span",1,50,1),
    PCONF_END,
  },
};
#define CH2_MIN (ch2Config.getIval(0))
#define CH2_MID (ch2Config.getIval(1))
#define CH2_MAX (ch2Config.getIval(2))
#define CH2_DEAD (ch2Config.getIval(3))
#define CH2_FREQ (ch2Config.getIval(4))
#define CH2_SPAN (ch2Config.getIval(5))



////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif
