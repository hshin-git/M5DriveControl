# M5DriveControl
Multi-functional drive controller for hobby RC car/boat

## Description

- RCカー用の多機能ドライブコントローラ"M5DriveControl(M5DC)"です。
- ドライブ制御用のアルゴリズムとパラメータを利用者が追加できます。


## DEMO

https://protopedia.net/prototype/2351
https://protopedia.net/prototype/3144

https://twitter.com/bitter_acids/status/1589359870247317504?s=20&t=2DZw5PTo6njKDlqDN1gCRQ
https://twitter.com/bitter_acids/status/1589181781013848064?s=20&t=2DZw5PTo6njKDlqDN1gCRQ


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


## Usage


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



## Installation

- M5Atom Matrixのデータ領域SPIFSに"data/*"を書き込む（[ESP32 SPIFS](https://github.com/me-no-dev/arduino-esp32fs-plugin)利用）
- M5Atom Matrixのプログラム"M5DriveControl"を書き込む（[Arduino IED](https://www.arduino.cc/en/software)利用）
- M5Atom MatrixをRCユニットと接続する（受信機からサーボ又はESCの間）


## Author
https://protopedia.net/prototyper/hshin
https://twitter.com/bitter_acids


## License


