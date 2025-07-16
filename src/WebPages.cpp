/*
 * ESP32WebConfig Library - Web Pages Implementation
 * HTML, CSS, and JavaScript templates for web interface
 * 
 * Author: ZONIO Project Team
 * License: MIT
 */

#include "WebPages.h"

String WebPages::getHTMLHeader(const String& title) {
    return R"(
<!DOCTYPE html>
<html lang='cs'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>)" + title + R"(</title>
    <style>)" + getCSS() + R"(</style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>🚀 %DEVICE_NAME%</h1>
            <div class='subtitle'>ESP32WebConfig v%VERSION%</div>
        </div>
        )" + getNavigation() + R"(
)";
}

String WebPages::getHTMLFooter() {
    return R"(
    </div>
    <script>)" + getJavaScript() + R"(</script>
</body>
</html>
)";
}

String WebPages::getNavigation() {
    return R"(
        <nav class='nav'>
            <a href='/' class='nav-btn'>📊 Status</a>
            <a href='/config' class='nav-btn'>⚙️ Configuration</a>
            <a href='/network' class='nav-btn'>🌐 Network</a>
            <a href='/advanced' class='nav-btn'>🔧 Advanced</a>
            <a href='/backup' class='nav-btn'>💾 Backup</a>
        </nav>
    )";
}

