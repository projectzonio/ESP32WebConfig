/*
 * ESP32WebConfig Library v1.0.0 - Implementation
 * Universal web configuration interface for ESP32/ESP8266
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#include "ESP32WebConfig.h"
#include "WebPages.h"

// ===== CONSTRUCTOR AND DESTRUCTOR =====

ESP32WebConfig::ESP32WebConfig() {
    server = nullptr;
    dnsServer = nullptr;
    initialized = false;
    configChanged = false;
    lastSave = 0;
    saveDelay = 5000; // 5 seconds delay for auto-save
    eepromSize = WEBCONFIG_EEPROM_SIZE;
    eepromStartAddr = 0;
    autoSave = true;
    enableSTA = false;
    useDHCP = true;
    enableMDNS = true;
    
    // Default AP configuration
    apSSID = "ESP32-Config";
    apPassword = "12345678";
    apIP = IPAddress(192, 168, 4, 1);
    apGateway = IPAddress(192, 168, 4, 1);
    apSubnet = IPAddress(255, 255, 255, 0);
    
    // Default admin credentials
    adminUsername = "admin";
    adminPassword = "admin";
    
    // Default device name
    deviceName = "ESP32-Device";
    mdnsName = "esp32-config";
}

ESP32WebConfig::~ESP32WebConfig() {
    if (server) delete server;
    if (dnsServer) delete dnsServer;
}

// ===== BASIC CONFIGURATION =====

void ESP32WebConfig::setDeviceName(const String& name) {
    deviceName = name;
    if (mdnsName == "esp32-config") {  // If mDNS wasn't changed
        mdnsName = name;
        mdnsName.toLowerCase();
        mdnsName.replace(" ", "-");
    }
}

void ESP32WebConfig::setAPConfig(const String& ssid, const String& password, IPAddress ip) {
    apSSID = ssid;
    apPassword = password;
    apIP = ip;
    
    // Automatic gateway and subnet setup
    apGateway = ip;
    apSubnet = IPAddress(255, 255, 255, 0);
}

void ESP32WebConfig::setSTAConfig(const String& ssid, const String& password, IPAddress ip) {
    enableSTA = true;
    staSSID = ssid;
    staPassword = password;
    staIP = ip;
    useDHCP = (ip == IPAddress(0, 0, 0, 0));
}

void ESP32WebConfig::setCredentials(const String& username, const String& password) {
    adminUsername = username;
    adminPassword = password;
}

void ESP32WebConfig::setMDNS(const String& name) {
    mdnsName = name;
    mdnsName.toLowerCase();
    mdnsName.replace(" ", "-");
    enableMDNS = true;
}

void ESP32WebConfig::setEEPROM(int size, int startAddr) {
    eepromSize = size;
    eepromStartAddr = startAddr;
}

// ===== PARAMETERS - BASIC ADDITION =====

void ESP32WebConfig::addParameter(const String& key, const String& label, ParameterType type, const String& defaultValue) {
    ConfigParameter param;
    param.key = key;
    param.label = label;
    param.type = type;
    param.value = defaultValue;
    param.defaultValue = defaultValue;
    param.required = false;
    param.readOnly = false;
    param.order = parameters.size();
    param.group = "general";
    param.minValue = 0;
    param.maxValue = 100;
    param.step = 1;
    
    parameters.push_back(param);
}

void ESP32WebConfig::addParameter(const String& key, const String& label, ParameterType type,
                                 const String& defaultValue, const String& description,
                                 const String& validation, bool required) {
    addParameter(key, label, type, defaultValue);
    
    // Find just added parameter and modify it
    for (auto& param : parameters) {
        if (param.key == key) {
            param.description = description;
            param.validation = validation;
            param.required = required;
            break;
        }
    }
}

// ===== SPECIALIZED PARAMETERS =====

void ESP32WebConfig::addTextParameter(const String& key, const String& label, 
                                     const String& defaultValue, const String& placeholder) {
    addParameter(key, label, PARAM_TEXT, defaultValue);
    if (!placeholder.isEmpty()) {
        setParameterAttributes(key, "placeholder='" + placeholder + "'");
    }
}

void ESP32WebConfig::addPasswordParameter(const String& key, const String& label) {
    addParameter(key, label, PARAM_PASSWORD, "");
}

void ESP32WebConfig::addNumberParameter(const String& key, const String& label, 
                                       int defaultValue, int min, int max, int step) {
    addParameter(key, label, PARAM_NUMBER, String(defaultValue));
    for (auto& param : parameters) {
        if (param.key == key) {
            param.minValue = min;
            param.maxValue = max;
            param.step = step;
            break;
        }
    }
}

void ESP32WebConfig::addFloatParameter(const String& key, const String& label, 
                                      float defaultValue, float min, float max, float step) {
    addParameter(key, label, PARAM_FLOAT, String(defaultValue, 2));
    setParameterAttributes(key, "min='" + String(min, 2) + "' max='" + String(max, 2) + "' step='" + String(step, 2) + "'");
}

void ESP32WebConfig::addCheckboxParameter(const String& key, const String& label, bool defaultValue) {
    addParameter(key, label, PARAM_CHECKBOX, defaultValue ? "true" : "false");
}

void ESP32WebConfig::addSelectParameter(const String& key, const String& label, 
                                       const String& options, const String& defaultValue) {
    addParameter(key, label, PARAM_SELECT, defaultValue);
    setParameterAttributes(key, "data-options='" + options + "'");
}

void ESP32WebConfig::addRangeParameter(const String& key, const String& label, 
                                      int defaultValue, int min, int max, int step) {
    addParameter(key, label, PARAM_RANGE, String(defaultValue));
    for (auto& param : parameters) {
        if (param.key == key) {
            param.minValue = min;
            param.maxValue = max;
            param.step = step;
            break;
        }
    }
}

void ESP32WebConfig::addTextareaParameter(const String& key, const String& label, 
                                         const String& defaultValue, int rows) {
    addParameter(key, label, PARAM_TEXTAREA, defaultValue);
    setParameterAttributes(key, "rows='" + String(rows) + "'");
}

// ===== ADVANCED PARAMETERS =====

void ESP32WebConfig::addParameterToGroup(const String& group, const String& key, const String& label, 
                                        ParameterType type, const String& defaultValue) {
    addParameter(key, label, type, defaultValue);
    for (auto& param : parameters) {
        if (param.key == key) {
            param.group = group;
            break;
        }
    }
}

void ESP32WebConfig::setParameterAttributes(const String& key, const String& attributes) {
    for (auto& param : parameters) {
        if (param.key == key) {
            param.attributes = attributes;
            break;
        }
    }
}

void ESP32WebConfig::setParameterOrder(const String& key, int order) {
    for (auto& param : parameters) {
        if (param.key == key) {
            param.order = order;
            break;
        }
    }
}

void ESP32WebConfig::setParameterReadOnly(const String& key, bool readOnly) {
    for (auto& param : parameters) {
        if (param.key == key) {
            param.readOnly = readOnly;
            break;
        }
    }
}

// ===== PARAMETER VALUES =====

String ESP32WebConfig::getValue(const String& key) {
    return getParameterValue(key);
}

int ESP32WebConfig::getInt(const String& key) {
    return getParameterValue(key).toInt();
}

float ESP32WebConfig::getFloat(const String& key) {
    return getParameterValue(key).toFloat();
}

bool ESP32WebConfig::getBool(const String& key) {
    String value = getParameterValue(key);
    return (value == "true" || value == "1" || value == "on");
}

bool ESP32WebConfig::setValue(const String& key, const String& value) {
    return setParameterValue(key, value);
}

bool ESP32WebConfig::setValue(const String& key, int value) {
    return setParameterValue(key, String(value));
}

bool ESP32WebConfig::setValue(const String& key, float value) {
    return setParameterValue(key, String(value, 2));
}

bool ESP32WebConfig::setValue(const String& key, bool value) {
    return setParameterValue(key, value ? "true" : "false");
}

// ===== CALLBACK SYSTEM =====

void ESP32WebConfig::onParameterChange(OnParameterChangeCallback callback) {
    onParameterChangeCallback = callback;
}

void ESP32WebConfig::onConfigSave(OnConfigSaveCallback callback) {
    onConfigSaveCallback = callback;
}

void ESP32WebConfig::onConfigLoad(OnConfigLoadCallback callback) {
    onConfigLoadCallback = callback;
}

void ESP32WebConfig::setCustomValidator(CustomValidatorCallback callback) {
    customValidator = callback;
}

// ===== CONTROL =====

void ESP32WebConfig::begin() {
    Serial.println("ESP32WebConfig: Initializing...");
    
    // Initialize EEPROM
    #ifdef ESP8266
    EEPROM.begin(eepromSize);
    #else
    EEPROM.begin(eepromSize);
    #endif
    
    // Load configuration
    load();
    
    // Initialize WiFi
    if (enableSTA) {
        initSTA();
    }
    initAP();
    
    // Initialize web server
    initWebServer();
    
    // Initialize mDNS
    if (enableMDNS) {
        initMDNS();
    }
    
    initialized = true;
    
    Serial.println("ESP32WebConfig: Initialization complete");
    Serial.printf("AP: %s (IP: %s)\n", apSSID.c_str(), apIP.toString().c_str());
    if (enableSTA) {
        Serial.printf("STA: %s\n", staSSID.c_str());
    }
    if (enableMDNS) {
        Serial.printf("mDNS: http://%s.local\n", mdnsName.c_str());
    }
}

void ESP32WebConfig::handle() {
    if (!initialized) return;
    
    if (server) server->handleClient();
    if (dnsServer) dnsServer->processNextRequest();
    
    // Auto-save if needed
    if (autoSave && configChanged && (millis() - lastSave > saveDelay)) {
        save();
    }
}

// ===== CONFIGURATION =====

bool ESP32WebConfig::save() {
    bool success = saveToEEPROM();
    if (success) {
        configChanged = false;
        lastSave = millis();
        Serial.println("ESP32WebConfig: Configuration saved");
        
        if (onConfigSaveCallback) {
            onConfigSaveCallback();
        }
    }
    return success;
}

bool ESP32WebConfig::load() {
    bool success = loadFromEEPROM();
    if (success) {
        Serial.println("ESP32WebConfig: Configuration loaded");
        
        if (onConfigLoadCallback) {
            onConfigLoadCallback();
        }
    } else {
        Serial.println("ESP32WebConfig: Using default configuration");
        setDefaults();
    }
    return success;
}

void ESP32WebConfig::reset() {
    Serial.println("ESP32WebConfig: Resetting configuration");
    clearEEPROM();
    setDefaults();
    save();
}

void ESP32WebConfig::setDefaults() {
    for (auto& param : parameters) {
        param.value = param.defaultValue;
    }
    configChanged = true;
}

// ===== EXPORT/IMPORT =====

String ESP32WebConfig::exportConfig() {
    JsonDocument doc;
    doc["device"] = deviceName;
    doc["version"] = ESP32WEBCONFIG_VERSION;
    doc["timestamp"] = millis();
    
    JsonObject config = doc["config"];
    for (const auto& param : parameters) {
        config[param.key] = param.value;
    }
    
    String output;
    serializeJson(doc, output);
    return output;
}

bool ESP32WebConfig::importConfig(const String& json) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {
        Serial.printf("ESP32WebConfig: JSON parsing error: %s\n", error.c_str());
        return false;
    }
    
    JsonObject config = doc["config"];
    if (config.isNull()) {
        Serial.println("ESP32WebConfig: Invalid configuration format");
        return false;
    }
    
    // Import values
    for (JsonPair kv : config) {
        setParameterValue(kv.key().c_str(), kv.value().as<String>());
    }
    
    configChanged = true;
    Serial.println("ESP32WebConfig: Configuration imported");
    return true;
}

// ===== INFORMATION =====

bool ESP32WebConfig::isConfigChanged() {
    return configChanged;
}

String ESP32WebConfig::getVersion() {
    return ESP32WEBCONFIG_VERSION;
}

String ESP32WebConfig::getDeviceName() {
    return deviceName;
}

bool ESP32WebConfig::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String ESP32WebConfig::getLocalIP() {
    if (isConnected()) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

String ESP32WebConfig::getAPIP() {
    return WiFi.softAPIP().toString();
}

int ESP32WebConfig::getParameterCount() {
    return parameters.size();
}

std::vector<String> ESP32WebConfig::getParameterKeys() {
    std::vector<String> keys;
    for (const auto& param : parameters) {
        keys.push_back(param.key);
    }
    return keys;
}

void ESP32WebConfig::printConfig() {
    Serial.println("=== ESP32WebConfig - Current Configuration ===");
    for (const auto& param : parameters) {
        Serial.printf("%s = %s\n", param.key.c_str(), param.value.c_str());
    }
    Serial.println("============================================");
}

// ===== UTILITY FUNCTIONS =====

String ESP32WebConfig::getParameterValue(const String& key) {
    for (const auto& param : parameters) {
        if (param.key == key) {
            return param.value;
        }
    }
    return "";
}

bool ESP32WebConfig::setParameterValue(const String& key, const String& value) {
    for (auto& param : parameters) {
        if (param.key == key) {
            String oldValue = param.value;
            
            // Validation
            if (!validateParameter(param, value)) {
                return false;
            }
            
            param.value = value;
            configChanged = true;
            lastSave = millis();
            
            // Callback
            if (onParameterChangeCallback) {
                onParameterChangeCallback(key, oldValue, value);
            }
            
            return true;
        }
    }
    return false;
}

bool ESP32WebConfig::validateParameter(const ConfigParameter& param, const String& value) {
    // Basic validation by type
    switch (param.type) {
        case PARAM_NUMBER:
        case PARAM_RANGE: {
            int val = value.toInt();
            if (val < param.minValue || val > param.maxValue) {
                return false;
            }
            break;
        }
        case PARAM_FLOAT: {
            float val = value.toFloat();
            // For float we use attributes for min/max
            break;
        }
        case PARAM_EMAIL: {
            if (value.indexOf('@') == -1 || value.indexOf('.') == -1) {
                return false;
            }
            break;
        }
    }
    
    // Required field
    if (param.required && value.isEmpty()) {
        return false;
    }
    
    // Custom validator
    if (customValidator) {
        String error = customValidator(param.key, value);
        if (!error.isEmpty()) {
            Serial.printf("Validation error for %s: %s\n", param.key.c_str(), error.c_str());
            return false;
        }
    }
    
    return true;
}

// ===== NETWORK FUNCTIONS =====

void ESP32WebConfig::initAP() {
    #ifdef ESP8266
    WiFi.mode(enableSTA ? WIFI_AP_STA : WIFI_AP);
    #else
    WiFi.mode(enableSTA ? WIFI_AP_STA : WIFI_AP);
    #endif
    WiFi.softAPConfig(apIP, apGateway, apSubnet);
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    
    Serial.printf("AP started: %s (IP: %s)\n", apSSID.c_str(), WiFi.softAPIP().toString().c_str());
}

void ESP32WebConfig::initSTA() {
    if (useDHCP) {
        WiFi.begin(staSSID.c_str(), staPassword.c_str());
    } else {
        WiFi.config(staIP, apGateway, apSubnet);
        WiFi.begin(staSSID.c_str(), staPassword.c_str());
    }
    
    Serial.printf("Connecting to WiFi: %s\n", staSSID.c_str());
}

void ESP32WebConfig::initWebServer() {
    server = new WebServer(80);
    dnsServer = new DNSServer();
    
    // Setup handlers
    server->on("/", [this]() { handleRoot(); });
    server->on("/config", [this]() { handleConfig(); });
    server->on("/network", [this]() { handleNetwork(); });
    server->on("/advanced", [this]() { handleAdvanced(); });
    server->on("/backup", [this]() { handleBackup(); });
    server->on("/save", HTTP_POST, [this]() { handleSave(); });
    server->on("/get", [this]() { handleGet(); });
    server->on("/set", HTTP_POST, [this]() { handleSet(); });
    server->on("/reset", [this]() { handleReset(); });
    server->on("/reboot", [this]() { handleReboot(); });
    server->on("/export", [this]() { handleExport(); });
    server->on("/import", HTTP_POST, [this]() { handleImport(); });
    server->on("/api/status", [this]() { 
        server->send(200, "application/json", getStatusJSON()); 
    });
    server->onNotFound([this]() { handleNotFound(); });
    
    server->begin();
    
    // DNS server for captive portal
    dnsServer->start(53, "*", apIP);
    
    Serial.println("Web server started on port 80");
}

void ESP32WebConfig::initMDNS() {
    #ifdef ESP8266
    if (MDNS.begin(mdnsName.c_str())) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("mDNS started: http://%s.local\n", mdnsName.c_str());
    }
    #else
    if (MDNS.begin(mdnsName.c_str())) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("mDNS started: http://%s.local\n", mdnsName.c_str());
    }
    #endif
}

// ===== WEB HANDLERS =====

void ESP32WebConfig::handleRoot() {
    if (!authenticate()) return;
    
    String html = getMainPageHTML();
    html = replacePlaceholders(html);
    server->send(200, "text/html", html);
}

void ESP32WebConfig::handleConfig() {
    if (!authenticate()) return;
    
    String html = getConfigPageHTML();
    html = replacePlaceholders(html);
    server->send(200, "text/html", html);
}

void ESP32WebConfig::handleSave() {
    if (!authenticate()) return;
    
    if (server->method() == HTTP_POST) {
        // Save all parameters from form
        bool hasChanges = false;
        
        for (auto& param : parameters) {
            if (server->hasArg(param.key)) {
                String newValue = server->arg(param.key);
                if (param.type == PARAM_CHECKBOX) {
                    newValue = server->hasArg(param.key) ? "true" : "false";
                }
                
                if (param.value != newValue) {
                    if (setParameterValue(param.key, newValue)) {
                        hasChanges = true;
                    }
                }
            }
        }
        
        if (hasChanges) {
            save();
        }
        
        server->send(200, "text/html", 
            "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Saved</title>"
            "<meta http-equiv='refresh' content='3;url=/'></head><body>"
            "<h1>✅ Configuration saved!</h1>"
            "<p>Redirecting in 3 seconds...</p>"
            "</body></html>");
    }
}

void ESP32WebConfig::handleExport() {
    if (!authenticate()) return;
    
    String config = exportConfig();
    server->sendHeader("Content-Disposition", "attachment; filename=\"" + deviceName + "_config.json\"");
    server->send(200, "application/json", config);
}

void ESP32WebConfig::handleNotFound() {
    // Captive portal redirect
    server->sendHeader("Location", "http://" + apIP.toString(), true);
    server->send(302, "text/plain", "");
}

bool ESP32WebConfig::authenticate() {
    if (!server->authenticate(adminUsername.c_str(), adminPassword.c_str())) {
        server->requestAuthentication();
        return false;
    }
    return true;
}

// ===== EEPROM FUNCTIONS =====

bool ESP32WebConfig::saveToEEPROM() {
    JsonDocument doc;
    doc["magic"] = WEBCONFIG_MAGIC_NUMBER;
    doc["version"] = ESP32WEBCONFIG_VERSION;
    
    JsonObject config = doc["config"];
    for (const auto& param : parameters) {
        config[param.key] = param.value;
    }
    
    String output;
    serializeJson(doc, output);
    
    // Check size
    if (output.length() > eepromSize - 4) {
        Serial.println("ESP32WebConfig: Configuration too large for EEPROM");
        return false;
    }
    
    // Save size
    EEPROM.put(eepromStartAddr, (uint16_t)output.length());
    
    // Save data
    for (size_t i = 0; i < output.length(); i++) {
        EEPROM.write(eepromStartAddr + 2 + i, output[i]);
    }
    
    return EEPROM.commit();
}

bool ESP32WebConfig::loadFromEEPROM() {
    // Load size
    uint16_t size;
    EEPROM.get(eepromStartAddr, size);
    
    if (size == 0 || size > eepromSize - 4) {
        return false;
    }
    
    // Load data
    String input;
    input.reserve(size);
    for (uint16_t i = 0; i < size; i++) {
        input += (char)EEPROM.read(eepromStartAddr + 2 + i);
    }
    
    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input);
    
    if (error) {
        Serial.printf("ESP32WebConfig: EEPROM data parsing error: %s\n", error.c_str());
        return false;
    }
    
    // Check magic number
    if (doc["magic"] != WEBCONFIG_MAGIC_NUMBER) {
        Serial.println("ESP32WebConfig: Invalid EEPROM data (magic number)");
        return false;
    }
    
    // Load values
    JsonObject config = doc["config"];
    for (JsonPair kv : config) {
        setParameterValue(kv.key().c_str(), kv.value().as<String>());
    }
    
    configChanged = false;
    return true;
}

void ESP32WebConfig::clearEEPROM() {
    for (int i = eepromStartAddr; i < eepromStartAddr + eepromSize; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

// ===== STATUS AND CONFIG JSON =====

String ESP32WebConfig::getStatusJSON() {
    JsonDocument doc;
    doc["device"] = deviceName;
    doc["version"] = ESP32WEBCONFIG_VERSION;
    doc["uptime"] = millis();
    doc["wifi_connected"] = isConnected();
    doc["wifi_ssid"] = WiFi.SSID();
    doc["local_ip"] = getLocalIP();
    doc["ap_ip"] = getAPIP();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["config_changed"] = configChanged;
    
    String output;
    serializeJson(doc, output);
    return output;
}

String ESP32WebConfig::getConfigJSON() {
    JsonDocument doc;
    for (const auto& param : parameters) {
        doc[param.key] = param.value;
    }
    
    String output;
    serializeJson(doc, output);
    return output;
}

// ===== PLACEHOLDER REPLACEMENT =====

String ESP32WebConfig::replacePlaceholders(String html) {
    html.replace("%DEVICE_NAME%", deviceName);
    html.replace("%VERSION%", ESP32WEBCONFIG_VERSION);
    html.replace("%WIFI_SSID%", WiFi.SSID());
    html.replace("%WIFI_IP%", getLocalIP());
    html.replace("%AP_IP%", getAPIP());
    html.replace("%FREE_HEAP%", String(ESP.getFreeHeap()));
    html.replace("%UPTIME%", String(millis() / 1000));
    
    return html;
}

// Placeholder implementations for missing handlers
void ESP32WebConfig::handleNetwork() { handleRoot(); }
void ESP32WebConfig::handleAdvanced() { handleRoot(); }
void ESP32WebConfig::handleBackup() { handleRoot(); }
void ESP32WebConfig::handleGet() { server->send(200, "application/json", getConfigJSON()); }
void ESP32WebConfig::handleSet() { server->send(200, "text/plain", "OK"); }
void ESP32WebConfig::handleReset() { reset(); handleRoot(); }
void ESP32WebConfig::handleReboot() { ESP.restart(); }
void ESP32WebConfig::handleImport() { server->send(200, "text/plain", "OK"); }

// Placeholder implementations for missing template functions
String ESP32WebConfig::getMainPageHTML() { return WebPages::getMainPage(); }
String ESP32WebConfig::getConfigPageHTML() { return WebPages::getConfigPage(); }
String ESP32WebConfig::getNetworkPageHTML() { return WebPages::getNetworkPage(); }
String ESP32WebConfig::getAdvancedPageHTML() { return WebPages::getAdvancedPage(); }
String ESP32WebConfig::getBackupPageHTML() { return WebPages::getBackupPage(); }
String ESP32WebConfig::generateParameterGroupsHTML() { return ""; }
String ESP32WebConfig::getGroupTitle(const String& groupName) { return groupName; }
String ESP32WebConfig::generateParameterHTML(const ConfigParameter& param) { return ""; }
String ESP32WebConfig::generateSelectOptions(const ConfigParameter& param) { return ""; }
String ESP32WebConfig::getInputType(ParameterType type) { return "text"; }
void ESP32WebConfig::enableDebug(bool enable) { /* TODO */ }