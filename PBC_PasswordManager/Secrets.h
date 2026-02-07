#ifndef SECRETS_H
#define SECRETS_H

#include <Arduino.h>

#define MAX_SECRETS 10       // max. Einträge
#define MAX_LENGTH 32        // max. Länge für Name, User, Passwort

struct Secret {
    char name[MAX_LENGTH];
    char user[MAX_LENGTH];
    char pass[MAX_LENGTH];
};

// Array, wird im EEPROM gespeichert oder initial per Serial befüllt
Secret secrets[MAX_SECRETS];

#endif