String WebPages::getCSS() {
    return R"(
        :root {
            --primary-color: #2196F3;
            --primary-dark: #1976D2;
            --success-color: #4CAF50;
            --warning-color: #FF9800;
            --danger-color: #F44336;
            --background: #f5f5f5;
            --card-bg: #ffffff;
            --text-color: #333;
            --border-color: #e0e0e0;
            --shadow: 0 2px 8px rgba(0,0,0,0.1);
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: var(--background);
            color: var(--text-color);
            line-height: 1.6;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }

        .header {
            background: linear-gradient(135deg, var(--primary-color), var(--primary-dark));
            color: white;
            padding: 20px;
            border-radius: 12px;
            margin-bottom: 30px;
            box-shadow: var(--shadow);
        }

        .header h1 {
            font-size: 1.8em;
            margin-bottom: 8px;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .header .subtitle {
            opacity: 0.9;
            font-size: 0.95em;
        }

        .nav {
            display: flex;
            gap: 10px;
            margin-bottom: 30px;
            flex-wrap: wrap;
        }

        .nav-btn {
            padding: 12px 20px;
            background: var(--card-bg);
            color: var(--primary-color);
            text-decoration: none;
            border-radius: 8px;
            border: 2px solid var(--primary-color);
            transition: all 0.3s ease;
            font-weight: 500;
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .nav-btn:hover {
            background: var(--primary-color);
            color: white;
            transform: translateY(-2px);
            box-shadow: var(--shadow);
        }

        .nav-btn.active {
            background: var(--primary-color);
            color: white;
        }

        .card {
            background: var(--card-bg);
            border-radius: 12px;
            padding: 25px;
            margin-bottom: 25px;
            box-shadow: var(--shadow);
            border: 1px solid var(--border-color);
        }

        .card-title {
            font-size: 1.3em;
            font-weight: 600;
            margin-bottom: 20px;
            color: var(--primary-color);
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .status-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }

        .status-item {
            background: var(--card-bg);
            padding: 20px;
            border-radius: 10px;
            border-left: 4px solid var(--primary-color);
            box-shadow: var(--shadow);
        }

        .status-item.success { border-left-color: var(--success-color); }
        .status-item.warning { border-left-color: var(--warning-color); }
        .status-item.danger { border-left-color: var(--danger-color); }

        .status-label {
            font-weight: 600;
            color: var(--text-color);
            margin-bottom: 5px;
        }

        .status-value {
            font-size: 1.1em;
            color: var(--primary-color);
        }

        .status-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 8px;
        }

        .status-indicator.online { background: var(--success-color); }
        .status-indicator.offline { background: var(--danger-color); }

        .form-group {
            margin-bottom: 25px;
        }

        .form-group.grouped {
            background: #f9f9f9;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 30px;
        }

        .group-title {
            font-weight: 600;
            color: var(--primary-color);
            margin-bottom: 15px;
            font-size: 1.1em;
            border-bottom: 2px solid var(--border-color);
            padding-bottom: 8px;
        }

        label {
            display: block;
            margin-bottom: 8px;
            font-weight: 500;
            color: var(--text-color);
        }

        .label-description {
            font-size: 0.85em;
            color: #666;
            font-weight: normal;
            margin-top: 3px;
        }

        input, select, textarea {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid var(--border-color);
            border-radius: 8px;
            font-size: 1em;
            transition: border-color 0.3s ease, box-shadow 0.3s ease;
            background: white;
        }

        input:focus, select:focus, textarea:focus {
            outline: none;
            border-color: var(--primary-color);
            box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1);
        }

        input[type="range"] {
            padding: 8px;
            height: 40px;
        }

        input[type="checkbox"] {
            width: auto;
            margin-right: 10px;
            transform: scale(1.2);
        }

        .checkbox-container {
            display: flex;
            align-items: center;
            margin-top: 5px;
        }

        .btn {
            padding: 12px 25px;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            font-weight: 500;
            cursor: pointer;
            text-decoration: none;
            display: inline-flex;
            align-items: center;
            gap: 8px;
            transition: all 0.3s ease;
            text-align: center;
        }

        .btn-primary {
            background: var(--primary-color);
            color: white;
        }

        .btn-primary:hover {
            background: var(--primary-dark);
            transform: translateY(-2px);
            box-shadow: var(--shadow);
        }

        .btn-success {
            background: var(--success-color);
            color: white;
        }

        .btn-warning {
            background: var(--warning-color);
            color: white;
        }

        .btn-danger {
            background: var(--danger-color);
            color: white;
        }

        .btn-secondary {
            background: #6c757d;
            color: white;
        }

        .btn-group {
            display: flex;
            gap: 10px;
            flex-wrap: wrap;
            margin-top: 20px;
        }

        .alert {
            padding: 15px 20px;
            border-radius: 8px;
            margin-bottom: 20px;
            border-left: 4px solid;
        }

        .alert-success {
            background: #d4edda;
            color: #155724;
            border-left-color: var(--success-color);
        }

        .alert-warning {
            background: #fff3cd;
            color: #856404;
            border-left-color: var(--warning-color);
        }

        .alert-danger {
            background: #f8d7da;
            color: #721c24;
            border-left-color: var(--danger-color);
        }

        .alert-info {
            background: #cce7ff;
            color: #004085;
            border-left-color: var(--primary-color);
        }

        .table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
            background: white;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: var(--shadow);
        }

        .table th, .table td {
            padding: 12px 15px;
            text-align: left;
            border-bottom: 1px solid var(--border-color);
        }

        .table th {
            background: #f8f9fa;
            font-weight: 600;
            color: var(--text-color);
        }

        .table tr:hover {
            background: #f8f9fa;
        }

        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 2px solid #f3f3f3;
            border-top: 2px solid var(--primary-color);
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }

        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }

        .range-value {
            display: inline-block;
            margin-left: 10px;
            font-weight: 600;
            color: var(--primary-color);
        }

        .validation-error {
            color: var(--danger-color);
            font-size: 0.85em;
            margin-top: 5px;
            display: none;
        }

        .form-group.error input,
        .form-group.error select,
        .form-group.error textarea {
            border-color: var(--danger-color);
        }

        .form-group.error .validation-error {
            display: block;
        }

        @media (max-width: 768px) {
            .container {
                padding: 15px;
            }
            
            .nav {
                justify-content: center;
            }
            
            .nav-btn {
                flex: 1;
                justify-content: center;
                min-width: 120px;
            }
            
            .status-grid {
                grid-template-columns: 1fr;
            }
            
            .btn-group {
                justify-content: center;
            }
            
            .card {
                padding: 20px;
            }
        }

        /* Dark mode support */
        @media (prefers-color-scheme: dark) {
            :root {
                --background: #1a1a1a;
                --card-bg: #2d2d2d;
                --text-color: #e0e0e0;
                --border-color: #404040;
            }
            
            input, select, textarea {
                background: #3d3d3d;
                color: #e0e0e0;
            }
        }
    )";
}

String WebPages::getJavaScript() {
    return R"(
        // Real-time form validation and updates
        document.addEventListener('DOMContentLoaded', function() {
            // Auto-save draft to localStorage (disabled in artifacts)
            const form = document.getElementById('configForm');
            if (form) {
                const inputs = form.querySelectorAll('input, select, textarea');
                
                // Save on change
                inputs.forEach(input => {
                    input.addEventListener('input', function() {
                        validateField(input);
                        updateRangeValue(input);
                    });
                });
            }
            
            // Range input value display
            document.querySelectorAll('input[type="range"]').forEach(updateRangeValue);
            
            // Auto-refresh status
            setInterval(refreshStatus, 30000);
        });
        
        function updateRangeValue(input) {
            if (input.type === 'range') {
                let display = input.parentElement.querySelector('.range-value');
                if (!display) {
                    display = document.createElement('span');
                    display.className = 'range-value';
                    input.parentElement.appendChild(display);
                }
                display.textContent = input.value + (input.dataset.unit || '');
            }
        }
        
        function validateField(input) {
            const group = input.closest('.form-group');
            const error = group.querySelector('.validation-error');
            
            // Basic validation
            let isValid = true;
            let message = '';
            
            if (input.required && !input.value.trim()) {
                isValid = false;
                message = 'This field is required';
            } else if (input.type === 'number') {
                const num = parseFloat(input.value);
                const min = parseFloat(input.min);
                const max = parseFloat(input.max);
                
                if (isNaN(num)) {
                    isValid = false;
                    message = 'Please enter a valid number';
                } else if (min !== undefined && num < min) {
                    isValid = false;
                    message = 'Minimum value is ' + min;
                } else if (max !== undefined && num > max) {
                    isValid = false;
                    message = 'Maximum value is ' + max;
                }
            }
            
            // Update UI
            if (isValid) {
                group.classList.remove('error');
            } else {
                group.classList.add('error');
                if (error) error.textContent = message;
            }
            
            return isValid;
        }
        
        function saveConfig() {
            const form = document.getElementById('configForm');
            if (!form) return;
            
            // Validate all fields
            const inputs = form.querySelectorAll('input[required], input[type="number"]');
            let allValid = true;
            
            inputs.forEach(input => {
                if (!validateField(input)) {
                    allValid = false;
                }
            });
            
            if (!allValid) {
                alert('Please fix errors in the form before saving.');
                return;
            }
            
            // Confirm save
            if (!confirm('Save configuration and restart device?')) {
                return;
            }
            
            // Submit form
            form.submit();
        }
        
        function resetConfig() {
            if (!confirm('Really reset to default settings? All data will be lost!')) {
                return;
            }
            
            window.location.href = '/reset';
        }
        
        function refreshStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update status indicators
                    updateStatusIndicators(data);
                })
                .catch(error => {
                    console.log('Status refresh failed:', error);
                });
        }
        
        function updateStatusIndicators(data) {
            // Update WiFi status
            const wifiIndicator = document.getElementById('wifi-status');
            if (wifiIndicator) {
                wifiIndicator.className = 'status-indicator ' + (data.wifi_connected ? 'online' : 'offline');
            }
            
            // Update other status elements as needed
        }
        
        function exportConfig() {
            window.location.href = '/export';
        }
        
        function importConfig() {
            const input = document.createElement('input');
            input.type = 'file';
            input.accept = '.json';
            input.onchange = function(e) {
                const file = e.target.files[0];
                if (file) {
                    const reader = new FileReader();
                    reader.onload = function(e) {
                        const formData = new FormData();
                        formData.append('config', e.target.result);
                        
                        fetch('/import', {
                            method: 'POST',
                            body: formData
                        })
                        .then(response => response.text())
                        .then(result => {
                            alert('Configuration imported!');
                            location.reload();
                        })
                        .catch(error => {
                            alert('Import error: ' + error);
                        });
                    };
                    reader.readAsText(file);
                }
            };
            input.click();
        }
    )";
}

String WebPages::getMainPage() {
    return getHTMLHeader("ESP32 Configuration - Status") + R"(
        <div class='status-grid'>
            <div class='status-item success'>
                <div class='status-label'>WiFi Connection</div>
                <div class='status-value'>
                    <span id='wifi-status' class='status-indicator online'></span>
                    %WIFI_SSID%
                </div>
            </div>
            
            <div class='status-item'>
                <div class='status-label'>IP Address (STA)</div>
                <div class='status-value'>%WIFI_IP%</div>
            </div>
            
            <div class='status-item'>
                <div class='status-label'>AP Address</div>
                <div class='status-value'>%AP_IP%</div>
            </div>
            
            <div class='status-item'>
                <div class='status-label'>Free Memory</div>
                <div class='status-value'>%FREE_HEAP% B</div>
            </div>
            
            <div class='status-item'>
                <div class='status-label'>Uptime</div>
                <div class='status-value'>%UPTIME% s</div>
            </div>
            
            <div class='status-item'>
                <div class='status-label'>Parameters</div>
                <div class='status-value'>%PARAM_COUNT%</div>
            </div>
        </div>

        <div class='card'>
            <div class='card-title'>📊 System Overview</div>
            
            <div class='alert alert-info'>
                <strong>📱 Web Interface Active</strong><br>
                System is ready for operation. Go to Configuration tab to change settings.
            </div>
            
            <table class='table'>
                <thead>
                    <tr>
                        <th>Component</th>
                        <th>Status</th>
                        <th>Information</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>Web Server</td>
                        <td><span class='status-indicator online'></span> Online</td>
                        <td>Port 80, HTTP authentication active</td>
                    </tr>
                    <tr>
                        <td>EEPROM Storage</td>
                        <td><span class='status-indicator online'></span> Active</td>
                        <td>Auto-save enabled</td>
                    </tr>
                    <tr>
                        <td>mDNS</td>
                        <td><span class='status-indicator online'></span> Active</td>
                        <td>%DEVICE_NAME%.local</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <div class='card'>
            <div class='card-title'>🔗 Quick Actions</div>
            <div class='btn-group'>
                <a href='/config' class='btn btn-primary'>⚙️ Configuration</a>
                <a href='/export' class='btn btn-secondary'>💾 Export Settings</a>
                <a href='/reboot' class='btn btn-warning' onclick='return confirm("Really restart device?")'>🔄 Restart</a>
            </div>
        </div>
    )" + getHTMLFooter();
}

String WebPages::getConfigPage() {
    return getHTMLHeader("ESP32 Configuration") + R"(
        <div class='card'>
            <div class='card-title'>⚙️ Device Configuration</div>
            
            <form id='configForm' method='POST' action='/save'>
                <div class='form-group'>
                    <label for='device_name'>Device Name</label>
                    <input type='text' id='device_name' name='device_name' value='%DEVICE_NAME%' required>
                    <div class='validation-error'></div>
                </div>
                
                <div class='form-group'>
                    <label for='update_interval'>Update Interval (seconds)</label>
                    <input type='number' id='update_interval' name='update_interval' value='30' min='5' max='3600' step='5' required>
                    <div class='validation-error'></div>
                </div>
                
                <div class='form-group'>
                    <label for='enable_debug'>Enable Debug Output</label>
                    <div class='checkbox-container'>
                        <input type='checkbox' id='enable_debug' name='enable_debug' value='true'>
                        <label for='enable_debug'>Enable serial debug output</label>
                    </div>
                </div>
                
                <div class='btn-group'>
                    <button type='button' class='btn btn-success' onclick='saveConfig()'>💾 Save Configuration</button>
                    <button type='button' class='btn btn-secondary' onclick='location.reload()'>🔄 Reload</button>
                    <button type='button' class='btn btn-danger' onclick='resetConfig()'>🗑️ Reset to Defaults</button>
                </div>
            </form>
        </div>
    )" + getHTMLFooter();
}

