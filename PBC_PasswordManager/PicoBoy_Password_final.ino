#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "KeyboardDE.h"
#include "PinManager.h"
#include "Secrets.h"
#include <EEPROM.h>

// ================= Pins =================
#define TFT_CS   10
#define TFT_RST  9
#define TFT_DC   8
#define TFT_BLK  26

#define NUM_KEYS 7
const uint8_t keys[NUM_KEYS] = {3, 1, 2, 4, 0, 27, 28}; // UP, DOWN, LEFT, RIGHT, CENTER, A, B

// ================= EEPROM Layout =================
#define EEPROM_PIN_START         0
#define EEPROM_SECRET_COUNT_ADDR  PIN_LENGTH
#define EEPROM_SECRET_START      (PIN_LENGTH + 1)

// ================= Debounce =================
bool lastState[NUM_KEYS];
unsigned long lastDebounce[NUM_KEYS];
#define DEBOUNCE_DELAY 200

// ================= Display & PinManager =================
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
PinManager pinManager(&tft);

// ================= Menü =================
bool menuActive = false;
int menuIndex = 0;

// ================= KeyHandled =================
bool keyHandled[NUM_KEYS] = {false};

// ================= Serial Input State =================
bool firstRun = false;
bool secretsDone = false;
char serialBuffer[3*MAX_LENGTH];
int serialIdx = 0;
int secretsEntered = 0;
bool serialDisplayShown = false; // nur einmalig

// ================= EEPROM Helper =================
void saveSecret(int index, Secret s){
    int base = EEPROM_SECRET_START + index * sizeof(Secret);
    for(int i = 0; i < sizeof(Secret); i++)
        EEPROM.write(base + i, *((uint8_t*)&s + i));

    if(index >= EEPROM.read(EEPROM_SECRET_COUNT_ADDR))
        EEPROM.write(EEPROM_SECRET_COUNT_ADDR, index+1);

    EEPROM.commit();
}

Secret loadSecret(int index){
    Secret s;
    int base = EEPROM_SECRET_START + index * sizeof(Secret);
    for(int i = 0; i < sizeof(Secret); i++)
        *((uint8_t*)&s + i) = EEPROM.read(base + i);
    return s;
}

int loadSecretCount() {
    return EEPROM.read(EEPROM_SECRET_COUNT_ADDR);
}

bool secretsStored() {
    Secret s = loadSecret(0);
    return s.name[0] != 0xFF && s.name[0] != 0x00;
}

void parseSecretLine(const char* line, Secret &s) {
    int part = 0;
    int idx = 0;
    char buffer[MAX_LENGTH] = {0};
    for(int i=0; line[i] != 0; i++){
        if(line[i] == ';'){
            buffer[idx] = 0;
            idx = 0;
            if(part == 0) strncpy(s.name, buffer, MAX_LENGTH);
            if(part == 1) strncpy(s.user, buffer, MAX_LENGTH);
            part++;
        } else {
            if(idx < MAX_LENGTH-1) buffer[idx++] = line[i];
        }
    }
    buffer[idx] = 0;
    if(part == 2) strncpy(s.pass, buffer, MAX_LENGTH);
}

// ================= Setup =================
void setup() {
    Serial.begin(115200);
    delay(300);

    // ===== Display initialisieren =====
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, LOW);
    delay(50);
    digitalWrite(TFT_RST, HIGH);

    tft.init(240,280);
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    tft.setFont(&FreeSansBold12pt7b);

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    // ===== Keys =====
    for(int i=0; i<NUM_KEYS; i++){
        pinMode(keys[i], INPUT_PULLUP);
        lastState[i] = HIGH;
        lastDebounce[i] = 0;
    }

    kbdDE.begin();
    delay(200);

    // ===== EEPROM initialisieren =====
    EEPROM.begin(EEPROM_SECRET_START + sizeof(Secret)*MAX_SECRETS);

    // ===== Prüfen, ob Secrets schon gespeichert =====
    firstRun = !secretsStored();

    // ===== Secrets aus EEPROM laden (auch für Neustart) =====
    for(int i=0;i<MAX_SECRETS;i++){
        Secret s = loadSecret(i);
        if(s.name[0] == 0xFF){
            memset(&secrets[i], 0, sizeof(Secret));
        } else {
            secrets[i] = s;
        }
    }
}

