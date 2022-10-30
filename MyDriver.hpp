#include "Driver.hpp"
#include "src/LibPID.hpp"
#include "src/LibMisc.hpp"
////////////////////////////////////////////////////////////////////////////////
// Define My Drivers
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Thru driver
////////////////////////////////////////////////////////////////////////////////
class thruDriver: public Driver {
public:
  void setup() { };
  void loop() { 
    for (int i=0; i<4; i++) OUT[i] = IN[i];
  };
} thruDriver;



////////////////////////////////////////////////////////////////////////////////
// G-Vectoring mode
////////////////////////////////////////////////////////////////////////////////
ParameterList gvectConfig = {
  .conf = {
    PCONF_RANGE("gain",50,"%%",0,100,1),
    PCONF_RANGE("hpf",1,"span",1,100,1),
    PCONF_RANGE("lpf",1,"span",1,100,1),
    PCONF_END,
  },
};
#define GVECT_GAIN  (gvectConfig.getIval(0)/0.05)
#define GVECT_HPF (gvectConfig.getIval(1))
#define GVECT_LPF (gvectConfig.getIval(2))


class gvectDriver: public Driver {
  int MIN = 1000;
  int MAX = 2000;
  FilterHP HPF;
  FilterLP LPF;
public:
  void setup() {
    MIN = min(CH1_MIN,CH1_MAX);
    MAX = max(CH1_MIN,CH1_MAX);
    HPF.setup(GVECT_HPF,0.0);
    LPF.setup(GVECT_LPF,0.0);
  };
  void loop() {
    //
    float Gy = ACCL[0];
    float dotGy = HPF.update(Gy);
    float Gxc = -GVECT_GAIN*LPF.update(dotGy);
    if (Gy*dotGy < 0.0) Gxc = -Gxc;
    Gxc = constrain(Gxc, -1.0,1.0);
    // ch1
    OUT[0] = constrain(IN[0],MIN,MAX);
    // ch2
    if (abs(IN[1] - CH2_MID) < CH2_DEAD) {
      OUT[1] = CH2_MID;
    } 
    else if (IN[1] > CH2_MID) {
      OUT[1] = CH2_MID + (IN[1] - CH2_MID)*(1.0 + Gxc);
      OUT[1] = constrain(OUT[1], CH2_MIN,CH2_MAX);
    } else
      OUT[1] = IN[1];
  };
} gvectDriver;



////////////////////////////////////////////////////////////////////////////////
// Drift mode
////////////////////////////////////////////////////////////////////////////////
ParameterList driftConfig = {
  .conf = {
    PCONF_RANGE("rate",50,"deg/sec",10,720,10),
    PCONF_NUMBER("P",50,"%%",0,100),
    PCONF_NUMBER("I",0,"%%",0,100),
    PCONF_NUMBER("D",0,"%%",0,100),
    PCONF_RANGE("rev",0,"bool",0,1,1),
    PCONF_END,
  },
};
#define DRIFT_G (500./driftConfig.getIval(0))
#define DRIFT_P (driftConfig.getIval(1)/50.)
#define DRIFT_I (driftConfig.getIval(2)/250.)
#define DRIFT_D (driftConfig.getIval(3)/5000.)
#define DRIFT_REV (driftConfig.getIval(4))


class driftDriver: public Driver {
  int MIN = 1000;
  int MAX = 2000;
  LibPID CH1_PID;
public:
  void setup() {
    MIN = min(CH1_MIN,CH1_MAX);
    MAX = max(CH1_MIN,CH1_MAX);
    CH1_PID.setup(DRIFT_P,DRIFT_I,DRIFT_D,MIN,CH1_MID,MAX,400);
  };
  void loop() {
    float YAW_RATE = GYRO[2];
    OUT[0] = CH1_PID.loop(IN[0], DRIFT_G*(DRIFT_REV? -YAW_RATE: YAW_RATE));
    OUT[1] = IN[1];
    //
    OUT[0] = constrain(OUT[0],MIN,MAX);
  };
} driftDriver;



