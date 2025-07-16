# ESP32WebConfig Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue.svg)](https://www.arduino.cc/reference/en/libraries/)
[![ESP32](https://img.shields.io/badge/ESP32-Compatible-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![ESP8266](https://img.shields.io/badge/ESP8266-Compatible-green.svg)](https://www.espressif.com/en/products/socs/esp8266)

Univerzální webové konfigurační rozhraní pro ESP32/ESP8266 s moderním designem a pokročilými funkcemi.

## 🚀 Hlavní funkce

- **Duální WiFi režim** - AP + STA současně
- **Konfigurovatelná IP adresa** AP (bez kolizí s domácí sítí)
- **13 typů parametrů** (text, number, checkbox, select, range, atd.)
- **Automatické EEPROM** ukládání/načítání
- **Responzivní webové rozhraní** s moderním designem
- **Real-time validace** formulářů v JavaScript
- **Callback systém** pro reakce na změny
- **Export/import konfigurace** (JSON)
- **mDNS podpora** pro snadný přístup
- **Grupování parametrů** do logických celků
- **HTTP autentifikace**
- **Captive portal** pro první konfiguraci
- **Auto-save** s konfigurovatelným zpožděním
- **Custom validátory** pro složité kontroly
- **RESTful API** endpointy
- **Dark mode** podpora
- **Mobile-first** responsive design

## 📱 Podporované platformy

- **ESP32** (všechny varianty: S2, S3, C3, C6)
- **ESP8266** (NodeMCU, Wemos D1 Mini, atd.)
- **Minimální flash requirements**: ~3.5KB flash + 1KB RAM

## 🛠️ Instalace

### Arduino IDE
1. Stáhněte ZIP soubor z GitHub
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library
3. Vyberte stažený ZIP soubor

### PlatformIO
```ini
lib_deps = 
    https://github.com/projectzonio/ESP32WebConfig.git
```

### Manuální instalace
```bash
cd ~/Arduino/libraries/
git clone https://github.com/projectzonio/ESP32WebConfig.git
```

## 📖 Rychlý start

### Základní příklad

```cpp
#include <ESP32WebConfig.h>

ESP32WebConfig webConfig;

void setup() {
    Serial.begin(115200);
    
    // Základní konfigurace
    webConfig.setDeviceName("ESP32-Demo");
    webConfig.setAPConfig("ESP32-Config", "12345678", IPAddress(192, 168, 5, 1));
    webConfig.setCredentials("admin", "admin123");
    
    // Přidání parametrů
    webConfig.addTextParameter("device_name", "Název zařízení", "ESP32-Device");
    webConfig.addNumberParameter("update_interval", "Interval (s)", 30, 5, 300);
    webConfig.addCheckboxParameter("enable_mqtt", "MQTT komunikace", true);
    
    // Spuštění
    webConfig.begin();
    
    Serial.println("Webové rozhraní spuštěno!");
    Serial.printf("AP: http://%s\n", webConfig.getAPIP().c_str());
}

void loop() {
    webConfig.handle();
    
    // Vaše aplikační logika
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        lastPrint = millis();
        
        String deviceName = webConfig.getValue("device_name");
        int interval = webConfig.getInt("update_interval");
        bool mqttEnabled = webConfig.getBool("enable_mqtt");
        
        Serial.printf("Aktuální konfigurace: %s, %ds, MQTT:%s\n", 
                     deviceName.c_str(), interval, mqttEnabled ? "ON" : "OFF");
    }
}
```

### Pokročilý příklad s callbacky

```cpp
#include <ESP32WebConfig.h>

ESP32WebConfig webConfig;

void setup() {
    Serial.begin(115200);
    
    // Konfigurace knihovny
    webConfig.setDeviceName("ESP32-Advanced");
    webConfig.setAPConfig("ESP32-Advanced-Config", "12345678");
    webConfig.setCredentials("admin", "admin123");
    webConfig.setMDNS("esp32-advanced");
    
    // Přidání parametrů do skupin
    webConfig.addParameterToGroup("basic", "device_name", "Název zařízení", PARAM_TEXT, "ESP32-Advanced");
    webConfig.addParameterToGroup("basic", "operation_mode", "Provozní režim", PARAM_SELECT, "0");
    webConfig.setParameterAttributes("operation_mode", "data-options='[{\"0\":\"Automatický\"},{\"1\":\"Manuální\"},{\"2\":\"Testovací\"}]'");
    
    webConfig.addParameterToGroup("temp", "target_temp", "Cílová teplota (°C)", PARAM_FLOAT, "25.0");
    webConfig.setParameterAttributes("target_temp", "min='15.0' max='40.0' step='0.1'");
    
    webConfig.addParameterToGroup("mqtt", "mqtt_server", "MQTT Server", PARAM_TEXT, "192.168.1.100");
    webConfig.addParameterToGroup("mqtt", "mqtt_port", "MQTT Port", PARAM_NUMBER, "1883");
    
    // Callback při změně parametru
    webConfig.onParameterChange([](String key, String oldValue, String newValue) {
        Serial.printf("Změna: %s = %s -> %s\n", key.c_str(), oldValue.c_str(), newValue.c_str());
        
        if (key == "target_temp") {
            float temp = newValue.toFloat();
            Serial.printf("Nová cílová teplota: %.1f°C\n", temp);
            // Zde aplikujte změnu do vašeho systému
        }
    });
    
    // Callback při uložení
    webConfig.onConfigSave([]() {
        Serial.println("Konfigurace uložena!");
        // Zde můžete provést restart nebo reload aplikace
    });
    
    // Custom validátor
    webConfig.setCustomValidator([](String key, String value) -> String {
        if (key == "target_temp") {
            float temp = value.toFloat();
            if (temp < 15.0 || temp > 40.0) {
                return "Teplota musí být mezi 15-40°C";
            }
        }
        return ""; // OK
    });
    
    webConfig.begin();
    
    Serial.println("Pokročilé webové rozhraní spuštěno!");
    Serial.printf("Přístup: http://%s nebo http://esp32-advanced.local\n", webConfig.getAPIP().c_str());
}

void loop() {
    webConfig.handle();
    
    // Vaše hlavní aplikace
}
```

## 📋 Podporované typy parametrů

| Typ | Popis | Příklad |
|-----|-------|---------|
| `PARAM_TEXT` | Textový input | Název zařízení |
| `PARAM_PASSWORD` | Password input (skryté) | Heslo |
| `PARAM_NUMBER` | Číselný input | Port číslo |
| `PARAM_FLOAT` | Desetinné číslo | Teplota |
| `PARAM_CHECKBOX` | Zaškrtávací pole | Povolit MQTT |
| `PARAM_SELECT` | Výběr z možností | Provozní režim |
| `PARAM_TEXTAREA` | Větší textová oblast | Poznámky |
| `PARAM_COLOR` | Color picker | Barva LED |
| `PARAM_RANGE` | Posuvník | PWM hodnota |
| `PARAM_EMAIL` | Email input | Kontaktní email |
| `PARAM_URL` | URL input | Webhook URL |
| `PARAM_TIME` | Time input | Čas spuštění |
| `PARAM_DATE` | Date input | Datum kalibrace |

## 🎯 Skupiny parametrů

Parametry můžete organizovat do logických skupin:

```cpp
// Základní nastavení
webConfig.addParameterToGroup("basic", "device_name", "Název", PARAM_TEXT, "ESP32");
webConfig.addParameterToGroup("basic", "mode", "Režim", PARAM_SELECT, "auto");

// Síťové nastavení
webConfig.addParameterToGroup("network", "wifi_ssid", "WiFi SSID", PARAM_TEXT, "");
webConfig.addParameterToGroup("network", "static_ip", "Statická IP", PARAM_TEXT, "");

// MQTT konfigurace
webConfig.addParameterToGroup("mqtt", "mqtt_server", "MQTT Server", PARAM_TEXT, "");
webConfig.addParameterToGroup("mqtt", "mqtt_port", "MQTT Port", PARAM_NUMBER, "1883");
```

Podporované skupiny:
- `basic` - 🔧 Základní nastavení
- `time` - ⏰ Časové nastavení
- `light` - 💡 Světelný cyklus
- `pwm` - 📊 PWM nastavení
- `temp` - 🌡️ Teplotní regulace
- `safety` - 🛡️ Bezpečnost
- `mqtt` - 📡 MQTT komunikace
- `general` - ⚙️ Obecné nastavení

## 🔧 API Reference

### Základní konfigurace

```cpp
void setDeviceName(const String& name);
void setAPConfig(const String& ssid, const String& password = "", IPAddress ip = IPAddress(192, 168, 4, 1));
void setSTAConfig(const String& ssid, const String& password = "", IPAddress ip = IPAddress(0, 0, 0, 0));
void setCredentials(const String& username, const String& password);
void setMDNS(const String& name);
```

### Přidání parametrů

```cpp
void addParameter(const String& key, const String& label, ParameterType type, const String& defaultValue = "");
void addTextParameter(const String& key, const String& label, const String& defaultValue = "", const String& placeholder = "");
void addNumberParameter(const String& key, const String& label, int defaultValue = 0, int min = 0, int max = 100, int step = 1);
void addFloatParameter(const String& key, const String& label, float defaultValue = 0.0, float min = 0.0, float max = 100.0, float step = 0.1);
void addCheckboxParameter(const String& key, const String& label, bool defaultValue = false);
void addSelectParameter(const String& key, const String& label, const String& options, const String& defaultValue = "");
void addRangeParameter(const String& key, const String& label, int defaultValue, int min, int max, int step = 1);
```

### Získání hodnot

```cpp
String getValue(const String& key);
int getInt(const String& key);
float getFloat(const String& key);
bool getBool(const String& key);
```

### Nastavení hodnot

```cpp
bool setValue(const String& key, const String& value);
bool setValue(const String& key, int value);
bool setValue(const String& key, float value);
bool setValue(const String& key, bool value);
```

### Callbacky

```cpp
void onParameterChange(OnParameterChangeCallback callback);
void onConfigSave(OnConfigSaveCallback callback);
void onConfigLoad(OnConfigLoadCallback callback);
void setCustomValidator(CustomValidatorCallback callback);
```

### Správa konfigurace

```cpp
bool save();
bool load();
void reset();
String exportConfig();
bool importConfig(const String& json);
```

## 🌐 Web rozhraní

### Captive Portal
Knihovna automaticky vytváří captive portal, který přesměruje všechny HTTP požadavky na konfigurační stránku.

### Responsive Design
Rozhraní je optimalizováno pro:
- **Desktop** - plné rozložení se všemi funkcemi
- **Tablet** - adaptivní layout
- **Mobile** - touch-friendly ovládání

### Dark Mode
Automatická detekce podle nastavení prohlížeče (`prefers-color-scheme: dark`)

### Real-time Validace
- Okamžitá kontrola formulářových polí
- Vizuální zpětná vazba pro chyby
- Custom validační pravidla

## 📊 RESTful API

Knihovna poskytuje REST API pro programový přístup:

### Endpointy

| Endpoint | Metoda | Popis |
|----------|--------|-------|
| `/api/status` | GET | Systémové informace |
| `/api/config` | GET | Aktuální konfigurace |
| `/get?key=param` | GET | Hodnota parametru |
| `/set` | POST | Nastavení parametru |
| `/export` | GET | Export konfigurace |
| `/import` | POST | Import konfigurace |
| `/reset` | GET | Reset na výchozí |
| `/reboot` | GET | Restart zařízení |

### Příklady API volání

```bash
# Získání všech parametrů
curl http://192.168.4.1/api/config

# Získání konkrétního parametru
curl http://192.168.4.1/get?key=device_name

# Nastavení parametru
curl -X POST http://192.168.4.1/set -d "key=target_temp&value=25.5"

# Export konfigurace
curl http://192.168.4.1/export -o config.json

# Systémové informace
curl http://192.168.4.1/api/status
```

## 💾 Úložiště konfigurace

### EEPROM
- Automatické ukládání do EEPROM
- Konfigurovatelná velikost a pozice
- Magic number validace
- Verze konfigurace

### Backup systém
- Automatické zálohy před restartem
- Historie posledních 5 záloh
- Export/import JSON formátu

## 🔒 Bezpečnost

### HTTP Autentifikace
```cpp
webConfig.setCredentials("admin", "secure_password");
```

### Validace vstupů
- Server-side validace všech parametrů
- Client-side real-time validace
- Custom validační funkce
- XSS ochrana

### Časový limit AP
- Automatické vypnutí AP po timeoutu
- Konfigurovatelný timeout (výchozí 5 minut)

## 📱 Minimální WebConfig (pro ESP8266/ESP32-C3)

Pro projekty s omezenou pamětí je k dispozici **ZonioMinimalConfig**:

```cpp
#include "ZonioMinimalConfig.h"

ZonioMinimalConfig zonioConfig;

void setup() {
    zonioConfig.setDeviceName("Zonio-Device");
    zonioConfig.addParameter("wifi_ssid", "WiFi SSID", "default-ssid");
    zonioConfig.addParameter("wifi_password", "WiFi Password", "");
    zonioConfig.addNumberParameter("interval", "Interval (s)", 30, 5, 300);
    zonioConfig.begin();
}

void loop() {
    if (zonioConfig.isConfigMode()) {
        zonioConfig.handle();
        if (zonioConfig.shouldRestartDevice()) {
            ESP.restart();
        }
        return;
    }
    
    // Vaše hlavní aplikace
}
```

**Výhody minimální verze:**
- Pouze **~3KB flash** + **1KB RAM**
- **5minutový timeout** AP režimu
- **Single HTML** stránka s inline CSS/JS
- Kompatibilní s ESP8266 a ESP32-C3

## 🚀 Použití v reálných projektech

### IoT Senzory
```cpp
// Meteostanice s BME280
webConfig.addFloatParameter("temp_offset", "Teplotní offset", 0.0);
webConfig.addNumberParameter("read_interval", "Interval čtení (s)", 60);
webConfig.addParameter("mqtt_topic", "MQTT Topic", "weather/outdoor");
```

### PWM Regulátory
```cpp
// LED/PWM ovladač
webConfig.addRangeParameter("min_pwm", "Min PWM (%)", 0, 0, 100);
webConfig.addRangeParameter("max_pwm", "Max PWM (%)", 100, 0, 100);
webConfig.addFloatParameter("target_temp", "Cílová teplota", 25.0);
webConfig.addCheckboxParameter("auto_mode", "Auto režim", true);
```

### Domácí automatizace
```cpp
// Smart switch
webConfig.addSelectParameter("mode", "Režim", 
    "[{\"timer\":\"Časovač\"},{\"manual\":\"Manuální\"},{\"auto\":\"Automatický\"}]", "auto");
webConfig.addParameter("schedule_on", "Zapnout v", PARAM_TIME, "20:00");
webConfig.addParameter("schedule_off", "Vypnout v", PARAM_TIME, "06:00");
```

## 🔗 Kompatibilita s ZONIO ekosystémem

Knihovna je navržena pro maximální kompatibilitu s [ZONIO Pipeline](https://github.com/projectzonio/zonioPipeline) projektem:

- **Unifikované API** napříč všemi ZONIO zařízeními
- **Standardní MQTT topics** pro komunikaci
- **Auto-discovery** protocol pro mesh sítě
- **Kompatibilní konfigurace** formáty

## 📚 Další příklady

V složce `examples/` najdete:

- **BasicUsage** - Základní použití knihovny
- **AdvancedDemo** - Pokročilé funkce a callbacky
- **PWMController** - PWM regulátor s web rozhraním  
- **IoTSensor** - IoT senzor s MQTT
- **MinimalConfig** - Minimální konfigurace pro ESP8266
- **ZonioIntegration** - Integrace s ZONIO projektem

## 🤝 Přispívání

1. Forkněte repozitář
2. Vytvořte feature branch (`git checkout -b feature/amazing-feature`)
3. Commitněte změny (`git commit -m 'Add amazing feature'`)
4. Pushněte branch (`git push origin feature/amazing-feature`)
5. Vytvořte Pull Request

## 📄 Licence

Tento projekt je licencován pod MIT License - viz [LICENSE](LICENSE) soubor.

## 👥 Autoři

- **ZONIO Project Team** - *Initial work* - [projectzonio](https://github.com/projectzonio)

## 🙏 Poděkování

- Arduino community za ESP32/ESP8266 core
- Všem přispěvatelům a uživatelům knihovny
- Claude 4 Sonnet za asistenci při vývoji

## 📞 Podpora

- **Issues**: [GitHub Issues](https://github.com/projectzonio/ESP32WebConfig/issues)
- **Discussions**: [GitHub Discussions](https://github.com/projectzonio/ESP32WebConfig/discussions)
- **Wiki**: [GitHub Wiki](https://github.com/projectzonio/ESP32WebConfig/wiki)

---

**ESP32WebConfig** - *Making IoT configuration simple and beautiful* 🚀