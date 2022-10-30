#ifndef _LIB_PWM_
#define _LIB_PWM_
////////////////////////////////////////////////////////////////////////////////
// class LibPWM{}: PWM信号の入出力ライブラリ
//  setupIn(): 入力ピンの初期化
//  getUsec(): 入力パルス幅[usec]
//  getFreq(): 入力パルス周波数[Hz]
//  setupMean(): 入力パルス平均
//  getUsecMean(): 入力パスル平均[usec]
//  attach(): 割り込み処理の再開
//  detach(): 割り込み処理の中止
//  setupOut(): 出力ピンの初期化
//  putUsec(): 出力パルス幅[usec]
//  putFreq(): 出力パルス周波数[Hz]
//  reset(): 状態リセット
////////////////////////////////////////////////////////////////////////////////

// PWM watch dog timer
#include <Ticker.h>

// PWM pulse in
typedef struct {
  int pin;
  int tout;
  // for pulse
  int dstUsec;
  int prev;
  unsigned long last;
  // for freq
  int dstFreq;
  unsigned long lastFreq;
} InPulse;

// PWM pulse out
typedef struct {
  int pin;
  int freq;
  int bits;
  int duty;
  int usec;
  int dstUsec;
} OutPulse;

class LibPWM {
  static const int MAX = 8; // max of channels 

  static int InCH;   // number of in-channels
  static int OutCH;   // number of out-channels
  
  static InPulse IN[MAX]; // pwm in-pulse
  static OutPulse OUT[MAX]; // pwm out-pulse

  static Ticker WDT;      // watch dog timer
  static bool WATCHING;
  static float MEAN[MAX]; // mean of pwm in-pulse
  
  static void ISR(void *arg) {
    unsigned long tnow = micros();
    int ch = (int)arg;
    InPulse* pwm = &IN[ch];
    int vnow = digitalRead(pwm->pin);
    
    if (pwm->prev==0 && vnow==1) {
      // at up edge
      pwm->prev = 1;
      pwm->last = tnow;
      // for freq
      pwm->dstFreq = (tnow > pwm->lastFreq? 1000000/(tnow - pwm->lastFreq): 0);
      pwm->lastFreq = tnow;
    }
    else
    if (pwm->prev==1 && vnow==0) {
      // at down edge
      pwm->dstUsec = tnow - pwm->last;
      pwm->prev = 0;
      pwm->last = tnow;
    }
  }

  static void TSR(void) {
    unsigned long tnow = micros();
    for (int ch=0; ch<InCH; ch++) {
      InPulse* pwm = &IN[ch];
      if (pwm->last + pwm->tout < tnow) {
        pwm->dstUsec = 0;
        pwm->dstFreq = 0;
      }
    }
  }

public: 
  LibPWM() {
    // do nothing
  };
  static int setupIn(int pin, int toutUs=21*1000) {
    int ch = -1;
    if (InCH < MAX) {
      ch = InCH++;
      InPulse* pwm = &IN[ch];
      //
      pwm->pin = pin;
      pwm->tout = toutUs;
      // for pulse
      pwm->dstUsec = 0;
      pwm->prev = 0;
      pwm->last = micros();
      // for freq
      pwm->dstFreq = 0;
      pwm->lastFreq = micros();
      //
      pinMode(pin,INPUT);
      attachInterruptArg(pin,&ISR,(void*)ch,CHANGE);
      if (ch == 0) WDT.attach_ms(pwm->tout/1000,&TSR);
      WATCHING = true;
    }
    return ch;
  };
  static int getUsec(int ch) {
    if (ch >= 0 && ch < InCH) {
      InPulse* pwm = &IN[ch];
      return pwm->dstUsec;
    }
    return -1;
  }
  static int getFreq(int ch) {
    if (ch >= 0 && ch < InCH) {
      InPulse* pwm = &IN[ch];
      return pwm->dstFreq;
    }
    return -1;
  }
  
  static void detach(void) {
    if (InCH == 0) return;
    WDT.detach();
    for (int ch=0; ch<InCH; ch++) {
      InPulse *pwm = &IN[ch];
      detachInterrupt(pwm->pin);
    }
    WATCHING = false;
  };
  static void attach(void) {
    if (InCH == 0 || WATCHING) return;
    for (int ch=0; ch<InCH; ch++) {
      InPulse *pwm = &IN[ch];
      attachInterruptArg(pwm->pin,&ISR,(void*)ch,CHANGE);
      if (ch == 0) WDT.attach_ms(pwm->tout/1000,&TSR);
    }
    WATCHING = true;
  };
  static void reset(void) {
    detach();
    resetOut();
    InCH = OutCH = 0;
  };

