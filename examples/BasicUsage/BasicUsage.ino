/*
 * ESP32WebConfig Basic Usage Example
 * 
 * This example demonstrates the basic usage of ESP32WebConfig library.
 * It creates a simple configuration interface with various parameter types.
 * 
 * Hardware:
 * - ESP32 or ESP8266 board
 * 
 * Usage:
 * 1. Upload this sketch
 * 2. Connect to "ESP32-Demo-Config" WiFi network (password: 12345678)
 * 3. Open browser and go to http://192.168.4.1
 * 4. Login with admin/admin123
 * 5. Configure parameters and save
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#include <ESP32WebConfig.h>

// Create ESP32WebConfig instance
ESP32WebConfig webConfig;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("ESP32WebConfig Basic Usage Example");
  Serial.println("===================================");
  
  // ===== BASIC CONFIGURATION =====
  
  // Set device name (appears in web interface)
  webConfig.setDeviceName("ESP32-Demo-Device");
  
  // Configure Access Point
  webConfig.setAPConfig("ESP32-Demo-Config", "12345678", IPAddress(192, 168, 4, 1));
  
  // Optional: Configure home WiFi connection
  // webConfig.setSTAConfig("Your-WiFi-SSID", "Your-WiFi-Password");
  
  // Set admin credentials for web interface
  webConfig.setCredentials("admin", "admin123");
  
  // Optional: Enable mDNS for easy access
  webConfig.setMDNS("esp32-demo");
  
  // ===== ADD CONFIGURATION PARAMETERS =====
  
  // Text parameter
  webConfig.addTextParameter("device_name", "Device Name", "ESP32-Demo", "Enter device name");
  
  // Number parameter with range
  webConfig.addNumberParameter("update_interval", "Update Interval (seconds)", 30, 5, 300, 5);
  
  // Float parameter
  webConfig.addFloatParameter("temperature_offset", "Temperature Offset (°C)", 0.0, -5.0, 5.0, 0.1);
  
  // Checkbox parameter
  webConfig.addCheckboxParameter("enable_serial", "Enable Serial Output", true);
  
  // Select parameter
  webConfig.addSelectParameter("log_level", "Log Level", 
    "[{\"0\":\"None\"},{\"1\":\"Error\"},{\"2\":\"Warning\"},{\"3\":\"Info\"},{\"4\":\"Debug\"}]", "2");
  
  // Range slider parameter
  webConfig.addRangeParameter("brightness", "LED Brightness (%)", 50, 0, 100, 5);
  
  // ===== START WEB CONFIGURATION =====
  webConfig.begin();
  
  Serial.println("");
  Serial.println("Web Configuration Started!");
  Serial.printf("Access Point: %s\n", "ESP32-Demo-Config");
  Serial.printf("IP Address: %s\n", webConfig.getAPIP().c_str());
  Serial.printf("Web Interface: http://%s\n", webConfig.getAPIP().c_str());
  Serial.printf("mDNS: http://esp32-demo.local\n");
  Serial.printf("Login: admin / admin123\n");
  Serial.println("");
  
  // Print current configuration
  printCurrentConfig();
}

void loop() {
  // Handle web configuration
  webConfig.handle();
  
  // ===== YOUR APPLICATION CODE HERE =====
  
  static unsigned long lastPrint = 0;
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  // Print configuration values every 10 seconds
  if (millis() - lastPrint > 10000) {
    lastPrint = millis();
    printCurrentConfig();
  }
  
  // Blink built-in LED based on brightness setting
  unsigned long blinkInterval = map(webConfig.getInt("brightness"), 0, 100, 2000, 100);
  if (millis() - lastBlink > blinkInterval) {
    lastBlink = millis();
    
    if (webConfig.getBool("enable_serial")) {
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
  
  delay(10);
}

void printCurrentConfig() {
  Serial.println("=== Current Configuration ===");
  Serial.printf("Device Name: %s\n", webConfig.getValue("device_name").c_str());
  Serial.printf("Update Interval: %d seconds\n", webConfig.getInt("update_interval"));
  Serial.printf("Temperature Offset: %.1f°C\n", webConfig.getFloat("temperature_offset"));
  Serial.printf("Serial Output: %s\n", webConfig.getBool("enable_serial") ? "Enabled" : "Disabled");
  Serial.printf("Log Level: %s\n", getLogLevelName(webConfig.getInt("log_level")).c_str());
  Serial.printf("LED Brightness: %d%%\n", webConfig.getInt("brightness"));
  Serial.printf("Configuration Changed: %s\n", webConfig.isConfigChanged() ? "Yes" : "No");
  Serial.printf("Parameters Count: %d\n", webConfig.getParameterCount());
  Serial.println("============================");
}

String getLogLevelName(int level) {
  switch (level) {
    case 0: return "None";
    case 1: return "Error";
    case 2: return "Warning";
    case 3: return "Info";
    case 4: return "Debug";
    default: return "Unknown";
  }
}