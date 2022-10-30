#ifndef _LIB_IMU_
#define _LIB_IMU_
////////////////////////////////////////////////////////////////////////////////
// class LibIMU{}: 姿勢推定用ライブラリ（Mahonyフィルタ、可変更新周期、座標変換に対応）
//  setup(): AHRSの初期化
//  loop(): AHRSの更新
//  initAXIS(): 座標軸の変更（シャーシ固定系の変更）
//  initMEAN(): バイアスの更新（センサのキャリブレーション）
////////////////////////////////////////////////////////////////////////////////
#define DEBUG	Serial

class LibIMU {
  /* AHRS */
  //#include <utility/MahonyAHRS.h>
  //---------------------------------------------------------------------------------------------------
  // Definitions
  
  //#define sampleFreq  25.0f     // sample frequency in Hz
  float sampleFreq = 25.0;
  #define twoKpDef  (2.0f * 1.0f) // 2 * proportional gain
  #define twoKiDef  (2.0f * 0.0f) // 2 * integral gain
  
  //#define twoKiDef  (0.0f * 0.0f)
  
  //---------------------------------------------------------------------------------------------------
  // Variable definitions
  
  volatile float twoKp = twoKpDef;                      // 2 * proportional gain (Kp)
  volatile float twoKi = twoKiDef;                      // 2 * integral gain (Ki)
  volatile float q0 = 1.0, q1 = 0.0, q2 = 0.0, q3 = 0.0;          // quaternion of sensor frame relative to auxiliary frame
  volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f; // integral error terms scaled by Ki

  //---------------------------------------------------------------------------------------------------
  // IMU algorithm update
  
  void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az,float *pitch,float *roll,float *yaw) {
    float recipNorm;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float qa, qb, qc;
  
  
    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
  
      // Normalise accelerometer measurement
      recipNorm = invSqrt(ax * ax + ay * ay + az * az);
      ax *= recipNorm;
      ay *= recipNorm;
      az *= recipNorm;
  
      // Estimated direction of gravity and vector perpendicular to magnetic flux
      halfvx = q1 * q3 - q0 * q2;
      halfvy = q0 * q1 + q2 * q3;
      halfvz = q0 * q0 - 0.5f + q3 * q3;
  
      
  
      // Error is sum of cross product between estimated and measured direction of gravity
      halfex = (ay * halfvz - az * halfvy);
      halfey = (az * halfvx - ax * halfvz);
      halfez = (ax * halfvy - ay * halfvx);
  
      // Compute and apply integral feedback if enabled
      if(twoKi > 0.0f) {
        integralFBx += twoKi * halfex * (1.0f / sampleFreq);  // integral error scaled by Ki
        integralFBy += twoKi * halfey * (1.0f / sampleFreq);
        integralFBz += twoKi * halfez * (1.0f / sampleFreq);
        gx += integralFBx;  // apply integral feedback
        gy += integralFBy;
        gz += integralFBz;
      }
      else {
        integralFBx = 0.0f; // prevent integral windup
        integralFBy = 0.0f;
        integralFBz = 0.0f;
      }
  
      // Apply proportional feedback
      gx += twoKp * halfex;
      gy += twoKp * halfey;
      gz += twoKp * halfez;
    }
  
    // Integrate rate of change of quaternion
    gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
    gy *= (0.5f * (1.0f / sampleFreq));
    gz *= (0.5f * (1.0f / sampleFreq));
    qa = q0;
    qb = q1;
    qc = q2;
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);
  
    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
  
  
    *pitch = asin(-2 * q1 * q3 + 2 * q0* q2); // pitch
    *roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1); // roll
    *yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3);  //yaw
  
    *pitch *= RAD_TO_DEG;
      *yaw   *= RAD_TO_DEG;
      // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
      //  8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
      // - http://www.ngdc.noaa.gov/geomag-web/#declination
      *yaw   -= 8.5;
      *roll  *= RAD_TO_DEG;
  
    ///Serial.printf("%f    %f    %f \r\n",  pitch, roll, yaw);
  }
  
  //---------------------------------------------------------------------------------------------------
  // Fast inverse square-root
  // See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
  
  float invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
  #pragma GCC diagnostic warning "-Wstrict-aliasing"
    y = y * (1.5f - (halfx * y * y));
    return y;
  }

  
  /* IMU values */
  float accl[3] = {0.0,0.0,0.0};
  float gyro[3] = {0.0,0.0,0.0};
  /* IMU mean values */
  float ACCL[3] = {0.0,0.0,0.0};
  float GYRO[3] = {0.0,0.0,0.0};
  /* IMU temp */
  float temp = 0.0F;
  /* AHRS */
  float pitch = 0.0F;
  float roll = 0.0F;
  float yaw = 0.0F;
  
  /* Time */
  uint32_t Now = 0;
  uint32_t lastUpdate = 0;
  float deltat = 1.0f;
  
  /* Axis = Body Fixed Frame */
  float X[3] = {1.0,0.0,0.0};
  float Y[3] = {0.0,1.0,0.0};
  float Z[3] = {0.0,0.0,1.0};

  /* LPF */
  //FilterLP LPF[6];
  
  /* Vetor Operations */
  float dot(float* a,float* b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }
  float norm(float* a) { return sqrt(dot(a,a)); }
  void mul(float* a,float k,float* b) {
    b[0] = k*a[0];
    b[1] = k*a[1];
    b[2] = k*a[2];
  }
  void add(float* a,float* b,float* c) {
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
  }
  void sub(float* a,float* b,float* c) {
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
  }
  void dup(float* a,float* b) {
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
  }
  void normalize(float* a) { 
    float na = norm(a);
    a[0] /= na;
    a[1] /= na;
    a[2] /= na;
  }
  void cross(float* a,float* b, float* c) {
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
  }

