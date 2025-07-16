/*
 * ZonioMinimalConfig Example
 * 
 * Ultra-lightweight configuration interface for ESP8266/ESP32-C3
 * Perfect for projects with limited flash memory
 * 
 * Features:
 * - Only ~3KB flash usage
 * - 5-minute timeout AP mode
 * - Single HTML page configuration
 * - Compatible with existing firmware
 * 
 * Hardware tested:
 * - ESP8266 Wemos D1 Mini
 * - ESP32-C3 Super Mini
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#include "ZonioMinimalConfig.h"

// Create minimal config instance
ZonioMinimalConfig zonioConfig;

// Configuration variables (loaded from web interface)
char wifiSSID[64];
char wifiPassword[64];
char mqttServer[64];
int mqttPort;
char mqttUsername[32];
char mqttPassword[32];
int sensorInterval;
bool enableMQTT;
bool enableDebug;

void setup() {
    Serial.begin(115200);
    Serial.println("ZonioMinimalConfig Example");
    Serial.println("==========================");
    
    // ===== SETUP MINIMAL CONFIGURATION =====
    
    // Set device name (appears in web interface and AP name)
    zonioConfig.setDeviceName("Zonio-Minimal-Device");
    
    // Configure Access Point credentials
    zonioConfig.setAPCredentials("Zonio-Config", "zonio123");
    
    // Set custom AP IP address (optional)
    zonioConfig.setAPIP(192, 168, 4, 1);
    
    // ===== ADD CONFIGURATION PARAMETERS =====
    
    // Network settings
    zonioConfig.addParameter("wifi_ssid", "WiFi SSID", "TP-Link_63D1");
    zonioConfig.addParameter("wifi_password", "WiFi Password", "52855993");
    
    // MQTT settings
    zonioConfig.addParameter("mqtt_server", "MQTT Server", "192.168.0.131");
    zonioConfig.addNumberParameter("mqtt_port", "MQTT Port", 1883, 1, 65535);
    zonioConfig.addParameter("mqtt_username", "MQTT Username", "zonio");
    zonioConfig.addParameter("mqtt_password", "MQTT Password", "pentane");
    
    // Application settings
    zonioConfig.addNumberParameter("sensor_interval", "Sensor Interval (s)", 30, 5, 300);
    zonioConfig.addCheckboxParameter("enable_mqtt", "Enable MQTT", true);
    zonioConfig.addCheckboxParameter("enable_debug", "Debug Output", false);
    
    // ===== START CONFIGURATION MODE =====
    zonioConfig.begin();
    
    // Load configuration values into variables
    loadConfigValues();
    
    // If not in config mode, start main application
    if (!zonioConfig.isConfigMode()) {
        startMainApplication();
    } else {
        Serial.println("Configuration mode active");
        Serial.println("Connect to 'Zonio-Config' WiFi network");
        Serial.println("Open browser and go to http://192.168.4.1");
        Serial.println("Configuration will timeout in 5 minutes");
    }
}

void loop() {
    // Handle configuration interface
    if (zonioConfig.isConfigMode()) {
        zonioConfig.handle();
        
        // Check if configuration was saved and restart requested
        if (zonioConfig.shouldRestartDevice()) {
            Serial.println("Configuration saved! Restarting device...");
            zonioConfig.markRestartHandled();
            delay(1000);
            ESP.restart();
        }
        return; // Stay in config mode
    }
    
    // ===== MAIN APPLICATION LOOP =====
    runMainApplication();
}

void loadConfigValues() {
    // Load configuration from web interface into global variables
    strcpy(wifiSSID, zonioConfig.getString("wifi_ssid"));
    strcpy(wifiPassword, zonioConfig.getString("wifi_password"));
    strcpy(mqttServer, zonioConfig.getString("mqtt_server"));
    mqttPort = zonioConfig.getInt("mqtt_port");
    strcpy(mqttUsername, zonioConfig.getString("mqtt_username"));
    strcpy(mqttPassword, zonioConfig.getString("mqtt_password"));
    sensorInterval = zonioConfig.getInt("sensor_interval");
    enableMQTT = zonioConfig.getBool("enable_mqtt");
    enableDebug = zonioConfig.getBool("enable_debug");
    
    if (enableDebug) {
        Serial.println("Configuration loaded:");
        Serial.printf("WiFi: %s\n", wifiSSID);
        Serial.printf("MQTT: %s:%d (enabled: %s)\n", mqttServer, mqttPort, enableMQTT ? "Yes" : "No");
        Serial.printf("Sensor interval: %d seconds\n", sensorInterval);
        Serial.printf("Debug output: %s\n", enableDebug ? "Enabled" : "Disabled");
    }
}

void startMainApplication() {
    Serial.println("Starting main application...");
    
    // Initialize hardware
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Connect to WiFi if configured
    if (strlen(wifiSSID) > 0) {
        WiFi.begin(wifiSSID, wifiPassword);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println();
            Serial.printf("WiFi connected: %s\n", WiFi.localIP().toString().c_str());
        } else {
            Serial.println();
            Serial.println("WiFi connection failed");
        }
    }
    
    // Initialize MQTT if enabled
    if (enableMQTT && strlen(mqttServer) > 0) {
        initMQTT();
    }
    
    Serial.println("Main application started");
}

void runMainApplication() {
    static unsigned long lastSensorRead = 0;
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
    unsigned long now = millis();
    
    // Read sensors according to configured interval
    if (now - lastSensorRead > (sensorInterval * 1000)) {
        lastSensorRead = now;
        readSensors();
    }
    
    // Blink LED to show activity
    if (now - lastBlink > 1000) {
        lastBlink = now;
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
    }
    
    // Handle MQTT if enabled
    if (enableMQTT) {
        handleMQTT();
    }
    
    delay(100);
}

void readSensors() {
    // Simulate sensor reading
    float temperature = 20.0 + random(-50, 50) / 10.0; // Random temp 15-25°C
    float humidity = 50.0 + random(-100, 100) / 10.0;  // Random humidity 40-60%
    
    if (enableDebug) {
        Serial.printf("Sensors: T=%.1f°C, H=%.1f%%\n", temperature, humidity);
    }
    
    // Send to MQTT if enabled and connected
    if (enableMQTT && WiFi.status() == WL_CONNECTED) {
        sendMQTTData(temperature, humidity);
    }
}

void initMQTT() {
    if (enableDebug) {
        Serial.printf("Initializing MQTT connection to %s:%d\n", mqttServer, mqttPort);
    }
    
    // Here you would initialize your MQTT client
    // Example with PubSubClient:
    // mqttClient.setServer(mqttServer, mqttPort);
    // mqttClient.setCallback(mqttCallback);
}

void handleMQTT() {
    // Handle MQTT connection and messages
    // Example:
    // if (!mqttClient.connected()) {
    //     reconnectMQTT();
    // }
    // mqttClient.loop();
}

void sendMQTTData(float temperature, float humidity) {
    if (enableDebug) {
        Serial.printf("Sending MQTT data: T=%.1f°C, H=%.1f%%\n", temperature, humidity);
    }
    
    // Create JSON payload
    char payload[100];
    snprintf(payload, sizeof(payload), 
             "{\"temperature\":%.1f,\"humidity\":%.1f,\"device\":\"%s\"}", 
             temperature, humidity, zonioConfig.getString("wifi_ssid"));
    
    // Send to MQTT (example)
    // mqttClient.publish("zonio/sensors/data", payload);
    
    // For demo, just print what would be sent
    if (enableDebug) {
        Serial.printf("MQTT payload: %s\n", payload);
    }
}

/*
 * ===== INTEGRATION EXAMPLE FOR EXISTING FIRMWARE =====
 * 
 * To integrate ZonioMinimalConfig into your existing firmware:
 * 
 * 1. Add these lines to your global declarations:
 *    #include "ZonioMinimalConfig.h"
 *    ZonioMinimalConfig zonioConfig;
 *    bool configurationMode = true;
 * 
 * 2. Add this to the beginning of your setup():
 *    zonioConfig.setDeviceName("Your-Device-Name");
 *    zonioConfig.addParameter("wifi_ssid", "WiFi SSID", "default-ssid");
 *    zonioConfig.addParameter("wifi_password", "WiFi Password", "default-password");
 *    // ... add other parameters as needed ...
 *    zonioConfig.begin();
 *    
 *    if (!zonioConfig.isConfigMode()) {
 *        configurationMode = false;
 *        // Load your configuration values here
 *        // ... your existing setup continues ...
 *    }
 * 
 * 3. Add this to the beginning of your loop():
 *    if (configurationMode) {
 *        zonioConfig.handle();
 *        if (zonioConfig.shouldRestartDevice()) {
 *            ESP.restart();
 *        }
 *        return;  // Skip main application while in config mode
 *    }
 *    
 *    // ... your existing loop code continues unchanged ...
 * 
 * That's it! Your existing firmware now has a web configuration interface
 * with minimal code changes and maximum compatibility.
 * 
 * MEMORY USAGE:
 * - Library code: ~2.5KB flash
 * - HTML template: ~1KB flash  
 * - EEPROM storage: 512 bytes
 * - RAM usage: ~1KB (including parameters)
 * TOTAL OVERHEAD: ~3.5KB flash + 1KB RAM
 */