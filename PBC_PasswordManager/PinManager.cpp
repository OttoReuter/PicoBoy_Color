#include "PinManager.h"

// ================= Konstruktor =================
PinManager::PinManager(Adafruit_ST7789* display) {
    tft = display;
    pinIndex = 0;
    state = ENTER_PIN;
    cursorVisible = true;
    lastBlinkTime = millis();
    for(int i=0;i<PIN_LENGTH;i++){
        pinDigits[i]=0;
        confirmed[i]=false;
    }
}

// ================= begin =================
void PinManager::begin() {
    if (!isPinSet()) {
        state = SET_PIN;
    } else {
        state = ENTER_PIN;
        loadPinEEPROM();
    }
    resetPinInput();   // <-- PIN-Eingabe sauber initialisieren
    drawPinScreen();
}

// ================= prüfen, ob PIN gesetzt =================
bool PinManager::isPinSet() {
    for(int i=0;i<PIN_LENGTH;i++)
        if(EEPROM.read(i) != 0xFF) return true;
    return false;
}

// ================= Key Handling =================
void PinManager::handleKey(uint8_t index) {
    // LEFT = Abbruch / Reset
    if(index == 2){
        resetPinInput();
        return;
    }

    // UP = Ziffer erhöhen
    if(index == 0){
        pinDigits[pinIndex] = (pinDigits[pinIndex]+1)%10;
        drawPinScreen();
        return;
    }

    // DOWN = Ziffer verringern
    if(index == 1){
        pinDigits[pinIndex] = (pinDigits[pinIndex]+9)%10;
        drawPinScreen();
        return;
    }
   
    // CENTER = bestätigen / nächste Stelle
    if(index == 4){
        confirmed[pinIndex] = true;
        pinIndex++;
        if(pinIndex >= PIN_LENGTH){
            if(state == SET_PIN){
                savePinEEPROM();
                state = UNLOCKED;
                resetPinInput(); // Display sauber zurücksetzen
            } else if(state == ENTER_PIN){
                if(checkPin()) {
                    state = UNLOCKED;
                    resetPinInput(); // Display sauber zurücksetzen
                }
                else { // Fehler → Reset
                    resetPinInput();
                }
            }
        }
        drawPinScreen();
    }
}

// ================= PIN-Eingabe zurücksetzen =================
void PinManager::resetPinInput() {
    pinIndex = 0;
    for(int i=0;i<PIN_LENGTH;i++){
        pinDigits[i] = 0;
        confirmed[i] = false;
    }
}

// ================= Blinkender Cursor =================
void PinManager::updateDisplay() {
    if(millis() - lastBlinkTime > 500){
        cursorVisible = !cursorVisible;
        lastBlinkTime = millis();
        drawPinScreen();
    }
}

// ================= Prüfen PIN =================
bool PinManager::checkPin() {
    for(int i=0;i<PIN_LENGTH;i++)
        if(pinDigits[i] != EEPROM.read(i)) return false;
    return true;
}

// ================= EEPROM speichern =================
void PinManager::savePinEEPROM() {
    for(int i=0;i<PIN_LENGTH;i++)
        EEPROM.write(i, pinDigits[i]);
    EEPROM.commit();
}

// ================= EEPROM laden =================
void PinManager::loadPinEEPROM() {
    for(int i=0;i<PIN_LENGTH;i++)
        pinDigits[i] = EEPROM.read(i);
}

// ================= Freigabe prüfen =================
bool PinManager::isUnlocked() {
    return state == UNLOCKED;
}

// ================= PIN-Display =================
void PinManager::drawPinScreen() {
    // Überschrift 
    tft->setCursor(20, 40);
    tft->setTextColor(ST77XX_YELLOW);
    tft->print("PasswordManager");
    tft->setCursor(20, 100);
    tft->setTextColor(ST77XX_WHITE);
    if(state == SET_PIN) tft->print("Set PIN:");
    else tft->print("Enter PIN:");

    // Zahlenreihe
    int x = 20;
    int y = 150;
    int spacing = 18;
    int w = 18;   // Breite der Ziffern
    int h = 22;   // Höhe

    for(int i=0;i<PIN_LENGTH;i++){
        int digitX = x + i*spacing;

        // alten Inhalt löschen
        tft->fillRect(digitX - 2, y - 18, spacing, 24, ST77XX_BLACK);

        if(confirmed[i]){
            tft->setCursor(digitX, y);
            tft->setTextColor(ST77XX_WHITE);
            tft->print("*");
        }
        else if(i==pinIndex){
            tft->setCursor(digitX, y);
            tft->setTextColor(ST77XX_YELLOW);
            char c = cursorVisible ? ('0'+pinDigits[i]) : '_';
            tft->print(c);
        }
        else{
            tft->setCursor(digitX, y);
            tft->setTextColor(ST77XX_WHITE);
            tft->print("_");
        }
    }
}
