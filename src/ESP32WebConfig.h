/*
 * ESP32WebConfig Library v1.0.0
 * Universal web configuration interface for ESP32/ESP8266
 * 
 * Features:
 * - Dual WiFi mode (AP + STA)
 * - Dynamic parameters with various data types
 * - Automatic EEPROM saving/loading
 * - Responsive web interface with real-time validation
 * - Configurable IP address
 * - mDNS support
 * - Backup/restore configuration
 * - Callback system for changes
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#ifndef ESP32WEBCONFIG_H
#define ESP32WEBCONFIG_H

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <DNSServer.h>
  #include <EEPROM.h>
  #include <ESP8266mDNS.h>
  #define WebServer ESP8266WebServer
  #define WiFi_Mode_t WiFiMode_t
  #define WIFI_AP_STA WIFI_AP_STA
  #define WIFI_AP WIFI_AP
#else
  #include <WiFi.h>
  #include <WebServer.h>
  #include <DNSServer.h>
  #include <EEPROM.h>
  #include <ESPmDNS.h>
#endif

#include <ArduinoJson.h>
#include <functional>
#include <vector>
#include <map>

// Library version
#define ESP32WEBCONFIG_VERSION "1.0.0"

// EEPROM configuration size
#define WEBCONFIG_EEPROM_SIZE 4096
#define WEBCONFIG_MAGIC_NUMBER 0xC0FFEE

// Supported parameter data types
enum ParameterType {
    PARAM_TEXT,         // Text input
    PARAM_PASSWORD,     // Password input (hidden)
    PARAM_NUMBER,       // Number input
    PARAM_FLOAT,        // Float input
    PARAM_CHECKBOX,     // Checkbox (true/false)
    PARAM_SELECT,       // Select dropdown
    PARAM_TEXTAREA,     // Larger text area
    PARAM_COLOR,        // Color picker
    PARAM_RANGE,        // Range slider
    PARAM_EMAIL,        // Email input
    PARAM_URL,          // URL input
    PARAM_TIME,         // Time input
    PARAM_DATE          // Date input
};

// Structure for storing parameters
struct ConfigParameter {
    String key;                    // Parameter key
    String label;                  // UI label
    String description;            // Description/help text
    ParameterType type;            // Parameter type
    String value;                  // Current value
    String defaultValue;           // Default value
    String validation;             // Regex validation
    String options;                // Options for SELECT (JSON array)
    String attributes;             // Additional HTML attributes
    int minValue;                  // Min value for NUMBER/RANGE
    int maxValue;                  // Max value for NUMBER/RANGE
    int step;                      // Step for NUMBER/RANGE
    bool required;                 // Required parameter
    bool readOnly;                 // Read-only parameter
    String group;                  // Parameter group
    int order;                     // Display order
};

// Callback types
typedef std::function<void(String key, String oldValue, String newValue)> OnParameterChangeCallback;
typedef std::function<void()> OnConfigSaveCallback;
typedef std::function<void()> OnConfigLoadCallback;
typedef std::function<String(String key, String value)> CustomValidatorCallback;

// Main library class
class ESP32WebConfig {
private:
    // Web server and DNS
    WebServer* server;
    DNSServer* dnsServer;
    
    // Configuration
    std::vector<ConfigParameter> parameters;
    String apSSID;
    String apPassword;
    IPAddress apIP;
    IPAddress apGateway; 
    IPAddress apSubnet;
    String deviceName;
    String adminUsername;
    String adminPassword;
    bool enableSTA;
    String staSSID;
    String staPassword;
    IPAddress staIP;
    bool useDHCP;
    
    // EEPROM
    int eepromSize;
    int eepromStartAddr;
    bool autoSave;
    
    // mDNS
    String mdnsName;
    bool enableMDNS;
    
    // Callbacks
    OnParameterChangeCallback onParameterChangeCallback;
    OnConfigSaveCallback onConfigSaveCallback;
    OnConfigLoadCallback onConfigLoadCallback;
    CustomValidatorCallback customValidator;
    
    // Internal state
    bool initialized;
    bool configChanged;
    unsigned long lastSave;
    int saveDelay;
    
    // HTML templates
    String getMainPageHTML();
    String getConfigPageHTML();
    String getNetworkPageHTML();
    String getAdvancedPageHTML();
    String getBackupPageHTML();
    String getCSS();
    String getJavaScript();
    
    // Utility functions
    String replacePlaceholders(String html);
    String generateParameterHTML(const ConfigParameter& param);
    String generateParameterGroupsHTML();
    String getGroupTitle(const String& groupName);
    String generateSelectOptions(const ConfigParameter& param);
    String getInputType(ParameterType type);
    String getParameterValue(const String& key);
    bool setParameterValue(const String& key, const String& value);
    bool validateParameter(const ConfigParameter& param, const String& value);
    
    // Web handlers
    void handleRoot();
    void handleConfig();
    void handleNetwork();
    void handleAdvanced();
    void handleBackup();
    void handleSave();
    void handleGet();
    void handleSet();
    void handleReset();
    void handleReboot();
    void handleExport();
    void handleImport();
    void handleNotFound();
    
    // Network functions
    void initAP();
    void initSTA();
    void initWebServer();
    void initMDNS();
    
    // EEPROM functions
    bool saveToEEPROM();
    bool loadFromEEPROM();
    void clearEEPROM();
    
    // Utility
    String getStatusJSON();
    String getConfigJSON();
    bool authenticate();
    IPAddress parseIP(const String& ip);
    String ipToString(IPAddress ip);

public:
    // Constructor
    ESP32WebConfig();
    ~ESP32WebConfig();
    
    // ===== BASIC CONFIGURATION =====
    void setDeviceName(const String& name);
    void setAPConfig(const String& ssid, const String& password = "", 
                    IPAddress ip = IPAddress(192, 168, 4, 1));
    void setSTAConfig(const String& ssid, const String& password = "", 
                     IPAddress ip = IPAddress(0, 0, 0, 0));
    void setCredentials(const String& username, const String& password);
    void setMDNS(const String& name);
    void setEEPROM(int size = WEBCONFIG_EEPROM_SIZE, int startAddr = 0);
    
    // ===== PARAMETERS =====
    // Basic parameter addition
    void addParameter(const String& key, const String& label, ParameterType type, 
                     const String& defaultValue = "");
    
    // Extended parameter addition
    void addParameter(const String& key, const String& label, ParameterType type,
                     const String& defaultValue, const String& description,
                     const String& validation = "", bool required = false);
    
    // Specific parameter types
    void addTextParameter(const String& key, const String& label, 
                         const String& defaultValue = "", const String& placeholder = "");
    void addPasswordParameter(const String& key, const String& label);
    void addNumberParameter(const String& key, const String& label, 
                           int defaultValue = 0, int min = 0, int max = 100, int step = 1);
    void addFloatParameter(const String& key, const String& label, 
                          float defaultValue = 0.0, float min = 0.0, float max = 100.0, float step = 0.1);
    void addCheckboxParameter(const String& key, const String& label, bool defaultValue = false);
    void addSelectParameter(const String& key, const String& label, 
                           const String& options, const String& defaultValue = "");
    void addRangeParameter(const String& key, const String& label, 
                          int defaultValue, int min, int max, int step = 1);
    void addTextareaParameter(const String& key, const String& label, 
                             const String& defaultValue = "", int rows = 3);
    
    // Advanced parameters
    void addParameterToGroup(const String& group, const String& key, const String& label, 
                            ParameterType type, const String& defaultValue = "");
    void setParameterAttributes(const String& key, const String& attributes);
    void setParameterOrder(const String& key, int order);
    void setParameterReadOnly(const String& key, bool readOnly = true);
    
    // ===== PARAMETER VALUES =====
    String getValue(const String& key);
    int getInt(const String& key);
    float getFloat(const String& key);
    bool getBool(const String& key);
    
    bool setValue(const String& key, const String& value);
    bool setValue(const String& key, int value);
    bool setValue(const String& key, float value);
    bool setValue(const String& key, bool value);
    
    // ===== CALLBACK SYSTEM =====
    void onParameterChange(OnParameterChangeCallback callback);
    void onConfigSave(OnConfigSaveCallback callback);
    void onConfigLoad(OnConfigLoadCallback callback);
    void setCustomValidator(CustomValidatorCallback callback);
    
    // ===== CONTROL =====
    void begin();
    void handle();
    
    // ===== CONFIGURATION =====
    bool save();
    bool load();
    void reset();
    void setDefaults();
    
    // Export/Import
    String exportConfig();
    bool importConfig(const String& json);
    
    // ===== INFORMATION =====
    bool isConfigChanged();
    String getVersion();
    String getDeviceName();
    bool isConnected();
    String getLocalIP();
    String getAPIP();
    int getParameterCount();
    std::vector<String> getParameterKeys();
    
    // Debug
    void printConfig();
    void enableDebug(bool enable = true);
};

#endif // ESP32WEBCONFIG_H