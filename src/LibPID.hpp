#ifndef _LIB_PID_
#define _LIB_PID_

////////////////////////////////////////////////////////////////////////////////
// class LibPID{}: PID（比例、積分、微分）制御アルゴリズム（QuickPIDのラッパ）
//  setup(): PID制御のパラメータ変更
//  loop(): PID制御の出力計算
////////////////////////////////////////////////////////////////////////////////
#include "QuickPID.h"

class LibPID {
public:
  
  float Setpoint, Input, Output;
  float Min, Mean, Max;
  QuickPID* QPID;
  
  LibPID(void) {
    //
    Setpoint = 0.0F;
    Input = 0.0F;
    Output = 0.0F;
    //
    Mean = 1500;
    Min = 1000 - Mean;
    Max = 2000 - Mean;
    //
    QPID = new QuickPID(&Input, &Output, &Setpoint, 1.0,0.0,0.0, QuickPID::Action::direct);
    QPID->SetAntiWindupMode(QuickPID::iAwMode::iAwClamp);
    QPID->SetMode(QuickPID::Control::automatic);
    QPID->SetOutputLimits(Min,Max);
    QPID->SetSampleTimeUs(1000000/50);
  }
  
  // PID setup
  void setup(float Kp, float Ki, float Kd, int MIN=1000, int MEAN=1500, int MAX=2000, int Hz=50) {
    Min = MIN - MEAN;
    Mean = MEAN;
    Max = MAX - MEAN;
  
    QPID->SetTunings(Kp,Ki,Kd);
    QPID->SetOutputLimits(Min,Max);
    QPID->SetSampleTimeUs(Hz>=50? 1000000/Hz: 1000000/50);
  }
  void setupT(float Kp, float Ti, float Td, int MIN=1000, int MEAN=1500, int MAX=2000, int Hz=50) {
    if (Ti <= 0.0) Ti = 1.0;
    float Ki = Kp/Ti;
    float Kd = Kp*Td;
    setup(Kp,Ki,Kd, MIN,MEAN,MAX,Hz);
  }
  void setupU(float Ku, float Tu, int MIN=1000, int MEAN=1500, int MAX=2000, int Hz=50) {
    if (Tu <= 0.0) Tu = 1.0;
    // Ziegler–Nichols method
    float Ti = 0.50*Tu;
    float Td = 0.125*Tu;
    float Kp = 0.60*Ku;
    float Ki = Kp/Ti;
    float Kd = Kp*Td;
    setup(Kp,Ki,Kd, MIN,MEAN,MAX,Hz);
  }
  
  // PID loop
  float loop(float SP, float PV) {
    // Compute PID
    Setpoint = (SP > 0? SP - Mean: 0.0);
    Input = PV;
    QPID->Compute();
    return SP > 0? Mean + constrain(Output,Min,Max): 0;
  }

  // debug print
  void debug(void) {
    DEBUG.printf("%.2f %.2f %.2f\n", Setpoint,Input,Output);
  }

};



////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif

