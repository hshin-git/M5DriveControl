
#include <M5Atom.h>
#include "src/LibIMU.hpp"
#include "src/LibESC.hpp"
#include "src/LibMisc.hpp"
#include "src/LibPWM.hpp"
#include "src/LibLED.hpp"
#include "src/SBUS.h"
#include "MyDriver.hpp"

//
#define M5DC_LED_ENABLE (true)


////////////////////////////////////////////////////////////////////////////////
// controller parameters
////////////////////////////////////////////////////////////////////////////////
// define parameter table
ParameterTable WebConfig::CONFIG = {{
  // system
  {.name="ajax",.list=&ajaxConfig},
  {.name="global",.list=&globalConfig},
  {.name="port",.list=&portConfig},
  {.name="ch1",.list=&ch1Config}, 
  {.name="ch2",.list=&ch2Config},
  {.name="esc",.list=&escConfig},
  // user
  {.name="gvect",.list=&gvectConfig},
  {.name="drift",.list=&driftConfig},
  {.name="stunt",.list=&stuntConfig},
  {.name="tank",.list=&tankConfig},
}};


// define controller table
Driver *DRIVER[] = {
  // system
  &thruDriver,
  // user
  &gvectDriver,
  &driftDriver,
  &stuntDriver,
  &tankDriver,
};



////////////////////////////////////////////////////////////////////////////////
// controller variables
////////////////////////////////////////////////////////////////////////////////
//
WebConfig M5DC_CONF;
LibPWM M5DC_PORT;
LibIMU M5DC_AHRS;
LibLED M5DC_FACE;
//
SBUS M5DC_SBUS(Serial1);
uint16_t SBUS_Channel[18];
bool SBUS_FailSafe;
bool SBUS_LostFrame;
#define SBUS_RX (SBUS_IN)
#define SBUS_TX (-1)
//#define SBUS_TX (33)
#define SBUS2PWM(sbus)  (int(0.639534884*(sbus) + 865.755813953))
//
FilterLP IMU_LPF[3];
FilterLP OUT_LPF[2];
//
CountHZ LOOP_HZ;
CountHZ CONF_HZ;
CountHZ SBUS_HZ;
//
LibESC M5DC_ESC1;
LibESC M5DC_ESC2;



////////////////////////////////////////////////////////////////////////////////
// controller setup
////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // put your setup code here, to run once:
  M5.begin(true,true,M5DC_LED_ENABLE); //Init M5Atom-Matrix(Serial, I2C, LEDs).
  M5.IMU.Init();

  //strcpy(wifi.conf[0].body.text,"m5atom");
  M5DC_CONF.setup();

  // IMU
  M5DC_AHRS.setup(1000,IMU_AXIS);
  
  // PWM-IN or SBUS
  if (SBUS_IN) {
    M5DC_SBUS.begin(SBUS_RX,SBUS_TX,true,100000);
  } else {
    M5DC_PORT.setupIn(CH1_IN);
    M5DC_PORT.setupIn(CH2_IN);
  }
  
  // PWM-OUT
  if (ESC1_OUT) {
    M5DC_ESC1.setupPort(ESC1_OUT,ESC_BUDDY(ESC1_OUT),0);
    M5DC_ESC1.setupUsec(CH2_MIN,CH2_MID,CH2_MAX,CH2_DEAD);
    M5DC_ESC1.setupMode(ESC_MODE,ESC_BRAKE,ESC_DRAG,(ESC_INV&0x1),ESC_PWM);
  } else
    M5DC_PORT.setupOut(CH1_OUT,CH1_FREQ);
  if (ESC2_OUT) {
    M5DC_ESC2.setupPort(ESC2_OUT,ESC_BUDDY(ESC2_OUT),1);
    M5DC_ESC2.setupUsec(CH2_MIN,CH2_MID,CH2_MAX,CH2_DEAD);
    M5DC_ESC2.setupMode(ESC_MODE,ESC_BRAKE,ESC_DRAG,(ESC_INV&0x2),ESC_PWM);
  } else
    M5DC_PORT.setupOut(CH2_OUT,CH2_FREQ);

  // LED
  M5DC_FACE.setup(M5DC_LED_ENABLE,500);
  
  // DRV
  for (int i=0; i<LENGTH(DRIVER); i++) DRIVER[i]->setup();
  
  // LPF
  IMU_LPF[0].setup(IMU_SPAN,0.0);
  IMU_LPF[1].setup(IMU_SPAN,0.0);
  IMU_LPF[2].setup(IMU_SPAN,1.0);
  OUT_LPF[0].setup(CH1_SPAN,CH1_MID);
  OUT_LPF[1].setup(CH2_SPAN,CH2_MID);

}



////////////////////////////////////////////////////////////////////////////////
// controller loop
////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  LOOP_HZ.touch();

  // IMU
  M5DC_AHRS.loop(Driver::GYRO,Driver::ACCL,Driver::AHRS,&Driver::TEMP);
  for (int i=0; i<3; i++) Driver::ACCL[i] = IMU_LPF[i].update(Driver::ACCL[i]);
  
  // RCV
  if (SBUS_IN) {
    if (M5DC_SBUS.read(SBUS_Channel,&SBUS_FailSafe,&SBUS_LostFrame)) {
      SBUS_HZ.touch();
      for (int i=0; i<4; i++) Driver::IN[i] = SBUS2PWM(SBUS_Channel[i]);
    }
  } else {
    for (int i=0; i<4; i++) Driver::IN[i] = M5DC_PORT.getUsec(i);
  }
  // make dead band
  if (abs(Driver::IN[0] - CH1_MID) < CH1_DEAD) Driver::IN[0] = CH1_MID;
  if (abs(Driver::IN[1] - CH2_MID) < CH2_DEAD) Driver::IN[1] = CH2_MID;

  // LED
  M5DC_FACE.loop((SBUS_FailSafe? CRGB::Red: CRGB::Lime),RUN_MODE,Driver::IN);

  // control and output
  int MODE = RUN_MODE;
  DRIVER[MODE]->loop();
  if (ESC1_OUT)
    M5DC_ESC1.loop(OUT_LPF[0].update(DRIVER[MODE]->OUT[0]));
  else
    M5DC_PORT.putUsec(0,OUT_LPF[0].update(DRIVER[MODE]->OUT[0]));
  if (ESC2_OUT)
    M5DC_ESC2.loop(OUT_LPF[1].update(DRIVER[MODE]->OUT[1]));
  else
    M5DC_PORT.putUsec(1,OUT_LPF[1].update(DRIVER[MODE]->OUT[1]));
  //DEBUG.println(MODE);DRIVER[MODE]->debug();
  
  // count frequency
  Driver::FREQ[0] = LOOP_HZ.getFreq();
  //Driver::FREQ[1] = CONF_HZ.getFreq();
  Driver::FREQ[2] = SBUS_HZ.getFreq();
  Driver::FREQ[3] = M5DC_PORT.getFreq(0);

  // setup
  M5.update();
  if (M5.Btn.wasPressed()) {
    M5DC_CONF.start();
    while (M5DC_CONF.isWake()) {
      CONF_HZ.touch();
      M5.update();
      if (M5.Btn.wasPressed()) M5DC_CONF.stop();
      M5DC_CONF.loop();
      
      // IMU
      M5DC_AHRS.loop(Driver::GYRO,Driver::ACCL,Driver::AHRS,&Driver::TEMP);
      
      // RCV
      if (SBUS_RX && SBUS_TX) {
        if (M5DC_SBUS.read(SBUS_Channel,&SBUS_FailSafe,&SBUS_LostFrame)) {
          for (int i=0; i<4; i++) Driver::IN[i] = SBUS2PWM(SBUS_Channel[i]);
        }
      } else {
        for (int i=0; i<4; i++) Driver::IN[i] = M5DC_PORT.getUsec(i);
      }

      // LED
      M5DC_FACE.loop((SBUS_FailSafe? CRGB::Red: CRGB::Aqua),RUN_MODE,Driver::IN);
  
      // DRV and OUT
      int MODE = 0;
      DRIVER[MODE]->loop();
      M5DC_PORT.putUsec(0,DRIVER[MODE]->OUT[0]);
      //M5DC_PORT.putUsec(1,DRIVER[MODE]->OUT[1]);  // for safety
      // count frequency
      Driver::FREQ[1] = CONF_HZ.getFreq();
    } 
    
    // IMU
    M5DC_AHRS.setup(1000,IMU_AXIS);
    
    // PWM
    M5DC_PORT.putFreq(0,CH1_FREQ);
    M5DC_PORT.putFreq(1,CH2_FREQ);
    
    // ESC
    M5DC_ESC1.setupUsec(CH2_MIN,CH2_MID,CH2_MAX,CH2_DEAD);
    M5DC_ESC1.setupMode(ESC_MODE,ESC_BRAKE,ESC_DRAG,(ESC_INV&0x1),ESC_PWM);
    M5DC_ESC2.setupUsec(CH2_MIN,CH2_MID,CH2_MAX,CH2_DEAD);
    M5DC_ESC2.setupMode(ESC_MODE,ESC_BRAKE,ESC_DRAG,(ESC_INV&0x2),ESC_PWM);
    
    // DRV
    for (int i=0; i<LENGTH(DRIVER); i++) DRIVER[i]->setup();
    
    // LPF
    IMU_LPF[0].setup(IMU_SPAN,0.0);
    IMU_LPF[1].setup(IMU_SPAN,0.0);
    IMU_LPF[2].setup(IMU_SPAN,1.0);
    OUT_LPF[0].setup(CH1_SPAN,CH1_MID);
    OUT_LPF[1].setup(CH2_SPAN,CH2_MID);
  };
  
}



////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