public:
  /* initialize */
  void initMEAN(int msec = 1000) {
    for (int i=0; i<3; i++) {
      ACCL[i] = 0.0F;
      GYRO[i] = 0.0F;
    }
    
    int N = 0;
    unsigned long int timeout = millis() + msec;
    while (millis() < timeout) {
      M5.IMU.getGyroData(&gyro[0],&gyro[1],&gyro[2]);
      M5.IMU.getAccelData(&accl[0],&accl[1],&accl[2]);
      for (int i=0; i<3; i++) {
        ACCL[i] += accl[i];
        GYRO[i] += gyro[i];
      }
      N++;
      delay(1);
    }
  
    for (int i=0; i<3; i++) {
      ACCL[i] /= N;
      GYRO[i] /= N;
    }
  }
  
  void initAXIS(int xdir = 1) {
    // initial X0
    int xabs = abs(xdir);
    if (xabs>=1 && xabs<=3) {
      X[0] = (xabs==1? xdir/xabs: 0);
      X[1] = (xabs==2? xdir/xabs: 0);
      X[2] = (xabs==3? xdir/xabs: 0);
    }
    
    // Z := Anti Gravity
    dup(ACCL,Z);
    normalize(Z);
  
    // X := X0 - (Z,X0)Z
    float zx = dot(Z,X);
    float zxZ[3];
    mul(Z,zx,zxZ);
    sub(X,zxZ,X);
    normalize(X);
  
    // Y := Z x X
    cross(Z,X,Y);
    normalize(Y);
  }
  
  void setup(int msec = 1000, int xdir = 1) {
    M5.IMU.Init();
    initMEAN(msec);
    initAXIS(xdir);
  }
  
  void loop(float *gyro_=NULL, float *accl_=NULL, float *ahrs_=NULL, float *temp_=NULL) {
    // put your main code here, to run repeatedly:
    M5.IMU.getGyroData(&gyro[0], &gyro[1], &gyro[2]);
    M5.IMU.getAccelData(&accl[0], &accl[1], &accl[2]);
    M5.IMU.getTempData(&temp);

    // remove bias
    for (int i=0; i<3; i++) gyro[i] -= GYRO[i];
    
    // time update
    Now = millis();
    deltat = ((Now - lastUpdate) / 1000.0);
    lastUpdate = Now;
    sampleFreq = 1.0/deltat;

    //M5.IMU.getAhrsData(&pitch,&roll,&yaw);
    MahonyAHRSupdateIMU(dot(gyro,X)*DEG_TO_RAD,dot(gyro,Y)*DEG_TO_RAD,dot(gyro,Z)*DEG_TO_RAD, dot(accl,X),dot(accl,Y),dot(accl,Z), &pitch,&roll,&yaw);

    // copy results
    if (gyro_) {
      gyro_[0] = dot(gyro,X);
      gyro_[1] = dot(gyro,Y);
      gyro_[2] = dot(gyro,Z);
    }
    if (accl_) {
      accl_[0] = dot(accl,X);
      accl_[1] = dot(accl,Y);
      accl_[2] = dot(accl,Z);
    }
    if (ahrs_) {
      ahrs_[0] = roll;
      ahrs_[1] = pitch;
      ahrs_[2] = yaw;
    }
    if (temp_) {
      *temp_ = temp;
    }
  }
  void debug() {
    DEBUG.println("M5StackAHRS:");
    DEBUG.printf(" gyro=(%.2f,%.2f,%.2f)\n",gyro[0],gyro[1],gyro[2]);
    DEBUG.printf(" accl=(%.2f,%.2f,%.2f)\n",accl[0],accl[1],accl[2]);
    DEBUG.printf(" ahrs=(%.2f,%.2f,%.2f)\n",roll,pitch,yaw);
    DEBUG.printf(" GYRO=(%.2f,%.2f,%.2f)\n",GYRO[0],GYRO[1],GYRO[2]);
    DEBUG.printf(" ACCL=(%.2f,%.2f,%.2f)\n",ACCL[0],ACCL[1],ACCL[2]);
    DEBUG.printf(" X=(%.2f,%.2f,%.2f)\n",X[0],X[1],X[2]);
    DEBUG.printf(" Y=(%.2f,%.2f,%.2f)\n",Y[0],Y[1],Y[2]);
    DEBUG.printf(" Z=(%.2f,%.2f,%.2f)\n",Z[0],Z[1],Z[2]);   
  }
  
  int getFreq(void) { return int(1.0/deltat); }
#if 0
  float getAccT(void) { return LPF[0].update(dot(accl,X)); }
  float getAccL(void) { return LPF[1].update(dot(accl,Y)); }
  float getAccV(void) { return dot(accl,Z); }
  float getYawRate(void) { return LPF[2].update(dot(gyro,Z)); }
  float getRoll(void) {
    float Roll = - atan2(dot(accl,X),dot(accl,Z));
    return LPF[3].update(Roll * (180.0/PI));
  }
  float getPitch(void) {
    float Pitch = atan2(dot(accl,Y),dot(accl,Z));
    return LPF[4].update(Pitch * (180.0/PI));
  }
  float getTraction(float G0, float y0=0.1) {
    float x = getAccT()/G0;
    return fabs(x)>=1.0? y0: sqrt(1.-x*x); 
  }
#endif

};


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif
