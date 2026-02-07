#include "KeyboardDE.h"

KeyboardDE kbdDE;

void KeyboardDE::begin() {
  Keyboard.begin();
}

void KeyboardDE::print(const char* text) {
  while (*text) {
    sendChar(*text++);
    delay(5);
  }
}

void KeyboardDE::write(char c) {
  sendChar(c);
}

void KeyboardDE::pressEnter() {
  Keyboard.write(KEY_RETURN);
}

void KeyboardDE::pressTab() {
  Keyboard.write(KEY_TAB);
}

// ================= DE Keymap =================
void KeyboardDE::sendChar(char c) {
  switch (c) {

    case '@':
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press('q');
      break;

    case '?':
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(0x2D); // ß
      break;

    case ')':
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('9');
      break;

    case '(':
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('8');
      break;

    case '%':
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('5');
      break;

    case '!':
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('1');
      break;

    case '+':
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('1'); // ggf. anpassen
      break;

    default:
      Keyboard.write(c);
      return;
  }

  delay(5);
  Keyboard.releaseAll();
}
