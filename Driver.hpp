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
  .body = {0},
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
  .body = {0},
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
// ESC table
////////////////////////////////////////////////////////////////////////////////
// ESC setup
ParameterList escConfig = {
  .body = {0},
  .conf = {
    PCONF_SELECT("esc_mode",1,"mode","[0,1,2]","[\"FB\",\"FR\",\"FBR\"]"),
    PCONF_SELECT("esc_brake",0,"bool","[0,1]","[\"off\",\"on\"]"),
    PCONF_RANGE("esc_drag",50,"%%",0,100,10),
    PCONF_SELECT("esc_invert",0,"bmap","[0,1,2,3]","[\"00\",\"01\",\"10\",\"11\"]"),
    PCONF_SELECT("esc_pwmout",1,"bool","[0,1]","[\"single\",\"double\"]"),
    PCONF_END,
  },
};
//
#define ESC_MODE  (escConfig.getIval(0))
#define ESC_BRAKE (escConfig.getIval(1))
#define ESC_DRAG  (escConfig.getIval(2))
#define ESC_INV (escConfig.getIval(3))
#define ESC_PWM (escConfig.getIval(4))



////////////////////////////////////////////////////////////////////////////////
// System table
////////////////////////////////////////////////////////////////////////////////
// GPIO setup
ParameterList portConfig = {
  .body = {0},
  .conf = {
    PCONF_SELECT("sbus_in",26,"port","[0,26,32,22,19,23,33]","[\"off\",\"g1\",\"g2\",\"b1\",\"b2\",\"b3\",\"b4\"]"),
    PCONF_SELECT("ch1_in",26,"port","[26,32]","[\"g1\",\"g2\"]"),
    PCONF_SELECT("ch2_in",32,"port","[26,32]","[\"g1\",\"g2\"]"),
    PCONF_SELECT("esc1_out",0,"port","[0,22,23]","[\"off\",\"b1_b2\",\"b3_b4\"]"),
    PCONF_SELECT("esc2_out",0,"port","[0,22,19,23,26]","[\"off\",\"b1_b2\",\"b2_b3\",\"b3_b4\",\"g1_g2\"]"),
    PCONF_SELECT("ch1_out",22,"port","[22,19,23,33,26,32]","[\"b1\",\"b2\",\"b3\",\"b4\",\"g1\",\"g2\"]"),
    PCONF_SELECT("ch2_out",19,"port","[22,19,23,33,26,32]","[\"b1\",\"b2\",\"b3\",\"b4\",\"g1\",\"g2\"]"),
    PCONF_END,
  },
};
//
#define SBUS_IN (portConfig.getIval(0))
#define CH1_IN  (portConfig.getIval(1))
#define CH2_IN  (portConfig.getIval(2))
#define ESC1_OUT  (portConfig.getIval(3))
#define ESC2_OUT  (portConfig.getIval(4))
#define CH1_OUT (portConfig.getIval(5))
#define CH2_OUT (portConfig.getIval(6))

// buddy gpio pin for ESC
int ESC_BUDDY(const int p) {
  switch(p) {
    case 22: return 19;
    case 19: return 23;
    case 23: return 33;
    case 26: return 32;
    default: return -1;
  }
  return 0;
}


// global setup
ParameterList globalConfig = {
  .body = {0},
  .conf = {
    PCONF_SELECT("run_mode",0,"mode","[0,1,2,3,4]","[\"direct\",\"gvect\",\"drift\",\"stunt\",\"tank\"]"),
    PCONF_SELECT("imu_axis",1,"axis","[1,2,3,4,5,6]","[\"X+\",\"X-\",\"Y+\",\"Y-\",\"Z+\",\"Z-\"]"),
    PCONF_NUMBER("imu_lpf",1,"span",1,100),
    PCONF_TEXT("wifi_ssid",{0},"text",4,10),
    PCONF_TEXT("wifi_pass",{0},"text",8,10),
    PCONF_COLOR("led_col1",{0},"RGB"),
    PCONF_COLOR("led_col2",{0},"RGB"),
    PCONF_END,
  },
};
//
#define RUN_MODE  (globalConfig.getIval(0))
#define IMU_AXIS  (globalConfig.getIval(1))
#define IMU_SPAN  (globalConfig.getIval(2))
#define WIFI_SSID (globalConfig.getText(3))
#define WIFI_PASS (globalConfig.getText(4))
#define LED_COL1  (globalConfig.getText(5))
#define LED_COL2  (globalConfig.getText(6))
//
#define WIFI_SSID_DEFAULT "m5atom"
#define WIFI_PASS_DEFAULT ""
//
#define LENGTH(a) sizeof(a)/sizeof(a[0])



////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif
