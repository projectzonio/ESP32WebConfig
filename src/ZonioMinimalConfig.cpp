/*
 * ESP8266/ESP32C3 Minimal WebConfig Library - Implementation
 * Ultra-lightweight configuration interface for Zonio ecosystem
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#include "ZonioMinimalConfig.h"

ZonioMinimalConfig::ZonioMinimalConfig() {
    server = nullptr;
    dnsServer = nullptr;
    paramCount = 0;
    apActive = false;
    configMode = false;
    shouldRestart = false;
    apStartTime = 0;
    
    // Default settings
    strcpy(deviceName, "Zonio-Device");
    strcpy(apSSID, "Zonio-Config");
    strcpy(apPassword, "zonio123");
    apIP = IPAddress(192, 168, 4, 1);
}

ZonioMinimalConfig::~ZonioMinimalConfig() {
    if (server) delete server;
    if (dnsServer) delete dnsServer;
}

void ZonioMinimalConfig::setDeviceName(const char* name) {
    strncpy(deviceName, name, sizeof(deviceName) - 1);
    deviceName[sizeof(deviceName) - 1] = '\0';
}

void ZonioMinimalConfig::setAPCredentials(const char* ssid, const char* password) {
    strncpy(apSSID, ssid, sizeof(apSSID) - 1);
    strncpy(apPassword, password, sizeof(apPassword) - 1);
    apSSID[sizeof(apSSID) - 1] = '\0';
    apPassword[sizeof(apPassword) - 1] = '\0';
}

void ZonioMinimalConfig::setAPIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    apIP = IPAddress(a, b, c, d);
}

void ZonioMinimalConfig::addParameter(const char* key, const char* label, const char* defaultValue, ZonioParamType type) {
    if (paramCount >= ZONIO_MAX_PARAMS) return;
    
    ZonioParam* param = &params[paramCount];
    strncpy(param->key, key, sizeof(param->key) - 1);
    strncpy(param->label, label, sizeof(param->label) - 1);
    strncpy(param->value, defaultValue, sizeof(param->value) - 1);
    strncpy(param->defaultValue, defaultValue, sizeof(param->defaultValue) - 1);
    param->type = type;
    param->minValue = 0;
    param->maxValue = 100;
    param->options[0] = '\0';
    
    param->key[sizeof(param->key) - 1] = '\0';
    param->label[sizeof(param->label) - 1] = '\0';
    param->value[sizeof(param->value) - 1] = '\0';
    param->defaultValue[sizeof(param->defaultValue) - 1] = '\0';
    
    paramCount++;
}

void ZonioMinimalConfig::addNumberParameter(const char* key, const char* label, int defaultValue, int minVal, int maxVal) {
    addParameter(key, label, String(defaultValue).c_str(), ZONIO_NUMBER);
    if (paramCount > 0) {
        params[paramCount - 1].minValue = minVal;
        params[paramCount - 1].maxValue = maxVal;
    }
}

void ZonioMinimalConfig::addFloatParameter(const char* key, const char* label, float defaultValue) {
    addParameter(key, label, String(defaultValue, 2).c_str(), ZONIO_FLOAT);
}

void ZonioMinimalConfig::addCheckboxParameter(const char* key, const char* label, bool defaultValue) {
    addParameter(key, label, defaultValue ? "true" : "false", ZONIO_CHECKBOX);
}

void ZonioMinimalConfig::addSelectParameter(const char* key, const char* label, const char* options, const char* defaultValue) {
    addParameter(key, label, defaultValue, ZONIO_SELECT);
    if (paramCount > 0) {
        strncpy(params[paramCount - 1].options, options, sizeof(params[paramCount - 1].options) - 1);
        params[paramCount - 1].options[sizeof(params[paramCount - 1].options) - 1] = '\0';
    }
}

const char* ZonioMinimalConfig::getString(const char* key) {
    for (int i = 0; i < paramCount; i++) {
        if (strcmp(params[i].key, key) == 0) {
            return params[i].value;
        }
    }
    return "";
}

int ZonioMinimalConfig::getInt(const char* key) {
    return atoi(getString(key));
}

float ZonioMinimalConfig::getFloat(const char* key) {
    return atof(getString(key));
}

bool ZonioMinimalConfig::getBool(const char* key) {
    const char* value = getString(key);
    return strcmp(value, "true") == 0 || strcmp(value, "1") == 0;
}

void ZonioMinimalConfig::begin() {
    #ifdef ESP8266
    EEPROM.begin(ZONIO_EEPROM_SIZE);
    #else
    EEPROM.begin(ZONIO_EEPROM_SIZE);
    #endif
    
    // Load configuration
    if (!loadConfig()) {
        Serial.println("No valid config found, using defaults");
    }
    
    // Start AP mode for configuration
    configMode = true;
    apActive = true;
    apStartTime = millis();
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(apSSID, apPassword);
    
    // Start web server
    server = new WebServer(80);
    dnsServer = new DNSServer();
    
    server->on("/", [this]() { handleRoot(); });
    server->on("/save", HTTP_POST, [this]() { handleSave(); });
    server->onNotFound([this]() { handleNotFound(); });
    
    server->begin();
    dnsServer->start(53, "*", apIP);
    
    Serial.println("Zonio Config started");
    Serial.print("AP: ");
    Serial.print(apSSID);
    Serial.print(" | IP: ");
    Serial.println(apIP);
    Serial.println("Configuration timeout: 5 minutes");
}

void ZonioMinimalConfig::handle() {
    if (!apActive) return;
    
    server->handleClient();
    dnsServer->processNextRequest();
    
    // Check timeout
    if (millis() - apStartTime > ZONIO_AP_TIMEOUT) {
        Serial.println("Configuration timeout reached");
        apActive = false;
        configMode = false;
        
        // Stop AP mode
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        
        if (server) {
            server->stop();
            delete server;
            server = nullptr;
        }
        if (dnsServer) {
            dnsServer->stop();
            delete dnsServer;
            dnsServer = nullptr;
        }
        
        Serial.println("AP mode stopped - switching to main application");
    }
}

void ZonioMinimalConfig::saveConfig() {
    // Write magic number and version
    EEPROM.write(0, (ZONIO_MAGIC_NUMBER >> 24) & 0xFF);
    EEPROM.write(1, (ZONIO_MAGIC_NUMBER >> 16) & 0xFF);
    EEPROM.write(2, (ZONIO_MAGIC_NUMBER >> 8) & 0xFF);
    EEPROM.write(3, ZONIO_MAGIC_NUMBER & 0xFF);
    EEPROM.write(4, ZONIO_CONFIG_VERSION);
    EEPROM.write(5, paramCount);
    
    // Write parameters
    int addr = 6;
    for (int i = 0; i < paramCount; i++) {
        EEPROM.put(addr, params[i]);
        addr += sizeof(ZonioParam);
    }
    
    EEPROM.commit();
    Serial.println("Configuration saved to EEPROM");
}

bool ZonioMinimalConfig::loadConfig() {
    // Check magic number
    uint32_t magic = 0;
    magic |= (uint32_t)EEPROM.read(0) << 24;
    magic |= (uint32_t)EEPROM.read(1) << 16;
    magic |= (uint32_t)EEPROM.read(2) << 8;
    magic |= (uint32_t)EEPROM.read(3);
    
    if (magic != ZONIO_MAGIC_NUMBER) {
        Serial.println("Invalid magic number in EEPROM");
        return false;
    }
    
    // Check version
    if (EEPROM.read(4) != ZONIO_CONFIG_VERSION) {
        Serial.println("Invalid config version");
        return false;
    }
    
    // Read parameter count
    int savedParamCount = EEPROM.read(5);
    if (savedParamCount > ZONIO_MAX_PARAMS) {
        Serial.println("Too many parameters in EEPROM");
        return false;
    }
    
    // Read parameters
    int addr = 6;
    for (int i = 0; i < savedParamCount && i < paramCount; i++) {
        ZonioParam savedParam;
        EEPROM.get(addr, savedParam);
        
        // Find matching parameter by key
        for (int j = 0; j < paramCount; j++) {
            if (strcmp(params[j].key, savedParam.key) == 0) {
                strncpy(params[j].value, savedParam.value, sizeof(params[j].value) - 1);
                params[j].value[sizeof(params[j].value) - 1] = '\0';
                break;
            }
        }
        
        addr += sizeof(ZonioParam);
    }
    
    Serial.println("Configuration loaded from EEPROM");
    return true;
}

void ZonioMinimalConfig::generateHTML(char* buffer, size_t bufferSize) {
    snprintf(buffer, bufferSize,
        "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>%s</title><style>"
        "body{font-family:Arial,sans-serif;margin:0;padding:20px;background:#f0f0f0}"
        ".container{max-width:400px;margin:0 auto;background:white;padding:20px;border-radius:8px;box-shadow:0 2px 10px rgba(0,0,0,0.1)}"
        "h1{color:#333;text-align:center;margin-bottom:20px}"
        ".form-group{margin-bottom:15px}"
        "label{display:block;margin-bottom:5px;font-weight:bold;color:#555}"
        "input,select{width:100%%;padding:8px;border:1px solid #ddd;border-radius:4px;font-size:14px;box-sizing:border-box}"
        "input[type='checkbox']{width:auto;margin-right:8px}"
        ".checkbox-group{display:flex;align-items:center}"
        ".btn{background:#007cba;color:white;padding:10px 20px;border:none;border-radius:4px;cursor:pointer;font-size:16px;width:100%%}"
        ".btn:hover{background:#005a87}"
        ".timeout{color:#666;font-size:12px;text-align:center;margin-top:10px}"
        "</style></head><body><div class='container'><h1>%s</h1><form method='POST' action='/save'>",
        deviceName, deviceName);
    
    // Add form fields
    for (int i = 0; i < paramCount; i++) {
        char fieldHTML[256];
        ZonioParam* param = &params[i];
        
        switch (param->type) {
            case ZONIO_TEXT:
                snprintf(fieldHTML, sizeof(fieldHTML),
                    "<div class='form-group'><label>%s</label><input type='text' name='%s' value='%s'></div>",
                    param->label, param->key, param->value);
                break;
                
            case ZONIO_NUMBER:
                snprintf(fieldHTML, sizeof(fieldHTML),
                    "<div class='form-group'><label>%s</label><input type='number' name='%s' value='%s' min='%d' max='%d'></div>",
                    param->label, param->key, param->value, param->minValue, param->maxValue);
                break;
                
            case ZONIO_FLOAT:
                snprintf(fieldHTML, sizeof(fieldHTML),
                    "<div class='form-group'><label>%s</label><input type='number' name='%s' value='%s' step='0.1'></div>",
                    param->label, param->key, param->value);
                break;
                
            case ZONIO_CHECKBOX:
                snprintf(fieldHTML, sizeof(fieldHTML),
                    "<div class='form-group'><div class='checkbox-group'><input type='checkbox' name='%s' value='true'%s><label>%s</label></div></div>",
                    param->key, (strcmp(param->value, "true") == 0) ? " checked" : "", param->label);
                break;
                
            case ZONIO_SELECT:
                // Simple select implementation
                snprintf(fieldHTML, sizeof(fieldHTML),
                    "<div class='form-group'><label>%s</label><select name='%s'><option value='%s'>%s</option></select></div>",
                    param->label, param->key, param->value, param->value);
                break;
        }
        
        strncat(buffer, fieldHTML, bufferSize - strlen(buffer) - 1);
    }
    
    // Add submit button and footer
    strncat(buffer, 
        "<button type='submit' class='btn'>Save & Continue</button>"
        "</form><div class='timeout'>Configuration will timeout in 5 minutes</div>"
        "</div></body></html>", 
        bufferSize - strlen(buffer) - 1);
}

void ZonioMinimalConfig::handleRoot() {
    static char htmlBuffer[2048];
    generateHTML(htmlBuffer, sizeof(htmlBuffer));
    server->send(200, "text/html", htmlBuffer);
}

void ZonioMinimalConfig::handleSave() {
    // Process form data
    for (int i = 0; i < paramCount; i++) {
        ZonioParam* param = &params[i];
        
        if (param->type == ZONIO_CHECKBOX) {
            // Checkbox handling
            if (server->hasArg(param->key)) {
                strcpy(param->value, "true");
            } else {
                strcpy(param->value, "false");
            }
        } else {
            // Other input types
            if (server->hasArg(param->key)) {
                String value = server->arg(param->key);
                strncpy(param->value, value.c_str(), sizeof(param->value) - 1);
                param->value[sizeof(param->value) - 1] = '\0';
            }
        }
    }
    
    // Save to EEPROM
    saveConfig();
    
    // Send response
    server->send(200, "text/html", 
        "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Saved</title><style>"
        "body{font-family:Arial,sans-serif;margin:0;padding:20px;background:#f0f0f0;text-align:center}"
        ".container{max-width:400px;margin:0 auto;background:white;padding:20px;border-radius:8px;box-shadow:0 2px 10px rgba(0,0,0,0.1)}"
        "h1{color:#28a745}"
        "</style></head><body><div class='container'><h1>✓ Configuration Saved</h1>"
        "<p>Device will now continue with normal operation.</p></div></body></html>");
    
    // Mark for restart and stop AP
    shouldRestart = true;
    apActive = false;
    configMode = false;
    
    delay(1000);
    
    // Stop AP mode
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    
    if (server) {
        server->stop();
        delete server;
        server = nullptr;
    }
    if (dnsServer) {
        dnsServer->stop();
        delete dnsServer;
        dnsServer = nullptr;
    }
    
    Serial.println("Configuration saved - AP mode stopped");
}

void ZonioMinimalConfig::handleNotFound() {
    // Redirect to main page for captive portal
    server->sendHeader("Location", "http://" + apIP.toString(), true);
    server->send(302, "text/plain", "");
}