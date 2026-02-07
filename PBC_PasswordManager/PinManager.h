#ifndef PINMANAGER_H
#define PINMANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <EEPROM.h>

#define PIN_LENGTH 6

class PinManager {
public:
    PinManager(Adafruit_ST7789* display);
    void begin();
    void handleKey(uint8_t index);
    void updateDisplay();
    bool isUnlocked();

private:
    Adafruit_ST7789* tft;
    uint8_t pinDigits[PIN_LENGTH];       // aktuelle Ziffern
    uint8_t pinIndex;                    // aktive Ziffer
    bool cursorVisible;                  // für blinkenden Cursor
    unsigned long lastBlinkTime;         // Timer für Cursor
    enum State { SET_PIN, ENTER_PIN, UNLOCKED } state;

    bool checkPin();
    void drawPinScreen();
    void savePinEEPROM();
    void loadPinEEPROM();
    bool isPinSet();

    bool confirmed[PIN_LENGTH];          // markiert abgeschlossene Ziffern

    void resetPinInput();                // <-- Deklaration hinzugefügt
};

#endif
