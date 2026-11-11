#pragma once

#include <Arduino.h>

class DisplayManagerClass {
public:
    void begin();
    void update();
    void setEnabled(bool enabled);
    bool isEnabled() const;

private:
    bool _enabled = false;
    bool _ready = false;
    unsigned long _lastUpdate = 0;
};

extern DisplayManagerClass DisplayManager;
