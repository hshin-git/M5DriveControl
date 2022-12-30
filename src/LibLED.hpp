#ifndef _LIB_LED_
#define _LIB_LED_
////////////////////////////////////////////////////////////////////////////////
// class LibLED{}: LED制御ライブラリ（M5Atom標準ライブラリのバグ回避）
//  setup(): 初期化
//  loop(): 状態表示
//  setPixcel(): 一点塗り
//  showRainbow(): レインボウ表示
//  showRandum(): ランダム表示
////////////////////////////////////////////////////////////////////////////////
#include <FastLED.h>

class LibLED {
  CRGB LEDs[25];
  TimerMS TIME;
  bool ENABLE;
public:
  void setup(bool enable_=true,int msec= 1000) {
    ENABLE = enable_;
    if (!ENABLE) return;
    FastLED.addLeds<WS2812,27,GRB>(LEDs,25);
    FastLED.setBrightness(20);
    showRainbow(msec);
  }
  void loop(CRGB c, int mode, int *chan, int msec=100) {
    if (!ENABLE) return;
    if (TIME.isUp(msec)) {
      for (int y=0; y<5; y++) {
        for (int x=0; x<5; x++) {
          if (y>0) {
            int v = map(chan[y-1], 1000,2000, -1,5);
            setPixel(x,y, x==v? c: (x<v? CRGB::YellowGreen: CRGB::Black));
          } else {
            int m = mode - 1;
            setPixel(x,y, x==m? c: CRGB::Black);
          }
        }
      }
      FastLED.show();
    }
  }
  void setPixel(int p, CRGB c) { LEDs[p%25] = c; }
  void setPixel(int x, int y, CRGB c) { LEDs[(x+5*y)%25] = c; }
  void fillColor(CRGB c) {
    for (int p=0; p<25; p++) LEDs[p] = c;
  }
  void fillRainbow(int offset=0) {
    for (int p=0; p<25; p++) {
      int h = map((p+offset)%25, 0,25, 0,255);
      LEDs[p] = CHSV(h,255,255);
    }
  }
  void fillRandom(void) {
    for (int p=0; p<25; p++) {
      int h = random(0,255);
      LEDs[p] = CHSV(h,255,255);
    }
  }
  void showRainbow(int msec=1000) {
    if (!ENABLE) return;
    for (int i=0; i<25; i++) {
      fillRainbow(i);
      FastLED.show();
      delay(msec/25);
    }
  }
  void showRandom(int msec=1000) {
    if (!ENABLE) return;
    for (int i=0; i<25; i++) {
      fillRandom();
      FastLED.show();
      delay(msec/25);
    }
  }
};


#endif
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
