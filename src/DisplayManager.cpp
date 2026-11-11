#include "DisplayManager.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "AppPreferences.h"
#include "WifiNetworkList.h"
#include "WifiDeviceList.h"
#include "BLEDeviceList.h"
#include "WifiDetect.h"
#include "BLEDetect.h"

extern WifiNetworkList ssidList;
extern WifiDeviceList stationsList;
extern BLEDeviceList bleDeviceList;
extern AppPreferencesData appPrefs;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define I2C_SDA_PIN 5
#define I2C_SCL_PIN 6

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DisplayManagerClass DisplayManager;

void DisplayManagerClass::begin() {
    _enabled = true;
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 init failed");
        _ready = false;
        return;
    }
    _ready = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Sneak32");
    display.println("Starting...");
    display.display();
}

void DisplayManagerClass::setEnabled(bool enabled) {
    _enabled = enabled;
    if (_ready && !_enabled) {
        display.clearDisplay();
        display.display();
    }
}

bool DisplayManagerClass::isEnabled() const {
    return _enabled;
}

static const char* modeName(uint8_t mode) {
    switch (mode) {
        case OPERATION_MODE_OFF: return "OFF";
        case OPERATION_MODE_SCAN: return "SCAN";
        case OPERATION_MODE_DETECTION: return "DETECT";
        default: return "?";
    }
}

void DisplayManagerClass::update() {
    if (!_enabled || !_ready) return;
    unsigned long now = millis();
    if (now - _lastUpdate < 1000) return;
    _lastUpdate = now;

    size_t ssids = ssidList.size();
    size_t stations = stationsList.size();
    size_t ble = bleDeviceList.size();

    bool alarm = WifiDetector.isSomethingDetected() || BLEDetector.isSomethingDetected();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print(appPrefs.device_name);
    display.setCursor(0, 10);
    display.print("Mode:");
    display.print(modeName(appPrefs.operation_mode));
    if (appPrefs.stealth_mode) display.print(" ST");
    if (appPrefs.passive_scan) display.print(" P");

    display.setCursor(0, 22);
    display.print("WiFi AP:");
    display.print(ssids);
    display.print(" Dev:");
    display.print(stations);

    display.setCursor(0, 32);
    display.print("BLE Dev:");
    display.print(ble);

    display.setCursor(0, 44);
    if (appPrefs.operation_mode == OPERATION_MODE_DETECTION) {
        display.print("Alarm:");
        display.print(alarm ? "YES" : "no");
        display.print(" D:");
        display.print(WifiDetector.getDetectedDevicesCount() + BLEDetector.getDetectedDevicesCount());
    } else {
        display.print("Heap:");
        display.print(ESP.getFreeHeap() / 1024);
        display.print("k Up:");
        display.print(now / 1000);
        display.print("s");
    }

    display.setCursor(0, 56);
    display.print("COM13 2.4GHz scan");

    display.display();
}