  // ESP32's PWM channel 0 and 1 have common frequency.
  static inline int CH2PWM(int ch) { return (ch)*2; };

  static int setupOut(int pin, int freq = 50, int bits = 16) {
    int ch = -1;
    if (OutCH < MAX) {
      ch = OutCH++;
      OutPulse* out = &OUT[ch];
      out->pin = pin;
      out->freq = freq;
      out->bits = bits;
      out->duty = (1 << bits);
      out->usec = 1000000/freq;
      //
      pinMode(out->pin,OUTPUT);
      ledcSetup(CH2PWM(ch),out->freq,out->bits);
      ledcWrite(CH2PWM(ch),0);
      ledcAttachPin(out->pin,CH2PWM(ch));
      //DEBUG.printf("setupOut: ch=%d freq=%d bits=%d usec=%d\n",ch,out->freq,out->bits,out->usec);
    }
    return ch;
  }
  static void resetOut(void) {
    for (int ch=0; ch<OutCH; ch++) {
      OutPulse* out = &OUT[ch];
      ledcDetachPin(out->pin);
      pinMode(out->pin,INPUT);
    }
    OutCH = 0;
  }
  static float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
  static bool putUsec(int ch, float usec) {
    if (ch >= 0 && ch < OutCH) {
      OutPulse* out = &OUT[ch];
      int duty = mapFloat(usec, 0,out->usec, 0,out->duty);
      ledcWrite(CH2PWM(ch), duty);
      out->dstUsec = usec;
      return true;
    }
    return false;
  }
  static bool putFreq(int ch, int freq) {
    if (ch >= 0 && ch < OutCH) {
      OutPulse* out = &OUT[ch];
      out->freq = freq;
      out->usec = 1000000/freq;
      //
      ledcWrite(CH2PWM(ch),0);
      ledcDetachPin(out->pin);
      ledcSetup(CH2PWM(ch),out->freq,out->bits);
      ledcWrite(CH2PWM(ch),0);
      ledcAttachPin(out->pin,CH2PWM(ch));
      //DEBUG.printf("putFreq: ch=%d freq=%d bits=%d usec=%d\n",ch,out->freq,out->bits,out->usec);
      return true;
    }
    return false;
  }

  static void setupMean(bool first = false, int msec = 1000) {
    if (first) {
      unsigned long int timeout = millis() + msec;
      int count = 0;
      for (int ch=0; ch<MAX; ch++) MEAN[ch] = 0.0F;
      while (timeout > millis()) {
        for (int ch=0; ch<MAX; ch++) MEAN[ch] += getUsec(ch);
        count++;
        delay(5);
      }
      for (int ch=0; ch<MAX; ch++) MEAN[ch] /= count;
    }
//    detach();
//    if (PREFS.begin(M5LOGGER)) {
//      if (first)
//        PREFS.putBytes("servo", (uint8_t*)MEAN, sizeof(MEAN));
//      else
//        PREFS.getBytes("servo", (uint8_t*)MEAN, sizeof(MEAN));
//      PREFS.end();
//    }
//    attach();
  }
  static float getUsecMean(int ch) { return MEAN[ch]; }

  static void dump(void) {
    for (int ch=0; ch<InCH; ch++) {
      InPulse* pwm = &IN[ch];
      DEBUG.printf(" in(%d): pin=%2d pulse=%6d (usec) freq=%4d (Hz)\n", ch,pwm->pin,pwm->dstUsec,pwm->dstFreq);
    }
    for (int ch=0; ch<OutCH; ch++) {
      OutPulse* out = &OUT[ch];
      DEBUG.printf("out(%d): pin=%2d pulse=%6d (usec) freq=%4d (Hz)\n", ch,out->pin,out->dstUsec,out->freq);
    }
  }

};

// initialization for static class member
int LibPWM::InCH = 0;
int LibPWM::OutCH = 0;

InPulse LibPWM::IN[LibPWM::MAX];
OutPulse LibPWM::OUT[LibPWM::MAX];

Ticker LibPWM::WDT;
bool LibPWM::WATCHING = false;
float LibPWM::MEAN[LibPWM::MAX];


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif
