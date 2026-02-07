#ifndef KEYBOARD_DE_H
#define KEYBOARD_DE_H

#include <Arduino.h>
#include <Keyboard.h>

class KeyboardDE {
public:
  void begin();
  void print(const char* text);
  void write(char c);

  // Sondertasten optional
  void pressEnter();
  void pressTab();

private:
  void sendChar(char c);
};

extern KeyboardDE kbdDE;

#endif
