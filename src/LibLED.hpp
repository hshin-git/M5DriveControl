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

#define LED_DIM	(5)
#define LED_MAX	(LED_DIM - 1)
#define LED_NUM	(LED_DIM*LED_DIM)

class LibLED {
  CRGB LEDs[LED_NUM];
  TimerMS TIME;
  bool ENABLE;
  int AXIS;
public:
  void setup(bool enable=true,int axis=-2) {
    ENABLE = enable;
    AXIS = axis;
    if (!ENABLE) return;
    FastLED.addLeds<WS2812,27,GRB>(LEDs,LED_NUM);
    FastLED.setBrightness(10);
    //showRainbow(500);
  }
  CRGB v2c(float v, float vmin, float vmax) {
    int h = map(v, vmin,vmax, 128,0);
    if (h < 0 || h >= 128) return CRGB::Black;
    return CHSV(h,255,255);
  }
  void loop(int run, int mode, int *usec, float *gyro, float *accl, int msec=100) {
    static int color = 0;
    if (!ENABLE || !TIME.isUp(msec)) return;
    CRGB c = CRGB::Aqua;
    CRGB b = CRGB::Black;
    CRGB h = CHSV(color,255,255);
    if (run) color = (color + 4) % 255;
    for (int p=0; p<LED_NUM; p++) {
      int x = p % LED_DIM;
      int y = p / LED_DIM;
      switch(y) {
      case 0: setPixel(x,y, h); break;
      case 1: setPixel(x,y, (p==mode+1*5? c: b)); break;
      case 2: setPixel(x,y, (p>=2*5+4? b: v2c(usec[p-2*5],800,2200))); break;
      case 3: setPixel(x,y, (p>=3*5+3? b: v2c(gyro[p-3*5],-180,180))); break;
      case 4: setPixel(x,y, (p>=4*5+3? b: v2c(accl[p-4*5],-1.2,1.2))); break;
      default: setPixel(x,y, b);break;
      }
    }
    FastLED.show();
  }
  int rotate(int p) {
    int x = p % LED_DIM;
    int y = p / LED_DIM;
    int X, Y;
    switch(AXIS) {
    case  1: X = LED_MAX-x; Y = LED_MAX-y; break;
    case -1: X = x; Y = y; break;
    case  2: X = y; Y = LED_MAX-x; break;
    case -2: X = LED_MAX-y; Y = x; break;
    default: X = x; Y = y; break;
    }
    return (X + LED_DIM*Y) % LED_NUM;
  }
  void setPixel(int p, CRGB c) { LEDs[rotate(p)] = c; }
  void setPixel(int x, int y, CRGB c) { LEDs[rotate(x+LED_DIM*y)] = c; }
  void fillColor(CRGB c) {
    for (int p=0; p<LED_NUM; p++) LEDs[p] = c;
  }
  void fillRainbow(int offset=0) {
    for (int p=0; p<LED_NUM; p++) {
      int h = map((p+offset)%LED_NUM, 0,LED_NUM, 0,255);
      setPixel(p, CHSV(h,255,255));
    }
  }
  void fillRandom(void) {
    for (int p=0; p<LED_NUM; p++) {
      int h = random(0,255);
      setPixel(p, CHSV(h,255,255));
    }
  }
  void showRainbow(int msec=1000) {
    if (!ENABLE) return;
    for (int i=0; i<LED_NUM; i++) {
      fillRainbow(i);
      FastLED.show();
      delay(msec/LED_NUM);
    }
  }
  void showRandom(int msec=1000) {
    if (!ENABLE) return;
    for (int i=0; i<LED_NUM; i++) {
      fillRandom();
      FastLED.show();
      delay(msec/LED_NUM);
    }
  }
  
};


#endif
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
