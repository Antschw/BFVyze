#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>

/// @brief Bit mask to check if a key is currently pressed in GetAsyncKeyState
constexpr int KEY_PRESSED_MASK = 0x8000;

/// @brief Delay after detecting a hotkey to prevent multiple triggers (milliseconds)
constexpr int HOTKEY_DEBOUNCE_DELAY = 300;

/// @brief Polling interval for key detection (milliseconds)
constexpr int HOTKEY_POLL_INTERVAL = 50;

/// @brief BMP bit depth for grayscale conversion
constexpr int GRAYSCALE_BIT_DEPTH = 8;

/// @brief BMP file type identifier
constexpr int BMP_HEADER_TYPE = 0x4D42;

/// @brief Path to the Python backend executable (embedded Python).
const std::string PYTHON_EXECUTABLE_PATH = "python-3.13.2-embed-amd64\\pythonw.exe";

/// @brief Path to the Python backend script.
const std::string PYTHON_SCRIPT_PATH = "python-3.13.2-embed-amd64\\Scripts\\main.py";

#endif // CONSTANTS_H
