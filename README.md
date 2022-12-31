# M5DriveControl
Multi-functional drive controller for hobby RC car/boat

![M5DriveControl](https://user-images.githubusercontent.com/64751855/210130839-8ac4bba2-a86e-4d9c-adf9-b94cc5bffb43.png)



## 概要

- RCカーの多機能ドライブコントローラM5DriveControl (M5DC)です。
- RCカー搭載のIMUを用いて人間では不可能な運動制御を実現します。
- ドライブ制御用のアルゴリズム及びパラメータを簡単に追加できます。


## デモ
多機能ドライブコントローラM5DCの活用例です。アルゴリズム追加による機能拡張も可能です。

<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">RCカー用ジャイロを改造して「汎用ドライブコントローラー」でけた✌。受信器とIMU信号を入力にサーボ指令を毎秒400回出力する。制御アルゴリズムを追加できるので、ドリフトやスタント用以外にタンクや倒立ロボット用にも拡張できそう。<a href="https://t.co/79r7ZiOycV">https://t.co/79r7ZiOycV</a><a href="https://twitter.com/hashtag/m5stack?src=hash&amp;ref_src=twsrc%5Etfw">#m5stack</a> <a href="https://twitter.com/hashtag/%E3%82%BF%E3%83%9F%E3%83%A4RC?src=hash&amp;ref_src=twsrc%5Etfw">#タミヤRC</a> <a href="https://t.co/FQP6Ttldss">pic.twitter.com/FQP6Ttldss</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1580895090956201984?ref_src=twsrc%5Etfw">October 14, 2022</a></blockquote>


- [ドリフトRC](https://protopedia.net/prototype/2351)
<blockquote>
2021.08.07 公開（2021.09.07 更新）
M5StickCがRCカーのドリフト走行を安定化します。
M5StickC stabilizes your "drift driving" of RC car.
</blockquote>

- [スタントRC](https://protopedia.net/prototype/3144)
<blockquote>
2022.08.14 公開（2022.09.13 更新）
RCカーのスタント走行（片輪走行）用ステアリング補助システム。
Steering assist system for RC car to drive stunt "sidewall skiing".
</blockquote>

- [グリップRC](https://twitter.com/bitter_acids/status/1589181781013848064)
<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">RCカーでGベクタリングコントロールをトライアル中です。スロットル一定でも曲がる感があり面白い。<a href="https://t.co/Ag2onTEae0">https://t.co/Ag2onTEae0</a> <a href="https://t.co/loPcm9w0r6">pic.twitter.com/loPcm9w0r6</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1589181781013848064?ref_src=twsrc%5Etfw">November 6, 2022</a></blockquote>

- [タンクRC/ESC](https://twitter.com/bitter_acids/status/1589359870247317504)
<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">ドリームジャイロM5DriveControlのテストカーです。PIDコントローラーが目標の旋回速度となるよう左右のモーター出力を自動調整して曲がります。夢ジャイロの出力はESC用PWMだけでなくモータードライバ（Hブリッジ）用PWMも選べます。ハードの実装は適当です😁。<a href="https://t.co/lkv8I2Rxls">https://t.co/lkv8I2Rxls</a> <a href="https://t.co/3eQfsdrqkY">https://t.co/3eQfsdrqkY</a> <a href="https://t.co/gQnAwPtI1O">pic.twitter.com/gQnAwPtI1O</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1589359870247317504?ref_src=twsrc%5Etfw">November 6, 2022</a></blockquote> 

- [タミヤRC/ESC](https://twitter.com/bitter_acids/status/1606455913690587136)
<blockquote class="twitter-tweet"><p lang="ja" dir="ltr">グラスホッパーⅡです。ESCは汎用モータードライバ （DC5~15V 最大50A Hブリッジ）をm5atomで動かすハンドメイドです。普通に走ります✌<a href="https://twitter.com/hashtag/%E3%82%BF%E3%83%9F%E3%83%A4RC?src=hash&amp;ref_src=twsrc%5Etfw">#タミヤRC</a> <a href="https://twitter.com/hashtag/m5stack?src=hash&amp;ref_src=twsrc%5Etfw">#m5stack</a> <a href="https://t.co/94MQZ9tbjo">pic.twitter.com/94MQZ9tbjo</a></p>&mdash; 辛酸 (@bitter_acids) <a href="https://twitter.com/bitter_acids/status/1606455913690587136?ref_src=twsrc%5Etfw">December 24, 2022</a></blockquote>



## 特長
M5DCの代表的な特長を列挙します。

- ドリフトRCカー用ジャイロ（[ヨーレート目標の操舵制御](https://protopedia.net/prototype/2351)）
- スタントRCカー用片輪走行（[ロールアングル目標の操舵制御](https://protopedia.net/prototype/3144)）
- タンク用ジャイロ（[ヨーレート目標の左右ESC制御](https://twitter.com/bitter_acids/status/1589359870247317504)）
- グリップRCカー用アシスト（[GVC:G-Vectoring Control](https://www.mazda.co.jp/beadriver/dynamics/skyactivvd/gvc/)）
- 汎用モータドライバ対応（[制御モードFR/FB/FBR対応のESC化](https://twitter.com/bitter_acids/status/1606455913690587136)）
- [S.BUS](https://ja.wikipedia.org/wiki/S.BUS)/PWM対応（RC受信機との通信方式）
- 高速制御（制御ループは約400Hz≒約2.5msec）
- スマホ対応（スマホからM5DCへWiFi接続してセッティング）
- 高い拡張性（利用者によるパラメータ及びアルゴリズム追加）



## 要件
標準的なホビーRCカー、M5Atom Matrixとパソコンさえあれば利用できます。

- [ホビーRCカー](https://www.tamiya.com/japan/rc/index.html)（標準的なRCメカ：RC受信機、サーボ、ESC構成）
- [M5Atom Matrix](https://docs.m5stack.com/en/core/atom_matrix)
- [Arduino IDE](https://www.arduino.cc/en/software)
- [ESP32 SPIFS uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin)
- ワイヤハーネス類（コントローラM5DCとRCユニットの接続用、汎用ジャンパーケーブル可）



## インストール
概ねArduino IDEでM5Stackにプログラム書き込みする標準的な手順通りですが、
スマホ設定画面で用いるHTMLファイルとJavascriptライブラリをSPIFSに転送する必要があります。

- M5Atom Matrixのデータ領域SPIFSに[data/*](data/)を書き込む（[ESP32 SPIFS](https://github.com/me-no-dev/arduino-esp32fs-plugin)利用）
- M5Atom Matrixのプログラム領域に[M5DriveControl.ino](M5DriveControl.ino)を書き込む（[Arduino IDE](https://www.arduino.cc/en/software)利用）
- M5Atom MatrixをRCユニットと接続する（受信機からサーボ又はESCの間）



## 使い方（利用者）
コントローラM5DCは制御モード（制御オン、WiFiなし）と設定モード（制御オフ、WiFIあり）の状態を持ちます。
電源投入後は制御モードとなり、前面ボタン（A）により両モード間を行き来します。

- コントローラ本体の電源を入れる（USBケーブルまたはRCユニットBECから給電）
- コントローラ本体の前面ボタンを押して制御モードから設定モードに入る
- スマホからコントローラ本体へWiFi接続する（モバイル通信オフ、SSIDはm5atom、パスワードなし）
- スマホのブラウザから設定画面を表示する（IPアドレスは192.168.4.1）
- スマホのブラウザから必要なパラメータを設定する（入出力ピン定義、制御モード、制御パラメータ等）
- スマホのブラウザから"exit"ボタン又はコントローラ本体の前面ボタンを押して制御モードへ戻る
- RCカーを操縦して必要に応じて制御パラメータを調整する


![M5DC_WiFi_Setting](https://user-images.githubusercontent.com/64751855/210130915-7f154818-4a60-44a4-929c-8a5c9cbe35f5.jpg)



## 使い方（開発者）
ドリフトRCカー用ジャイロの具体例（実装済み）を用いて制御パラメータ／アルゴリズムの追加方法を解説します。
ファイル[MyDriver.hpp](MyDriver.hpp)を編集して制御パラメータdriftConfig及び制御アルゴリズムdriftDriverを追記します。

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


ファイル[M5DriveControl.ino](M5DriveControl.ino)を編集して制御パラメータと制御アルゴリズムを登録します。

```
ParameterTable WebConfig::CONFIG = {{
  ...
  //
  // refto user config defined in MyDriver.hpp
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
  // refto user driver defined in MyDriver.hpp
  //
  &gvectDriver,
  &driftDriver,
  &stuntDriver,
  &tankDriver,
};
```

以上でコンパイルしてM5DCへプログラムを書き込めば完了です。
M5DCの設定画面から追加したコントローラを呼び出して利用できます。



## 作者
- [Protopedia](https://protopedia.net/prototyper/hshin)
- [Twitter](https://twitter.com/bitter_acids)



以上
