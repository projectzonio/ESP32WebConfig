/*
 * ESP32WebConfig Library - Web Pages Templates
 * HTML, CSS, and JavaScript templates for web interface
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <Arduino.h>

class WebPages {
public:
    // Main page templates
    static String getMainPage();
    static String getConfigPage();
    static String getNetworkPage();
    static String getAdvancedPage();
    static String getBackupPage();
    
    // CSS and JavaScript
    static String getCSS();
    static String getJavaScript();
    
    // Template components
    static String getHTMLHeader(const String& title = "ESP32 Configuration");
    static String getHTMLFooter();
    static String getNavigation();
    
private:
    // Internal template builders
    static String buildFormField(const String& type, const String& name, const String& label, 
                                 const String& value = "", const String& attributes = "");
    static String buildCard(const String& title, const String& content);
    static String buildStatusGrid();
};

#endif // WEBPAGES_H