String WebPages::getNetworkPage() {
    return getHTMLHeader("Network Configuration") + R"(
        <div class='card'>
            <div class='card-title'>🌐 Network Interface</div>
            
            <table class='table'>
                <thead>
                    <tr>
                        <th>Interface</th>
                        <th>Status</th>
                        <th>IP Address</th>
                        <th>MAC Address</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>WiFi STA</td>
                        <td><span class='status-indicator online'></span> Connected</td>
                        <td>%WIFI_IP%</td>
                        <td>%WIFI_MAC%</td>
                    </tr>
                    <tr>
                        <td>WiFi AP</td>
                        <td><span class='status-indicator online'></span> Active</td>
                        <td>%AP_IP%</td>
                        <td>%AP_MAC%</td>
                    </tr>
                </tbody>
            </table>
        </div>
    )" + getHTMLFooter();
}

String WebPages::getAdvancedPage() {
    return getHTMLHeader("Advanced Settings") + R"(
        <div class='card'>
            <div class='card-title'>🔧 Advanced Settings</div>
            
            <div class='alert alert-warning'>
                <strong>⚠️ Warning</strong><br>
                Changing advanced settings may affect device stability. Proceed only if you understand the implications.
            </div>
            
            <div class='btn-group'>
                <button class='btn btn-info' onclick='showSystemLogs()'>📋 Show Logs</button>
                <button class='btn btn-warning' onclick='clearLogs()'>🗑️ Clear Logs</button>
                <button class='btn btn-danger' onclick='factoryReset()'>🏭 Factory Reset</button>
            </div>
        </div>
    )" + getHTMLFooter();
}

String WebPages::getBackupPage() {
    return getHTMLHeader("Backup & Restore") + R"(
        <div class='card'>
            <div class='card-title'>💾 Configuration Backup</div>
            
            <div class='alert alert-info'>
                <strong>💡 Tip:</strong> Regularly backup your configuration before making changes.
                Backup contains all settings in JSON format.
            </div>
            
            <div class='form-group'>
                <label>Export Configuration:</label>
                <div class='btn-group'>
                    <button class='btn btn-success' onclick='exportConfig()'>📤 Download Backup</button>
                    <button class='btn btn-info' onclick='viewConfig()'>👁️ View JSON</button>
                </div>
            </div>
            
            <div class='form-group'>
                <label for='import_file'>Import Configuration:</label>
                <input type='file' id='import_file' accept='.json' onchange='importConfig()'>
                <div class='label-description'>Select JSON file with configuration</div>
            </div>
        </div>
    )" + getHTMLFooter();
}

String WebPages::buildCard(const String& title, const String& content) {
    return R"(
        <div class='card'>
            <div class='card-title'>)" + title + R"(</div>
            )" + content + R"(
        </div>
    )";
}

String WebPages::buildFormField(const String& type, const String& name, const String& label, 
                               const String& value, const String& attributes) {
    String html = R"(
        <div class='form-group'>
            <label for=')" + name + R"('>)" + label + R"(</label>
    )";
    
    if (type == "checkbox") {
        html += R"(
            <div class='checkbox-container'>
                <input type='checkbox' id=')" + name + R"(' name=')" + name + R"(' value='true' )" + attributes + R"(>
                <label for=')" + name + R"('>)" + label + R"(</label>
            </div>
        )";
    } else {
        html += R"(
            <input type=')" + type + R"(' id=')" + name + R"(' name=')" + name + R"(' value=')" + value + R"(' )" + attributes + R"(>
        )";
    }
    
    html += R"(
            <div class='validation-error'></div>
        </div>
    )";
    
    return html;
}

String WebPages::buildStatusGrid() {
    return R"(
        <div class='status-grid'>
            <div class='status-item success'>
                <div class='status-label'>System Status</div>
                <div class='status-value'>
                    <span class='status-indicator online'></span>
                    Online
                </div>
            </div>
        </div>
    )";
}