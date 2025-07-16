/*
 * ESP32WebConfig Advanced Demo
 * 
 * This example shows advanced features of ESP32WebConfig:
 * - Parameter groups
 * - Callbacks for parameter changes
 * - Custom validation
 * - Export/import configuration
 * - Integration with existing application logic
 * 
 * Based on ZONIO PWM controller firmware
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#include <ESP32WebConfig.h>

ESP32WebConfig webConfig;

// Application configuration structure
struct AppConfig {
    String deviceName;
    int operationMode;
    float targetTemperature;
    float failsafeTemperature;
    bool enableLogging;
    String mqttServer;
    int mqttPort;
    String mqttUsername;
    String mqttPassword;
    int minPWM;
    int maxPWM;
    int dayMode;
    int dayStart;
    bool autoMode;
} appConfig;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("=== ESP32WebConfig Advanced Demo ===");
    
    // ===== LIBRARY CONFIGURATION =====
    
    webConfig.setDeviceName("ESP32-PWM-Controller");
    webConfig.setAPConfig("ESP32-PWM-Config", "12345678", IPAddress(192, 168, 5, 1));
    webConfig.setSTAConfig("TP-Link_63D1", "52855993");  // Your WiFi
    webConfig.setCredentials("admin", "admin123");
    webConfig.setMDNS("pwm-controller");
    
    // ===== PARAMETER GROUPS =====
    
    // === BASIC SETTINGS ===
    webConfig.addParameterToGroup("basic", "device_name", "Device Name", PARAM_TEXT, "ESP32-PWM-Controller");
    webConfig.addParameterToGroup("basic", "control_mode", "Control Mode", PARAM_SELECT, "0");
    webConfig.setParameterAttributes("control_mode", "data-options='[{\"0\":\"Automatic\"},{\"1\":\"Manual\"},{\"2\":\"Schedule\"}]'");
    
    webConfig.addParameterToGroup("basic", "enable_logging", "Enable Logging", PARAM_CHECKBOX, "true");
    webConfig.addParameterToGroup("basic", "log_level", "Log Level", PARAM_RANGE, "2");
    webConfig.setParameterAttributes("log_level", "min='0' max='4' step='1'");
    
    // === TEMPERATURE REGULATION ===
    webConfig.addParameterToGroup("temp", "target_temp", "Target Temperature (°C)", PARAM_FLOAT, "28.0");
    webConfig.setParameterAttributes("target_temp", "min='15.0' max='40.0' step='0.1'");
    
    webConfig.addParameterToGroup("temp", "activation_temp", "Activation Temperature (°C)", PARAM_FLOAT, "26.0");
    webConfig.setParameterAttributes("activation_temp", "min='15.0' max='35.0' step='0.1'");
    
    webConfig.addParameterToGroup("temp", "failsafe_temp", "Failsafe Temperature (°C)", PARAM_FLOAT, "31.0");
    webConfig.setParameterAttributes("failsafe_temp", "min='30.0' max='40.0' step='0.1'");
    
    // === PWM SETTINGS ===
    webConfig.addParameterToGroup("pwm", "min_light", "Minimum Light (%)", PARAM_RANGE, "20");
    webConfig.setParameterAttributes("min_light", "min='0' max='100' step='1'");
    
    webConfig.addParameterToGroup("pwm", "max_light", "Maximum Light (%)", PARAM_RANGE, "100");
    webConfig.setParameterAttributes("max_light", "min='0' max='100' step='1'");
    
    webConfig.addParameterToGroup("pwm", "start_light", "Initial Light (%)", PARAM_RANGE, "35");
    webConfig.setParameterAttributes("start_light", "min='0' max='100' step='1'");
    
    // === LIGHT CYCLE ===
    webConfig.addParameterToGroup("light", "light_cycle", "Light Cycle", PARAM_SELECT, "0");
    webConfig.setParameterAttributes("light_cycle", "data-options='[{\"0\":\"12h day / 12h night\"},{\"1\":\"18h day / 6h night\"},{\"2\":\"20h day / 4h night\"},{\"3\":\"24h day / 0h night\"}]'");
    
    webConfig.addParameterToGroup("light", "day_start_hour", "Day Start Hour", PARAM_NUMBER, "20");
    webConfig.setParameterAttributes("day_start_hour", "min='0' max='23' step='1'");
    
    webConfig.addParameterToGroup("light", "day_start_minute", "Day Start Minute", PARAM_NUMBER, "0");
    webConfig.setParameterAttributes("day_start_minute", "min='0' max='59' step='1'");
    
    // === MQTT COMMUNICATION ===
    webConfig.addParameterToGroup("mqtt", "mqtt_server", "MQTT Server", PARAM_TEXT, "192.168.0.131");
    webConfig.addParameterToGroup("mqtt", "mqtt_port", "MQTT Port", PARAM_NUMBER, "1883");
    webConfig.setParameterAttributes("mqtt_port", "min='1' max='65535' step='1'");
    
    webConfig.addParameterToGroup("mqtt", "mqtt_username", "MQTT Username", PARAM_TEXT, "zonio");
    webConfig.addParameterToGroup("mqtt", "mqtt_password", "MQTT Password", PARAM_PASSWORD, "pentane");
    
    webConfig.addParameterToGroup("mqtt", "temp_topic", "Temperature Topic", PARAM_TEXT, "zonio/weatherVenku");
    webConfig.addParameterToGroup("mqtt", "control_topic", "Control Topic", PARAM_TEXT, "zonio/pwm/control");
    webConfig.addParameterToGroup("mqtt", "status_topic", "Status Topic", PARAM_TEXT, "zonio/pwm/status");
    
    // ===== CALLBACK FUNCTIONS =====
    
    // Parameter change callback
    webConfig.onParameterChange([](String key, String oldValue, String newValue) {
        Serial.printf("Parameter changed: %s = %s -> %s\n", key.c_str(), oldValue.c_str(), newValue.c_str());
        
        // Specific reactions to key changes
        if (key == "control_mode") {
            int mode = newValue.toInt();
            Serial.printf("Control mode changed to: %s\n", mode == 0 ? "AUTO" : mode == 1 ? "MANUAL" : "SCHEDULE");
            updateOperationMode(mode);
        }
        
        if (key == "target_temp") {
            float temp = newValue.toFloat();
            Serial.printf("New target temperature: %.1f°C\n", temp);
            updateTargetTemperature(temp);
        }
        
        if (key == "failsafe_temp") {
            float temp = newValue.toFloat();
            Serial.printf("New failsafe temperature: %.1f°C\n", temp);
            updateFailsafeTemperature(temp);
        }
        
        // Immediate synchronization with application config
        syncConfigToApp();
    });
    
    // Configuration save callback
    webConfig.onConfigSave([]() {
        Serial.println("Configuration saved to EEPROM!");
        
        // Synchronize with application configuration
        syncConfigToApp();
        
        // Apply new configuration
        Serial.println("Applying new configuration...");
        applyNewConfiguration();
        
        // Optional restart for critical changes
        if (needsRestart()) {
            Serial.println("Restart required - restarting in 3 seconds...");
            delay(3000);
            ESP.restart();
        }
    });
    
    // Configuration load callback
    webConfig.onConfigLoad([]() {
        Serial.println("Configuration loaded from EEPROM");
        syncConfigToApp();
    });
    
    // Custom validator for complex checks
    webConfig.setCustomValidator([](String key, String value) -> String {
        // Temperature range validation
        if (key == "target_temp") {
            float temp = value.toFloat();
            if (temp < 15.0 || temp > 40.0) {
                return "Target temperature must be between 15-40°C";
            }
        }
        
        if (key == "failsafe_temp") {
            float temp = value.toFloat();
            if (temp < 30.0 || temp > 40.0) {
                return "Failsafe temperature must be between 30-40°C";
            }
            
            // Check that failsafe is higher than target
            float targetTemp = webConfig.getFloat("target_temp");
            if (temp <= targetTemp) {
                return "Failsafe temperature must be higher than target temperature";
            }
        }
        
        // MQTT port validation
        if (key == "mqtt_port") {
            int port = value.toInt();
            if (port < 1 || port > 65535) {
                return "MQTT port must be between 1 and 65535";
            }
        }
        
        // PWM range validation
        if (key == "min_light") {
            int minLight = value.toInt();
            int maxLight = webConfig.getInt("max_light");
            if (minLight >= maxLight) {
                return "Minimum light must be less than maximum light";
            }
        }
        
        if (key == "max_light") {
            int maxLight = value.toInt();
            int minLight = webConfig.getInt("min_light");
            if (maxLight <= minLight) {
                return "Maximum light must be greater than minimum light";
            }
        }
        
        return ""; // Validation OK
    });
    
    // ===== START SYSTEM =====
    webConfig.begin();
    
    // Load and synchronize configuration
    webConfig.load();
    syncConfigToApp();
    
    // Initialize application according to configuration
    initializeApplication();
    
    Serial.println("");
    Serial.println("=== WEB INTERFACE STARTED ===");
    Serial.printf("AP: %s (IP: %s)\n", "ESP32-PWM-Config", webConfig.getAPIP().c_str());
    if (webConfig.isConnected()) {
        Serial.printf("STA: %s (IP: %s)\n", WiFi.SSID().c_str(), webConfig.getLocalIP().c_str());
    }
    Serial.printf("mDNS: http://pwm-controller.local\n");
    Serial.printf("Login: admin / admin123\n");
    Serial.printf("Parameters: %d\n", webConfig.getParameterCount());
    Serial.println("");
    
    printCurrentConfig();
}

void loop() {
    // MANDATORY - handle web configuration
    webConfig.handle();
    
    // ===== MAIN APPLICATION LOGIC =====
    
    static unsigned long lastSensorRead = 0;
    static unsigned long lastStatusPrint = 0;
    static unsigned long lastMQTTUpdate = 0;
    
    unsigned long now = millis();
    
    // Read sensors according to configured interval
    if (now - lastSensorRead > 5000) { // Every 5 seconds for demo
        lastSensorRead = now;
        readSensors();
        
        // Regulation according to mode
        if (appConfig.operationMode == 0) { // Automatic
            performAutomaticControl();
        }
    }
    
    // Periodic status print
    if (now - lastStatusPrint > 30000) { // Every 30 seconds
        lastStatusPrint = now;
        printSystemStatus();
    }
    
    // MQTT update
    if (now - lastMQTTUpdate > 60000) { // Every minute
        lastMQTTUpdate = now;
        sendMQTTUpdate();
    }
    
    // Memory check if enabled
    if (appConfig.enableLogging) {
        static unsigned long lastMemCheck = 0;
        if (now - lastMemCheck > 60000) { // Every minute
            lastMemCheck = now;
            checkMemoryUsage();
        }
    }
    
    delay(100); // Short delay for stability
}

// ===== HELPER FUNCTIONS =====

void syncConfigToApp() {
    // Synchronize web configuration with application structure
    appConfig.deviceName = webConfig.getValue("device_name");
    appConfig.operationMode = webConfig.getInt("control_mode");
    appConfig.targetTemperature = webConfig.getFloat("target_temp");
    appConfig.failsafeTemperature = webConfig.getFloat("failsafe_temp");
    appConfig.enableLogging = webConfig.getBool("enable_logging");
    appConfig.mqttServer = webConfig.getValue("mqtt_server");
    appConfig.mqttPort = webConfig.getInt("mqtt_port");
    appConfig.mqttUsername = webConfig.getValue("mqtt_username");
    appConfig.mqttPassword = webConfig.getValue("mqtt_password");
    appConfig.minPWM = webConfig.getInt("min_light");
    appConfig.maxPWM = webConfig.getInt("max_light");
    appConfig.dayMode = webConfig.getInt("light_cycle");
    appConfig.dayStart = webConfig.getInt("day_start_hour");
    
    Serial.println("Configuration synchronized with application");
}

void printCurrentConfig() {
    Serial.println("=== CURRENT CONFIGURATION ===");
    Serial.printf("Name: %s\n", appConfig.deviceName.c_str());
    Serial.printf("Mode: %s\n", 
                 appConfig.operationMode == 0 ? "Automatic" : 
                 appConfig.operationMode == 1 ? "Manual" : "Schedule");
    Serial.printf("Target temp: %.1f°C\n", appConfig.targetTemperature);
    Serial.printf("Failsafe temp: %.1f°C\n", appConfig.failsafeTemperature);
    Serial.printf("PWM range: %d%% - %d%%\n", appConfig.minPWM, appConfig.maxPWM);
    Serial.printf("MQTT: %s:%d\n", appConfig.mqttServer.c_str(), appConfig.mqttPort);
    Serial.printf("Light cycle: %s\n", getLightCycleName(appConfig.dayMode).c_str());
    Serial.println("============================");
}

String getLightCycleName(int mode) {
    switch (mode) {
        case 0: return "12h day / 12h night";
        case 1: return "18h day / 6h night";
        case 2: return "20h day / 4h night";
        case 3: return "24h day / 0h night";
        default: return "Unknown";
    }
}

void updateOperationMode(int mode) {
    // Apply operation mode change
    switch (mode) {
        case 0: // Automatic
            Serial.println("Switching to automatic mode");
            enableAutomaticControl(true);
            break;
        case 1: // Manual
            Serial.println("Switching to manual mode");
            enableAutomaticControl(false);
            break;
        case 2: // Schedule
            Serial.println("Switching to schedule mode");
            startScheduleMode();
            break;
    }
}

void updateTargetTemperature(float temp) {
    // Apply new target temperature
    appConfig.targetTemperature = temp;
    Serial.printf("New target temperature set: %.1f°C\n", temp);
    
    // Immediate change in regulation if in automatic mode
    if (appConfig.operationMode == 0) {
        adjustControllerTarget(temp);
    }
}

void updateFailsafeTemperature(float temp) {
    // Apply new failsafe temperature
    appConfig.failsafeTemperature = temp;
    Serial.printf("New failsafe temperature set: %.1f°C\n", temp);
}

void initializeApplication() {
    Serial.println("Initializing application according to configuration...");
    
    // Initialize PWM pins
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Initialize MQTT if configured
    if (!appConfig.mqttServer.isEmpty()) {
        initMQTT();
    }
    
    // Set initial operation mode
    updateOperationMode(appConfig.operationMode);
    
    Serial.println("Application initialized");
}

void applyNewConfiguration() {
    Serial.println("Applying new configuration...");
    
    // Restart MQTT if parameters changed
    if (!appConfig.mqttServer.isEmpty()) {
        disconnectMQTT();
        delay(1000);
        initMQTT();
    }
    
    // Apply other configuration changes...
}

bool needsRestart() {
    // Check if restart is needed after configuration change
    // For demo, no restart needed
    return false;
}

void readSensors() {
    if (appConfig.enableLogging) {
        Serial.println("Reading sensors...");
    }
    // Here would be actual sensor reading
}

void performAutomaticControl() {
    // Automatic regulation according to target temperature
    if (appConfig.enableLogging) {
        Serial.printf("Automatic regulation - target: %.1f°C\n", appConfig.targetTemperature);
    }
}

void enableAutomaticControl(bool enable) {
    // Enable/disable automatic regulation
    Serial.printf("Automatic regulation: %s\n", enable ? "ENABLED" : "DISABLED");
}

void startScheduleMode() {
    Serial.println("Starting schedule mode...");
    // Schedule sequence
}

void adjustControllerTarget(float temp) {
    // Immediate setting of new target temperature in controller
    Serial.printf("Controller retargeted to %.1f°C\n", temp);
}

void printSystemStatus() {
    Serial.printf("Status - Heap: %d B, Uptime: %lu s, Mode: %d, Target: %.1f°C\n",
                 ESP.getFreeHeap(), millis()/1000, appConfig.operationMode, appConfig.targetTemperature);
}

void sendMQTTUpdate() {
    if (appConfig.mqttServer.isEmpty()) return;
    
    Serial.println("Sending MQTT update...");
    // MQTT messages would be sent here
}

void checkMemoryUsage() {
    uint32_t freeHeap = ESP.getFreeHeap();
    if (freeHeap < 50000) { // Less than 50kB
        Serial.printf("Low memory warning: %d B\n", freeHeap);
    }
}

void initMQTT() {
    Serial.printf("Initializing MQTT connection to %s:%d\n", 
                 appConfig.mqttServer.c_str(), appConfig.mqttPort);
    // MQTT client initialization would be here
}

void disconnectMQTT() {
    Serial.println("Disconnecting MQTT");
    // MQTT disconnection would be here
}