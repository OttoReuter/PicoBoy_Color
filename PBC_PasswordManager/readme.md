# PasswordManager auf PicoBoy Color

<img width="500" height="304" alt="grafik" src="https://github.com/user-attachments/assets/dbc03a8d-b0f5-45d0-9b70-4e9501bc7a73" />

## Grundsätzlich besteht bei der Umsetzung des Projektes folgendes Ziel:

### Ein PicoBoy Color fungiert als:

* PIN-geschützter Passwort-Tresor
* Anzeige über ST7789 TFT
* Ausgabe von User/Passwort per USB-HID-Keyboard
* Secrets werden einmalig über Serial Monitor eingegeben
* Daten werden dauerhaft im EEPROM gespeichert

### Die Initialisierung (Clean Start) funktioniert so:

1. EEPROM vollständig löschen (d.h. flash_nuke.uf2 ausführen)
2. PicoBoy Color mit gedrückter BOOTSEL-Taste anschließen
3. Sketch in der Arduino-IDE hochladen
4. in der Arduino-IDE das Board als 'Raspberry Pi Pico' mit COM-Port auswählen

### Erster Start (leeres EEPROM):

* Gerät erkennt: keine 'Secrets'
* Eingabe der Secrets über Serial-Monitor
* Gerät erkennt: kein PIN vorhanden
* Modus: SET_PIN
* 6-stellige PIN setzen
* PIN wird in EEPROM gespeichert

Die Beschreibung zur Eingabe der Secrets und Hinweise zur Bedienung finden Sie auf meiner Webseite
https://www.dgebhardt.de/arduino/pw_manager.html
