/*
 * ESP8266/ESP32C3 Minimal WebConfig Library
 * Ultra-lightweight configuration interface for Zonio ecosystem
 * Compatible with both ESP8266 and ESP32C3 super mini
 * 
 * Features:
 * - Minimal flash usage (~3KB total)
 * - 5-minute timeout AP mode
 * - Single HTML page with inline CSS/JS
 * - EEPROM configuration storage
 * - Compatible across all Zonio firmware versions
 */

#ifndef ZONIO_MINIMAL_WEBCONFIG_H
#define ZONIO_MINIMAL_WEBCONFIG_H

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <DNSServer.h>
  #include <EEPROM.h>
  #define WebServer ESP8266WebServer
  #define WiFi_Mode_t WiFiMode_t
  #define WIFI_AP_STA WIFI_AP_STA
  #define WIFI_AP WIFI_AP
#else
  #include <WiFi.h>
  #include <WebServer.h>
  #include <DNSServer.h>
  #include <EEPROM.h>
#endif

// Configuration constants
#define ZONIO_CONFIG_VERSION 1
#define ZONIO_MAGIC_NUMBER 0x5A4F4E49  // "ZONI" in hex
#define ZONIO_EEPROM_SIZE 512
#define ZONIO_MAX_PARAMS 8
#define ZONIO_AP_TIMEOUT 300000  // 5 minutes
#define ZONIO_PARAM_LEN 64

// Parameter types
enum ZonioParamType {
  ZONIO_TEXT,
  ZONIO_NUMBER,
  ZONIO_FLOAT,
  ZONIO_CHECKBOX,
  ZONIO_SELECT
};

// Configuration parameter structure
struct ZonioParam {
  char key[16];
  char label[32];
  char value[ZONIO_PARAM_LEN];
  char defaultValue[ZONIO_PARAM_LEN];
  ZonioParamType type;
  int minValue;
  int maxValue;
  char options[128];  // JSON array for select
};

// Main configuration class
class ZonioMinimalConfig {
private:
  WebServer* server;
  DNSServer* dnsServer;
  
  char deviceName[32];
  char apSSID[32];
  char apPassword[16];
  IPAddress apIP;
  
  ZonioParam params[ZONIO_MAX_PARAMS];
  int paramCount;
  
  unsigned long apStartTime;
  bool apActive;
  bool configMode;
  bool shouldRestart;
  
  // Minimal HTML template
  const char* getConfigPage();
  void handleRoot();
  void handleSave();
  void handleNotFound();
  
  // EEPROM functions
  void saveConfig();
  bool loadConfig();
  void clearConfig();
  
  // Utility functions
  bool isValidIP(const char* ip);
  void generateHTML(char* buffer, size_t bufferSize);
  
public:
  ZonioMinimalConfig();
  ~ZonioMinimalConfig();
  
  // Setup functions
  void setDeviceName(const char* name);
  void setAPCredentials(const char* ssid, const char* password);
  void setAPIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
  
  // Parameter management
  void addParameter(const char* key, const char* label, const char* defaultValue, ZonioParamType type = ZONIO_TEXT);
  void addNumberParameter(const char* key, const char* label, int defaultValue, int minVal = 0, int maxVal = 100);
  void addFloatParameter(const char* key, const char* label, float defaultValue);
  void addCheckboxParameter(const char* key, const char* label, bool defaultValue);
  void addSelectParameter(const char* key, const char* label, const char* options, const char* defaultValue);
  
  // Value getters
  const char* getString(const char* key);
  int getInt(const char* key);
  float getFloat(const char* key);
  bool getBool(const char* key);
  
  // Main functions
  void begin();
  void handle();
  bool isConfigMode() { return configMode; }
  bool shouldRestartDevice() { return shouldRestart; }
  void markRestartHandled() { shouldRestart = false; }
};

#endif // ZONIO_MINIMAL_WEBCONFIG_H