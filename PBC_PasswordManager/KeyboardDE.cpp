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

// ======================================================
//                DE KEYMAP
// ======================================================
void KeyboardDE::sendChar(char c) {

  switch (c) {

    // =========================
    // Y / Z Tausch
    // =========================
    case 'z': Keyboard.write('y'); return;
    case 'Z': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('y'); break;
    case 'y': Keyboard.write('z'); return;
    case 'Y': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('z'); break;

    // =========================
    // AltGr Kombinationen
    // =========================
    case '@': Keyboard.press(KEY_RIGHT_ALT); Keyboard.press('q'); break;
    case '{': Keyboard.press(KEY_RIGHT_ALT); Keyboard.press('7'); break;
    case '[': Keyboard.press(KEY_RIGHT_ALT); Keyboard.press('8'); break;
    case ']': Keyboard.press(KEY_RIGHT_ALT); Keyboard.press('9'); break;
    case '}': Keyboard.press(KEY_RIGHT_ALT); Keyboard.press('0'); break;
    
    // =========================
    // Shift Sonderzeichen
    // =========================
    case '!': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('1'); break;
    //case '§': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('3'); break;
    case '$': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('4'); break;
    case '%': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('5'); break;
    case '&': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('6'); break;
    case '/': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('7'); break;
    case '(': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('8'); break;
    case ')': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('9'); break;
    case '=': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('0'); break;
    case '?': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(0x5F); break;
    case '*': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(0x7D); break;
    case ':': Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('.'); break;
    
    default:
      Keyboard.write(c);
      return;
  }

  delay(5);
  Keyboard.releaseAll();
}
