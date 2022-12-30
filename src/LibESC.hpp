////////////////////////////////////////////////////////////////////////////////
// DIY-RCシステムのRX専用ソース
// DIY radio control system by M5Stack series
// https://github.com/hshin-git/M5RadioControl
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIB_ESC_H_
#define _LIB_ESC_H_


//#define DEBUG M5.Lcd
#define DEBUG Serial


////////////////////////////////////////////////////////////////////////////////
// class LibESC{}: Hブリッジ型ドライバ利用ESC（Arduino版OSSのESP32化）
//  setup(): ESCの初期化
//  loop(): ESCの速度調整
// https://github.com/TheDIYGuy999/DRV8833
// DRV8833.h - Library for the Texas Instruments DRV8833 motor driver.
// Created by TheDIYGuy999 June 2016
// Released into the public domain.
////////////////////////////////////////////////////////////////////////////////
class LibESC {
  // PWM parameters
  const int PWM_FREQ = 8*1024; // up to 80kHz
  const int PWM_BITS = 10;	// PWM resolution in bit width
  const int MAX_DUTY = (1<<PWM_BITS)-1;
private:
  // output
  int _out1,_out2;	// GPIO pin#
  int _pwm1,_pwm2;	// PWM ch# for ESP32
  // input
  int _usecMin;
  int _usecMid;
  int _usecMax;
  int _usecDead;	// dead band >= 5usec
  int _usecMinNeutral;
  int _usecMaxNeutral;
  int _usecInput;
  // mode
  int _modeESC;	// 0:FB, 1:FR, 2:FBR
  bool _brake;		// coast or brake mode
  int  _brakeDrag;	// strength of neutral brake
  bool _invert;	// invert FR direction
  bool _doublePWM;	//
  int _statePrev;	// 1:Forward 0:Neutral -1:Reverse
  int _countN2R;	// count of transition from Neutral to Reverse
  //
public:
  // NOTE: The first pin must always be PWM capable, the second only, if the last parameter is set to "true"
  // SYNTAX: IN1, IN2, min. input value, max. input value, neutral position width
  // invert rotation direction, true = both pins are PWM capable
  LibESC() {
    setupUsec();
    setupMode();
    //setupPort();
  };
  void setup(int pin1, int pin2, int chan) {
    setupPort(pin1,pin2,chan);
  };
  void setupUsec(int usecMin=1000, int usecMid=1500, int usecMax=2000, int usecDead=5) {
    if (usecDead < 5) usecDead = 5;
    _usecMin = usecMin;
    _usecMid = usecMid;
    _usecMax = usecMax;
    _usecDead = usecDead;
    _usecMinNeutral = _usecMid - usecDead;
    _usecMaxNeutral = _usecMid + usecDead;
    _usecInput = _usecMid;
  };
  void setupMode(int modeESC=1, bool brake=false, int brakeDrag=0, bool invert=false, bool doublePWM=true) {
    _modeESC = modeESC;
    _brake = brake;
    _brakeDrag = brakeDrag;
    _invert = invert;
    _doublePWM = doublePWM;
    _statePrev = 0;
    _countN2R = 0;
  };
  void setupPort(int out1=10, int out2=11, int chan=0) {
    _out1 = out1;
    _out2 = out2;
    #if ESP32
    _pwm1 = 2*chan + 0;
    _pwm2 = 2*chan + 1;
    pinMode(_out1, OUTPUT);
    pinMode(_out2, OUTPUT);
    digitalWrite(_out1, 0);
    digitalWrite(_out2, 0);
    ledcSetup(_pwm1,PWM_FREQ,PWM_BITS);
    ledcSetup(_pwm2,PWM_FREQ,PWM_BITS);
    ledcWrite(_pwm1,0);
    ledcWrite(_pwm2,0);
    ledcAttachPin(_out1,_pwm1);
    ledcAttachPin(_out2,_pwm2);
    #else
    _pwm1 = _out1;
    _pwm2 = _out2;
    pinMode(_out1, OUTPUT);
    pinMode(_out2, OUTPUT);
    digitalWrite(_out1, LOW);
    digitalWrite(_out2, LOW);
    #endif
  }
  inline void _analogWrite(int chan, int duty) {
    #if ESP32
    ledcWrite(chan, duty);
    #else
    analogWrite(chan, duty);
    #endif
    //DEBUG.print(chan);DEBUG.print(" ");DEBUG.println(duty);
  }
  inline void _motorForward(int duty, bool brake=false) {
    duty = constrain(duty, 0,MAX_DUTY);
    if (brake) {
      _analogWrite(_pwm2, MAX_DUTY);
      _analogWrite(_pwm1, MAX_DUTY - duty);
    } else {
      _analogWrite(_pwm1, 0);
      _analogWrite(_pwm2, duty);
    };
  }
  inline void _motorReverse(int duty, bool brake=false) {
    duty = constrain(duty, 0,MAX_DUTY);
    if (brake) {
      _analogWrite(_pwm1, MAX_DUTY);
      _analogWrite(_pwm2, MAX_DUTY - duty);
    } else {
      _analogWrite(_pwm2, 0);
      _analogWrite(_pwm1, duty);
    };
  }
  inline void _motorBrake(int duty, bool brake=false) {
    duty = constrain(duty, 0,MAX_DUTY);
    if (brake) {
      _analogWrite(_pwm1, MAX_DUTY - duty);
      _analogWrite(_pwm2, MAX_DUTY - duty);
    } else {
      _analogWrite(_pwm1, duty);
      _analogWrite(_pwm2, duty);
    }
  }
  // SYNTAX: Input value, max PWM, ramptime in ms per 1 PWM increment
  // true = brake active, false = brake in neutral position inactive
  void loop(int usecInput) {
    _usecInput = usecInput;
    if (_invert) _usecInput = map(_usecInput, _usecMin,_usecMax, _usecMax,_usecMin); // invert driving direction

    // H bridge controller
    bool Forward = (_usecInput >= _usecMaxNeutral);
    bool Reverse = (_usecInput <= _usecMinNeutral);

    // duty
    int duty = 0;
    if (Forward) {
      // Forward
      duty = map(_usecInput, _usecMaxNeutral,_usecMax, 0,MAX_DUTY);
      _countN2R = 0;
      _statePrev = 1;
    } else if (Reverse) {
      // Reverse
      duty = map(_usecInput, _usecMinNeutral,_usecMin, 0,MAX_DUTY);
      if (_statePrev == 0) _countN2R = _countN2R + 1;
      _statePrev = -1;
    } else {
      // Neutral
      duty = (_modeESC == 1)? map(_brakeDrag, 0,100, 0,MAX_DUTY): 0;
      _statePrev = 0;
    }

    // output
    if (_doublePWM) { // Mode with two PWM capable pins (both pins must be PWM capable!) -----
      if (Forward)
        _motorForward(duty,_brake);
      else if (Reverse && (_modeESC == 1))
        _motorReverse(duty,_brake);
      else if (Reverse && (_modeESC == 2) && (_countN2R >= 2))
        _motorReverse(duty,_brake);
      else // Neutral
        _motorBrake(duty,_brake);
    } else { 
      // Mode with only one PWM capable pin (pin 1 = PWM, pin2 = direction) -----
      // NOTE: the brake is always active in one direction and always inactive in the other!
      // Only use this mode, if your microcontroller does not have enough PWM capable pins!
      // If the brake is active in the wrong direction, simply switch both motor wires and
      // change the "invert" boolean!
      if (Forward)
        _motorForward(duty,true);
      else if (Reverse)
        _motorReverse(duty,false);
      else // Neutral
        _motorBrake(MAX_DUTY,_brake);
    }
  }

};




#endif