////////////////////////////////////////////////////////////////////////////////
// Stunt mode
////////////////////////////////////////////////////////////////////////////////
ParameterList stuntConfig = {
  .conf = {
    PCONF_FETCH("roll",0,"deg","ajax",5,30,60),
    PCONF_NUMBER("G",50,"%%",0,100),
    PCONF_NUMBER("P",50,"%%",0,100),
    PCONF_NUMBER("I",0,"%%",0,100),
    PCONF_NUMBER("D",0,"%%",0,100),
    PCONF_RANGE("rev",0,"bool",0,1,1),
    PCONF_NUMBER("ch1",0,"%%",0,100),
    PCONF_END,
  },
};
#define STUNT_ROLL  (stuntConfig.getIval(0))
#define STUNT_G (stuntConfig.getIval(1)/50.*(500./20.))
#define STUNT_P (stuntConfig.getIval(2)/50.)
#define STUNT_I (stuntConfig.getIval(3)/500.)
#define STUNT_D (stuntConfig.getIval(4)/500.)
#define STUNT_REV (stuntConfig.getIval(5))
#define STUNT_CH1 (stuntConfig.getIval(6)/50.)


class stuntDriver: public Driver {
  int MIN = 1000;
  int MAX = 2000;
  LibPID CH1_PID;
public:
  void setup() {
    MIN = min(CH1_MIN,CH1_MAX);
    MAX = max(CH1_MIN,CH1_MAX);
    CH1_PID.setup(STUNT_P,STUNT_I,STUNT_D,MIN,CH1_MID,MAX,400);
  };
  void loop() {
    float IMU_ROLL = AHRS[1];
    float ABS_ROLL = abs(STUNT_ROLL);
    if (abs(IMU_ROLL) > 30) {
      float DEL_ROLL = (IMU_ROLL>0? IMU_ROLL-ABS_ROLL: IMU_ROLL+ABS_ROLL);
      float CH1 = CH1_MID + STUNT_CH1*(IN[0] - CH1_MID);
      OUT[0] = CH1_PID.loop(CH1, STUNT_G*(STUNT_REV? -DEL_ROLL: DEL_ROLL));
      OUT[1] = IN[1];
    } else {
      OUT[0] = IN[0];
      OUT[1] = IN[1];
    }
    OUT[0] = constrain(OUT[0],MIN,MAX);
  };
} stuntDriver;



////////////////////////////////////////////////////////////////////////////////
// Tank mode
////////////////////////////////////////////////////////////////////////////////
ParameterList tankConfig = {
  .conf = {
    PCONF_RANGE("rate",60,"deg/sec",10,720,10),
    PCONF_NUMBER("P",50,"%%",0,100),
    PCONF_NUMBER("I",0,"%%",0,100),
    PCONF_NUMBER("D",0,"%%",0,100),
    PCONF_RANGE("rev",0,"bool",0,1,1),
    //PCONF_NUMBER("mix",50,"%%",0,100),
    PCONF_END,
  },
};
#define TANK_RATE (500./tankConfig.getIval(0))
#define TANK_P  (tankConfig.getIval(1)/50.)
#define TANK_I  (tankConfig.getIval(2)/250.)
#define TANK_D  (tankConfig.getIval(3)/500.)
#define TANK_REV  (tankConfig.getIval(4))
#define TANK_MIX  (tankConfig.getIval(5)/50.)


class tankDriver: public Driver {
  int MIN = 1000;
  int MAX = 2000;
  LibPID RATE_PID;
public:
  void setup() {
    MIN = min(CH1_MIN,CH1_MAX);
    MAX = max(CH1_MIN,CH1_MAX);
    RATE_PID.setup(TANK_P,TANK_I,TANK_D,MIN,CH1_MID,MAX,400);
  };
  void loop() {
    float YAW_RATE = GYRO[2];
    float OUT_RATE = RATE_PID.loop(IN[0], TANK_RATE*(TANK_REV? -YAW_RATE: YAW_RATE)) - CH1_MID;
    OUT[0] = IN[1] + OUT_RATE;
    OUT[1] = IN[1] - OUT_RATE;
    //
    if (abs(IN[1] - CH2_MID) < CH2_DEAD) {
      // Neutral
      OUT[0] = OUT[1] = CH2_MID;
    } else {
      // Forward/Reverse
      OUT[0] = constrain(OUT[0],CH2_MIN,CH2_MAX);
      OUT[1] = constrain(OUT[1],CH2_MIN,CH2_MAX);
    }
  };
} tankDriver;



////////////////////////////////////////////////////////////////////////////////
// Drone mode
////////////////////////////////////////////////////////////////////////////////
ParameterList droneConfig = {
  .conf = {
    PCONF_RANGE("angle",30,"deg",10,60,5),
    PCONF_NUMBER("P1",50,"%%",0,100),
    PCONF_NUMBER("I1",0,"%%",0,100),
    PCONF_NUMBER("D1",0,"%%",0,100),
    PCONF_RANGE("rate",50,"deg/sec",10,360,5),
    PCONF_NUMBER("P2",50,"%%",0,100),
    PCONF_NUMBER("I2",0,"%%",0,100),
    PCONF_NUMBER("D2",0,"%%",0,100),
    PCONF_END,
  },
};
#define DRONE_ANGL  (500./droneConfig.getIval(0))
#define DRONE_P1  (droneConfig.getIval(1)/50.)
#define DRONE_I1  (droneConfig.getIval(2)/250.)
#define DRONE_D1  (droneConfig.getIval(3)/500.)
#define DRONE_RATE  (500./droneConfig.getIval(4))
#define DRONE_P2  (droneConfig.getIval(5)/50.)
#define DRONE_I2  (droneConfig.getIval(6)/250.)
#define DRONE_D2  (droneConfig.getIval(7)/500.)


class droneDriver: public Driver {
  const int MIN = 1000;
  const int MID = 1500;
  const int MAX = 2000;
  const int FREQ = 400;
  LibPID PITCH_PID;
  LibPID ROLL_PID;
  LibPID RATE_PID;
public:
  void setup() {
    PITCH_PID.setup(DRONE_P1,DRONE_I1,DRONE_D1,MIN,MID,MAX,FREQ);
    ROLL_PID.setup(DRONE_P1,DRONE_I1,DRONE_D1,MIN,MID,MAX,FREQ);
    RATE_PID.setup(DRONE_P2,DRONE_I2,DRONE_D2,MIN,MID,MAX,FREQ);
  };
  void loop() {
    float PITCH = AHRS[0];
    float ROLL = AHRS[1];
    float RATE = GYRO[2];
    float OUT_PITCH = PITCH_PID.loop(IN[1], DRONE_ANGL*PITCH) - MID;
    float OUT_ROLL = ROLL_PID.loop(IN[0], DRONE_ANGL*ROLL) - MID;
    float OUT_RATE = RATE_PID.loop(IN[3], DRONE_RATE*RATE) - MID;
    //
    OUT[0] = IN[1] - OUT_PITCH - OUT_ROLL + OUT_RATE;
    OUT[1] = IN[1] - OUT_PITCH + OUT_ROLL - OUT_RATE;
    OUT[2] = IN[1] + OUT_PITCH + OUT_ROLL + OUT_RATE;
    OUT[3] = IN[1] + OUT_PITCH - OUT_ROLL - OUT_RATE;
    //
    OUT[0] = constrain(OUT[0],MIN,MAX);
    OUT[1] = constrain(OUT[1],MIN,MAX);
    OUT[2] = constrain(OUT[2],MIN,MAX);
    OUT[3] = constrain(OUT[3],MIN,MAX);
  };
} droneDriver;



////////////////////////////////////////////////////////////////////////////////
// System table
////////////////////////////////////////////////////////////////////////////////
// GPIO setup
ParameterList portConfig = {
  .conf = {
    PCONF_SELECT("sbus_in",0,"port","[0,26,32,22,19,23,33]","[\"off\",\"g1\",\"g2\",\"b1\",\"b2\",\"b3\",\"b4\"]"),
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
//
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
  .conf = {
    PCONF_SELECT("run_mode",1,"mode","[1,2,3,4]","[\"gvect\",\"drift\",\"stunt\",\"tank\"]"),
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
// ESC table
////////////////////////////////////////////////////////////////////////////////
// ESC setup
ParameterList escConfig = {
  .conf = {
    PCONF_SELECT("esc_mode",1,"mode","[0,1,2]","[\"FB\",\"FR\",\"FBR\"]"),
    PCONF_SELECT("esc_brake",0,"bool","[0,1]","[\"off\",\"on\"]"),
    PCONF_RANGE("esc_drag",50,"%%",0,100,1),
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
// EOF
////////////////////////////////////////////////////////////////////////////////