// ================= Loop =================
void loop() {
    unsigned long now = millis();

    // ===== Serial Input für Secrets (angepasst für Datei-Import) =====
    if(firstRun && !secretsDone){
        if(!serialDisplayShown){
            tft.fillScreen(ST77XX_BLACK);
            tft.setCursor(20,60);
            tft.setTextColor(ST77XX_YELLOW);
            tft.print("PasswordManager");
            tft.setCursor(40,120);
            tft.setTextColor(ST77XX_WHITE);
            tft.print("Enter Secrets");
            tft.setCursor(70,160);
            tft.setTextColor(ST77XX_WHITE);
            tft.print("via Serial");
            serialDisplayShown = true;
        }

        while(Serial.available()){
            char c = Serial.read();

            // Zeilenende erreicht?
            if(c == '\n' || c == '\r'){
                if(serialIdx == 0){
                    // Leere Zeile → Ende der Eingabe
                    secretsDone = true;

                    // PIN löschen → Setup erzwingen
                    for(int i=0;i<PIN_LENGTH;i++)
                        EEPROM.write(EEPROM_PIN_START + i, 0xFF);
                    EEPROM.commit();

                    // PIN Setup starten
                    tft.fillScreen(ST77XX_BLACK);
                    pinManager.begin(); 
                    break;
                } else {
                    // Zeile parsen und speichern
                    serialBuffer[serialIdx] = 0;
                    Secret s = {};
                    parseSecretLine(serialBuffer, s);
                    saveSecret(secretsEntered, s);
                    secrets[secretsEntered++] = s;
                    Serial.print("Saved Secret #"); Serial.println(secretsEntered);

                    serialIdx = 0; // für nächste Zeile zurücksetzen
                }
            } else {
                // Zeichen speichern
                if(serialIdx < sizeof(serialBuffer)-1)
                    serialBuffer[serialIdx++] = c;
            }
        }

        return; // nichts anderes machen, bis Secrets fertig
    }

    // ===== Key Handling =====
    for(int i=0;i<NUM_KEYS;i++){
        bool reading = digitalRead(keys[i]);

        if(reading==LOW && !keyHandled[i] && now - lastDebounce[i] > DEBOUNCE_DELAY){
            if(menuActive) handleMenuKey(i);
            else pinManager.handleKey(i);

            keyHandled[i] = true;
            lastDebounce[i] = now;
        }

        if(reading==HIGH) keyHandled[i] = false;
        lastState[i] = reading;
    }

    if(!menuActive) pinManager.updateDisplay();

    if(pinManager.isUnlocked() && !menuActive){
        menuActive = true;
        menuIndex = 0;
        drawSecretDetail();
    }
}

// ================= Menü Funktionen =================
int secretsCount() {
    int count = 0;
    for(int i = 0; i < MAX_SECRETS; i++){
        if(secrets[i].name[0] != '\0') count++;
    }
    return count;
}

int getValidSecretIndex(int menuPosition) {
    int count = -1;
    for(int i = 0; i < MAX_SECRETS; i++){
        if(secrets[i].name[0] != '\0'){
            count++;
            if(count == menuPosition) return i;
        }
    }
    return 0;
}

void drawSecretDetail() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(20, 40);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("PasswordManager");
    
    int maxChars = 26;
    int idx = getValidSecretIndex(menuIndex);
    
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(10,100);
    String sName = secrets[idx].name;
    if(sName.length() > maxChars) sName = sName.substring(0,maxChars-1) + "…";
    tft.print(sName);

    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10,135);
    String sUser = secrets[idx].user;
    if(sUser.length() > maxChars) sUser = sUser.substring(0,maxChars-1) + "…";
    tft.print(sUser);

    tft.setCursor(10,170);
    for(int i=0;i<maxChars;i++) tft.print("*");
    if(strlen(secrets[idx].pass) > maxChars);
}

void handleMenuKey(int keyIndex) {
    int count = secretsCount();
    if(count == 0) return;

    // UP = nach oben scrollen
    if(keyIndex == 0) {
        menuIndex = (menuIndex == 0) ? count - 1 : menuIndex - 1;
    }
    // DOWN = nach unten scrollen
    else if(keyIndex == 1) {
        menuIndex = (menuIndex == count - 1) ? 0 : menuIndex + 1;
    }

    drawSecretDetail();

    int idx = getValidSecretIndex(menuIndex);
    if(keyIndex == 5) kbdDE.print(secrets[idx].user);
    if(keyIndex == 6) kbdDE.print(secrets[idx].pass);
}
