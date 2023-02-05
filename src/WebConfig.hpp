////////////////////////////////////////////////////////////////////////////////
// ドリフトRCカー用ジャイロシステムGyroM5Atom
// GyroM5Atom system for RC drift car
// https://github.com/hshin-git/GyroM5
////////////////////////////////////////////////////////////////////////////////
#ifndef _WEB_CONFIG_
#define _WEB_CONFIG_

// large char buff for html/json
char BUFF_OF_JSON[16*1024];


////////////////////////////////////////////////////////////////////////////////
// struct ParameterList {}: 設定パラメータの管理クラス
//  init(): パラメータの初期値
//  load(): パラメータの復元
//  save(): パラメータの保存
//  setup(): パラメータの初期化
//  set(): パラメータの更新
//  toJSON(): パラメータのJSON文字列
//  toHTML(): パラメータのHTML文字列
////////////////////////////////////////////////////////////////////////////////
#include <Preferences.h>
Preferences ParameterListStorage;
#define DEBUG Serial
//
#define PLIST_NAME  "ParameterList"
#define PLIST_SIZE 16	/* size of parameter list */
#define PTABL_SIZE 16	/* size of parameter table */
#define PTEXT_SIZE 16	/* size of parameter text */
//
#define PTYPE_VOID 0
#define PTYPE_IVAL 1
#define PTYPE_IREF 2
#define PTYPE_FVAL 3
#define PTYPE_FREF 4
#define PTYPE_TEXT 5
//
#define PLIST_MAGIC 123456789
#define PCONF_END  {PTYPE_VOID,{.ival=PLIST_MAGIC},NULL}
// integer: type=range
#define PCONF_RANGE(key,val,unit,min,max,step) \
  {PTYPE_IVAL,{.ival=val}, \
  "{\"name\":\"" key "\",\"value\":%d,\"unit\":\"" unit \
  "\",\"type\":\"range\",\"min\":" #min ",\"max\":" #max ",\"step\":" #step "}"}
// integer: reference
#define PCONF_IREFTO(key,val,unit,min,max) \
  {PTYPE_IREF,{.iref=val}, \
  "{\"name\":\"" key "\",\"value\":%d,\"unit\":\"" unit \
  "\",\"type\":\"range\",\"min\":" #min ",\"max\":" #max ",\"step\":1}"}
// float: reference
#define PCONF_FREFTO(key,val,unit,min,max) \
  {PTYPE_FREF,{.fref=val}, \
  "{\"name\":\"" key "\",\"value\":%.f,\"unit\":\"" unit \
  "\",\"type\":\"range\",\"min\":" #min ",\"max\":" #max ",\"step\":1}"}
// integer: type=select
#define PCONF_SELECT(key,val,unit,opts,labs)  \
  {PTYPE_IVAL,{.ival=val}, \
  "{\"name\":\"" key "\",\"value\":%d,\"unit\":\"" unit \
  "\",\"type\":\"select\",\"option\":" opts ",\"label\":" labs "}"}
// string: type=text
#define PCONF_TEXT(key,val,unit,min,max)  \
  {PTYPE_TEXT,{.text=val},  \
  "{\"name\":\"" key "\",\"value\":\"%s\",\"unit\":\"" unit \
  "\",\"type\":\"text\",\"minlength\":" #min ",\"maxlength\":" #max ",\"size\":16}"}
// string: type=color
#define PCONF_COLOR(key,val,unit)  \
  {PTYPE_TEXT,{.text=val},  \
  "{\"name\":\"" key "\",\"value\":\"%s\",\"unit\":\"" unit \
  "\",\"type\":\"color\"}"}
// integer: type=number
#define PCONF_NUMBER(key,val,unit,min,max) \
  {PTYPE_IVAL,{.ival=val}, \
  "{\"name\":\"" key "\",\"value\":%d,\"unit\":\"" unit \
  "\",\"type\":\"number\",\"min\":" #min ",\"max\":" #max "}"}
// integer: fetch
#define PCONF_FETCH(key,val,unit,group,index,min,max) \
  {PTYPE_IVAL,{.ival=val}, \
  "{\"name\":\"" key "\",\"value\":%d,\"unit\":\"" unit \
  "\",\"type\":\"fetch\",\"group\":\"" group "\",\"index\":" #index ",\"min\":" #min ",\"max\":" #max "}"}


// Body of Parrameter
typedef union {
  int ival;
  int *iref;
  float fval;
  float *fref;
  char text[PTEXT_SIZE];
} ParameterBody;

// Conf of Parameter
typedef struct {
  int type;
  ParameterBody body;
  char *json;
} ParameterConf;

// List of Parameters
class ParameterList {
public:
  ParameterBody body[PLIST_SIZE];
  ParameterConf conf[PLIST_SIZE];
  //
  void init() {
    //memcpy(body,conf,sizeof(body));
    for (int i=0; i<PLIST_SIZE; i++) body[i] = conf[i].body;
    //DEBUG.println("ParameterList.init");
  };
  //
  void save(char *key) {
    if (ParameterListStorage.begin(PLIST_NAME)) {
      //DEBUG.println("ParameterList.save " + String(key));
      ParameterListStorage.putBytes(key,body,sizeof(body));
      ParameterListStorage.end();
    }
  };
  //
  void load(char *key) {
    if (ParameterListStorage.begin(PLIST_NAME)) {
      //DEBUG.println("ParameterList.load " + String(key));
      ParameterListStorage.getBytes(key,body,sizeof(body));
      ParameterListStorage.end();
    }
  };
  //
  bool valid(void) {
    //DEBUG.println("ParameterList.valid");
    int i;
    for (i=0; i<PLIST_SIZE && conf[i].json; i++);
    return (body[i].ival == PLIST_MAGIC);
  }
  //
  void setup(char *key) {
    //DEBUG.println("ParameterList.setup " + String(key));
    load(key);
    if (!valid()) {
      init();
      save(key);
    }    
  }
  //
  int toJSON(char *s) {
    char *p = s;
    p += sprintf(p,"[");
    for (int i=0; i<PLIST_SIZE && conf[i].json; i++) {
      switch (conf[i].type) {
        case PTYPE_IVAL: p += sprintf(p,conf[i].json,body[i].ival); break;
        case PTYPE_FVAL: p += sprintf(p,conf[i].json,body[i].fval); break;
        case PTYPE_IREF: p += sprintf(p,conf[i].json,*conf[i].body.iref); break;
        case PTYPE_FREF: p += sprintf(p,conf[i].json,*conf[i].body.fref); break;
        case PTYPE_TEXT: p += sprintf(p,conf[i].json,body[i].text); break;
        default: break;
      }
      p += sprintf(p,",");
    }
    p--;
    p += sprintf(p,"]");
    return p-s;
  };
  //
  int toHTML(char *s) {
    char *p = s;
    p += sprintf(p,"<ol start=0>\n");
    for (int i=0; i<PLIST_SIZE && conf[i].json; i++) {
      p += sprintf(p,"<li>");
      switch (conf[i].type) {
        case PTYPE_IVAL: p += sprintf(p,conf[i].json,body[i].ival); break;
        case PTYPE_FVAL: p += sprintf(p,conf[i].json,body[i].fval); break;
        case PTYPE_IREF: p += sprintf(p,conf[i].json,*conf[i].body.iref); break;
        case PTYPE_FREF: p += sprintf(p,conf[i].json,*conf[i].body.fref); break;
        case PTYPE_TEXT: p += sprintf(p,conf[i].json,body[i].text); break;
        default: break;
      }
      p += sprintf(p,"\n");
    }
    //p--;
    p += sprintf(p,"</ol>\n");
    return p-s;
  };
  //
  void set(int i, const char *v) {
    //DEBUG.print("set "); Serial.print(i); Serial.print(" "); Serial.println(v);
    if (i>=0 && i<PLIST_SIZE && conf[i].json) {
      switch (conf[i].type) {
        case PTYPE_IVAL: body[i].ival = atoi(v); break;
        case PTYPE_FVAL: body[i].fval = atof(v); break;
        case PTYPE_IREF: *(conf[i].body.iref) = atoi(v); break;
        case PTYPE_FREF: *(conf[i].body.fref) = atof(v); break;
        case PTYPE_TEXT: strcpy(body[i].text,v); break;
        default: break;
      }
    }
  };
  //
  inline int getIval(int i) { return body[i].ival; };
  inline int getIref(int i) { return *body[i].iref; };
  inline float getFval(int i) { return body[i].fval; };
  inline float getFref(int i) { return *body[i].fref; };
  inline char* getText(int i) { return body[i].text; };
  
};



////////////////////////////////////////////////////////////////////////////////
// struct ParameterTable {}: 設定パラメータの管理クラス
//  setup(): パラメータリストの初期化
//  set(): パラメータリストの更新
//  get(): パラメータリストの取得
//  toJSON(): パラメータのJSON文字列
//  toHTML(): パラメータのHTML文字列
////////////////////////////////////////////////////////////////////////////////
// Table of Parameters
typedef struct {
  char *name;
  ParameterList *list;
} ParameterDict;

class ParameterTable {
public:
  ParameterDict table[PTABL_SIZE];
  //
  ParameterList *get(const char *key) {
    for (int i=0; i<PTABL_SIZE && table[i].name; i++) {
      if (strcmp(key,table[i].name) == 0) return table[i].list;
    }
    return NULL;
  }
  //
  void set(const char *key, int idx, const char *val) {
    ParameterList *lst = get(key);
    if (lst) lst->set(idx,val);
    DEBUG.print(key);DEBUG.print(".");DEBUG.print(idx);DEBUG.print("=");DEBUG.println(val);
  }
  //
  void save(const char *key = NULL) {
    for (int i=0; i<PTABL_SIZE && table[i].name; i++) {
      if (key == NULL || strcmp(key,table[i].name) == 0) table[i].list->save(table[i].name);
    }
  }
  //
  int toJSON(char *s, const char *key = NULL) {
    char *p = s;
    p += sprintf(p,"{");
    for (int i=0; i<PTABL_SIZE && table[i].name; i++) {
      //DEBUG.println(table[i].name);
      if (key == NULL || strcmp(key,table[i].name) == 0) {
        #if 0
        char json[1024];
        table[i].list->toJSON(json);
        p += sprintf(p,"\"%s\":%s,",table[i].name,json);
        #else
        p += sprintf(p,"\"%s\":",table[i].name);
        p += table[i].list->toJSON(p);
        p += sprintf(p,",");
        #endif
      }
    }
    p--;
    p += sprintf(p,"}");
    return p-s;
  }
  //
  int toHTML(char *s, const char *key = NULL) {
    char *p = s;
    p += sprintf(p,"<h1>saved parameters</h1>\n");
    p += sprintf(p,"<hr>\n");
    for (int i=0; i<PTABL_SIZE && table[i].name; i++) {
      if (key == NULL || strcmp(key,table[i].name) == 0) {
        p += sprintf(p,"<h2>%s</h2>\n",table[i].name);
        p += table[i].list->toHTML(p);
      }
    }
    //p--;
    p += sprintf(p,"<hr>\n");
    return p-s;
  }
  //
  void setup(const char *key = NULL) {
    //DEBUG.println("ParameterTable.setup");
    for (int i=0; i<PTABL_SIZE && table[i].name; i++) {
      if (key == NULL || strcmp(key,table[i].name) == 0) table[i].list->setup(table[i].name);
    }
  }
  //
  void debug(void) {
    toHTML(BUFF_OF_JSON);
    DEBUG.println(BUFF_OF_JSON);
  }
};



////////////////////////////////////////////////////////////////////////////////
// class WebConfig{}: WiFi/WWWサーバの管理クラス
//  setup(): サーバの初期化
//  start(): サーバの起動
//  loop(): サーバの処理
//  stop(): サーバの停止
//  isWake(): サーバの起動有無
////////////////////////////////////////////////////////////////////////////////
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

//#include "index.html.h"
//#include "vue.min.js.h"

#define WIFI_AP_SSID  "m5atom"


class WebConfig {
  //
  static WebServer server;
  static bool serverInit;
  static bool serverWake;
  //
  //static void handleRoot() { server.send(200, "text/html", index_html); }
  //static void handleJava() { server.send(200, "text/javascript", vue_min_js); }
  //
  static void sync() {
    for (int i=0; i<server.args(); i++) {
      String arg = server.argName(i);
      String val = server.arg(i);
      //
      int pos = arg.indexOf('.');
      String key = arg.substring(0,pos);
      String idx = arg.substring(pos+1);
      CONFIG.set(key.c_str(),idx.toInt(),val.c_str());
    }
    CONFIG.save();
  }
  //
  static void handleSave() {
    sync();
    server.send(200, "text/plain", "Save OK.");
  }
  //
  static void handleExit() {
    sync();
    CONFIG.toHTML(BUFF_OF_JSON);
    server.send(200, "text/html", BUFF_OF_JSON);
    delay(500);
    stop();
  }
  //
  static void handleJson() {
    CONFIG.toJSON(BUFF_OF_JSON);
    server.send(200, "application/json", BUFF_OF_JSON);
  }
  //
  static void handleAjax() {
    CONFIG.toJSON(BUFF_OF_JSON, "ajax");
    server.send(200, "application/json", BUFF_OF_JSON);
  }
  //
  static void handleNotFound() {
    server.send(404, "text/plain", "Not Found.");
  }
  //
public:
  //
  static ParameterTable CONFIG;
  //
  static void setup(void) {
    SPIFFS.begin();
    CONFIG.setup();
  }
  //
  static void start(void) {
  
    DEBUG.println("Setup WIFI AP mode");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID);
    WiFi.begin();
    delay(500);
    
    DEBUG.print("Started! IP address: ");
    DEBUG.println(WiFi.softAPIP());
  
    if (MDNS.begin(WIFI_AP_SSID)) {
      MDNS.addService("http", "tcp", 80);
      DEBUG.println("MDNS responder started");
      DEBUG.print("You can now connect to http://");
      DEBUG.print(WIFI_AP_SSID);
      DEBUG.println(".local");
    }
  
    if (!serverInit) {
      server.serveStatic("/",SPIFFS,"/index.html");
      server.serveStatic("/vue.min.js",SPIFFS,"/vue.min.js");
      //server.on("/", HTTP_GET, handleRoot);
      //server.on("/vue.min.js", HTTP_GET, handleJava);
      server.on("/ajax", HTTP_GET, handleAjax);
      server.on("/json", HTTP_GET, handleJson);
      server.on("/exit", HTTP_GET, handleExit);
      server.on("/save", HTTP_GET, handleSave);
      server.onNotFound(handleNotFound);
      server.begin();
      DEBUG.println("HTTP server started");
      serverInit = true;
    }
  
    serverWake = true;
  }
  static void loop(void) {
    if (serverWake) server.handleClient();
  }
  static void stop(void) {
    if (serverWake) {
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      serverWake = false;
      DEBUG.println("Stopped WIFI");
    }
  }
  static bool isWake(void) {
    return serverWake;
  }
  static IPAddress getIP(void) {
    return WiFi.softAPIP();
  }
  //
};
//
WebServer WebConfig::server(80);
bool WebConfig::serverInit = false;
bool WebConfig::serverWake = false;
//ParameterTable WebConfig::CONFIG = {0};


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
#endif
