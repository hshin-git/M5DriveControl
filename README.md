# M5DriveControl
Multi-functional drive controller for hobby RC car/boat


## Description

- RCカーの多機能ドライブコントローラ"M5DriveControl(M5DC)"です。
- RCカー搭載のIMUを用いて、人間では不可能な運転制御を実現します。
- ドライブ制御用のアルゴリズムとパラメータを利用者が追加できます。


## DEMO


<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">RCカー用ジャイロを改造して「汎用ドライブコントローラー」でけた✌。受信器とIMU信号を入力にサーボ指令を毎秒400回出力する。制御アルゴリズムを追加できるので、ドリフトやスタント用以外にタンクや倒立ロボット用にも拡張できそう。<a href="https://t.co/79r7ZiOycV">https://t.co/79r7ZiOycV</a><a href="https://twitter.com/hashtag/m5stack?src=hash&amp;ref_src=twsrc%5Etfw">#m5stack</a> <a href="https://twitter.com/hashtag/%E3%82%BF%E3%83%9F%E3%83%A4RC?src=hash&amp;ref_src=twsrc%5Etfw">#タミヤRC</a> <a href="https://t.co/FQP6Ttldss">pic.twitter.com/FQP6Ttldss</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1580895090956201984?ref_src=twsrc%5Etfw">October 14, 2022</a></blockquote>


- [ドリフト](https://protopedia.net/prototype/2351)

- [スタント](https://protopedia.net/prototype/3144)

- [グリップ](https://twitter.com/bitter_acids/status/1589181781013848064)
<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">RCカーでGベクタリングコントロールをトライアル中です。スロットル一定でも曲がる感があり面白い。<a href="https://t.co/Ag2onTEae0">https://t.co/Ag2onTEae0</a> <a href="https://t.co/loPcm9w0r6">pic.twitter.com/loPcm9w0r6</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1589181781013848064?ref_src=twsrc%5Etfw">November 6, 2022</a></blockquote>

- [タンク/ESC](https://twitter.com/bitter_acids/status/1589359870247317504)
<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">ドリームジャイロM5DriveControlのテストカーです。PIDコントローラーが目標の旋回速度となるよう左右のモーター出力を自動調整して曲がります。夢ジャイロの出力はESC用PWMだけでなくモータードライバ（Hブリッジ）用PWMも選べます。ハードの実装は適当です😁。<a href="https://t.co/lkv8I2Rxls">https://t.co/lkv8I2Rxls</a> <a href="https://t.co/3eQfsdrqkY">https://t.co/3eQfsdrqkY</a> <a href="https://t.co/gQnAwPtI1O">pic.twitter.com/gQnAwPtI1O</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1589359870247317504?ref_src=twsrc%5Etfw">November 6, 2022</a></blockquote> 

- [グラホ/ESC](https://twitter.com/bitter_acids/status/1606455913690587136)
<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">グラスホッパーⅡです。ESCは汎用モータードライバ （DC5~15V 最大50A Hブリッジ）をm5atomで動かすハンドメイドです。普通に走ります✌<a href="https://twitter.com/hashtag/%E3%82%BF%E3%83%9F%E3%83%A4RC?src=hash&amp;ref_src=twsrc%5Etfw">#タミヤRC</a> <a href="https://twitter.com/hashtag/m5stack?src=hash&amp;ref_src=twsrc%5Etfw">#m5stack</a> <a href="https://t.co/94MQZ9tbjo">pic.twitter.com/94MQZ9tbjo</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1606455913690587136?ref_src=twsrc%5Etfw">December 24, 2022</a></blockquote>



## Features

- ドリフトRCカー用ジャイロ（ヨーレート目標の自動操舵）
- スタントRCカー用片輪走行（ロール角目標の自動操舵）
- タンク用ジャイロ（ヨーレート目標の左右スロットル調整）
- グリップRCカー用スロットルアシスト（GVC: G-Vectoring Control）
- 汎用モータドライバ（H-bridge）のESC化
- RC受信機の[S.BUS](https://ja.wikipedia.org/wiki/S.BUS)/PWM接続
- コントロール周期は約400Hz
- スマホからM5DCへWiFi接続してパラメータ調整
- M5DCのGPIOはパラメータで設定可能
- 利用者によるパラメータとアルゴリズムの追加



## Requrement

- ホビーRCカー（受信機は[S.BUS](https://ja.wikipedia.org/wiki/S.BUS)又はPWM出力）
- [M5Atom Matrix](https://docs.m5stack.com/en/core/atom_matrix)
- [Arduino IDE](https://www.arduino.cc/en/software)
- [ESP32 SPIFS](https://github.com/me-no-dev/arduino-esp32fs-plugin)
- コネクタとワイヤ（コントローラGPIOとRCユニットを接続）



## Installation

- M5Atom Matrixのデータ領域SPIFSに"data/*"を書き込む（[ESP32 SPIFS](https://github.com/me-no-dev/arduino-esp32fs-plugin)利用）
- M5Atom Matrixのプログラム"M5DriveControl"を書き込む（[Arduino IDE](https://www.arduino.cc/en/software)利用）
- M5Atom MatrixをRCユニットと接続する（受信機からサーボ又はESCの間）



## Usage
T.B.D.



## Advanced Usage
ドリフトRCカー用ジャイロの追加例です。
制御パラメータ"driftConfig"、制御アルゴリズム"driftDriver"を作成します。
```
ParameterList driftConfig = {
  .body = {0},
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
```


M5DCに制御パラメータと制御アルゴリズムを登録します。
```
ParameterTable WebConfig::CONFIG = {{
  ...
  //
  // user config in MyDriver.cpp
  //
  {.name="gvect",.list=&gvectConfig},
  {.name="drift",.list=&driftConfig},
  {.name="stunt",.list=&stuntConfig},
  {.name="tank",.list=&tankConfig},
}};


//
// definte of controller table
// (selected by global->run_mode)
//
Driver *DRIVER[] = {
  ...
  //
  // user driver in MyDrive.cpp
  //
  &gvectDriver,
  &driftDriver,
  &stuntDriver,
  &tankDriver,
};
```

以上でコンパイルしてM5DCへプログラムを書き込めば完了です。
M5DCの設定画面から追加したコントローラを呼び出して利用できます。



## Author
- [protopedia](https://protopedia.net/prototyper/hshin)
- [twitter](https://twitter.com/bitter_acids)



## License


