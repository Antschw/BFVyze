//
// Created by Antoine on 09/03/2025.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

/// @brief Bit mask to check if a key is currently pressed in GetAsyncKeyState
constexpr int KEY_PRESSED_MASK = 0x8000;

/// @brief Delay after detecting a hotkey to prevent multiple triggers (milliseconds)
constexpr int HOTKEY_DEBOUNCE_DELAY = 300;

/// @brief Polling interval for key detection (milliseconds)
constexpr int HOTKEY_POLL_INTERVAL = 50;

#endif // CONSTANTS_H
