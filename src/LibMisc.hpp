#ifndef _LIB_MISC_H_
#define _LIB_MISC_H_
////////////////////////////////////////////////////////////////////////////////
// class UserTask{}: タスク管理用ライブラリ（実行スレッドの制御）
//  setup(): タスク開始
//  suspend(): タスク中断
//  resume(): タスク再開
//  destroy(): タスク終了
//  getFreq(): ループ周期の取得[Hz]
////////////////////////////////////////////////////////////////////////////////
typedef struct {
  TaskHandle_t handle;
  void (*func)(void);
  int msec;
  bool flag;
  char name[8];
  uint32_t last, delta;
} TaskInfo;

class UserTask {
  #define MAX_TASK  4
  int task_id;
  static int task_count;
  static TaskInfo task_info[MAX_TASK];
  /* */
  static void taskLoop(void *args) {
    TaskInfo *info = &task_info[(int)args];
    while (true) {
      info->delta = micros() - info->last;
      info->last = micros();
      if (info->flag) (*(info->func))();
      delay(info->msec);
    }
  }
public:
  void setup(void (*func)(void), int msec=1, int prio=0, int stack=4096, int core=1) {
    if (task_count>=MAX_TASK) return;
    task_id = task_count++;
    TaskInfo *info = &task_info[task_id];
    info->func = func;
    info->msec = msec > 1? msec: 1;
    info->flag = true;
    info->last = 0;
    info->delta = 1;
    sprintf(info->name,"task%d",task_id);
    xTaskCreatePinnedToCore(taskLoop, info->name, stack, (void*)task_id, prio, &info->handle, core);
  }
  void suspend(void) {
    TaskInfo *info = &task_info[task_id];
    info->flag = false;
    delay(info->msec);
  }
  void resume(void) {
    TaskInfo *info = &task_info[task_id];
    info->flag = true;
    delay(info->msec);
  }
  void destroy(void) {
    TaskInfo *info = &task_info[task_id];
    vTaskDelete(info->handle);
  }
  int getFreq(void) {
    TaskInfo *info = &task_info[task_id];
    return info->flag? 1000000L/info->delta: 0;
  }
};
int UserTask::task_count = 0;
TaskInfo UserTask::task_info[MAX_TASK];


////////////////////////////////////////////////////////////////////////////////
// class TimerMS{}: タイマ管理用ライブラリ
//  isUp(): 指定時間の経過有無（タイマ更新あり）
//  getFreq(): タイマ周期の参照[Hz]
//  getDelta(): タイマ経過の参照[msec]
//  touch(): タイマ更新（ウォッチドッグタイマ等で利用）
//  isOld(): 指定時間の経過有無（タイマ更新なし）
////////////////////////////////////////////////////////////////////////////////
class TimerMS {
  unsigned long last;
  int freq;
public:
  TimerMS() {
    last = 0;
    freq = 1;
  }

  bool isUp(int msec) {
    unsigned long now = millis();
    if (last + msec <= now) {
      freq = 1000 / (now - last); 
      last = now;
      return true;
    }
    return false;
  }
  int getFreq(void) {
    return freq;
  }
  int getDelta(void) {
    return millis() - last; 
  }
  void touch(void) {
    last = millis();
  }
  bool isOld(int msec) {
    unsigned long now = millis();
    return last + msec <= now;
  }
};


////////////////////////////////////////////////////////////////////////////////
// class CountHz{}: 周期計測用ライブラリ
//  getFreq(): 周期の取得[Hz]
//  touch(): カウント（例：ループ内で呼ぶ）
////////////////////////////////////////////////////////////////////////////////
class CountHZ {
  unsigned long last;
  int freq, loop;
public:
  CountHZ() {
    last = 0;
    freq = 0;
    loop = 0;
  }
  void touch(void) {
    unsigned long now = millis();
    loop++;
    if (last + 1000 <= now) {
      last = now;
      freq = loop;
      loop = 0;
    }
  }
  int getFreq(void) {
    unsigned long now = millis();
    return last + 1100 < now? 0: freq;
  }
};


////////////////////////////////////////////////////////////////////////////////
// class FilterLP{}: 簡易ローパスフィルタ
//  update(): データ更新
////////////////////////////////////////////////////////////////////////////////
class FilterLP {
  float alpha, yp;
public:
  FilterLP() { setup(); }
  void setup_(float _alpha = 0.5, float _yp = 0.0) {
    alpha = _alpha;
    yp = _yp;
  }
  void setup(int span = 1, float _yp = 0.0) {
    if (span <= 0) span = 1;
    setup_(2.0/(1 + span),_yp);
  }
  float update(float x) {
    float y = alpha*x + (1.0-alpha)*yp;
    yp = y;
    return y;
  }
};


////////////////////////////////////////////////////////////////////////////////
// class FilterHP{}: 簡易ハイパスフィルタ
//  update(): データ更新
////////////////////////////////////////////////////////////////////////////////
class FilterHP {
  float alpha, yp, xp;
public:
  FilterHP() { setup(); }
  void setup_(float _alpha = 0.5, float _yp = 0.0) {
    alpha = _alpha;
    yp = _yp;
    xp = _yp;
  }
  void setup(int span = 1, float _yp = 0.0) {
    if (span <= 0) span = 1;
    setup_(2.0/(1 + span),_yp);
  }
  float update(float x) {
    float y = alpha*(x - xp) + alpha*yp;
    yp = y;
    xp = x;
    return y;
  }
};


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif
