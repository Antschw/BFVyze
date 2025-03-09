//
// Created by Antoine on 09/03/2025.
//

#include <windows.h>
#include "HotkeyManager.h"
#include "Constants.h"
#include <thread>

HotkeyManager::HotkeyManager(const int hotkey) : running_(false), hotkey_(hotkey) {}

HotkeyManager::~HotkeyManager() {
    stop();
}

void HotkeyManager::start(const Callback& callback) {
    running_ = true;
    std::thread([this, callback]() {
        while (running_) {
            if (const volatile auto keyState = GetAsyncKeyState(hotkey_); keyState & KEY_PRESSED_MASK) {
                callback();
                Sleep(HOTKEY_DEBOUNCE_DELAY);
            }
            Sleep(HOTKEY_POLL_INTERVAL);
        }
    }).detach();
}

void HotkeyManager::stop() {
    running_ = false;
}

void HotkeyManager::setHotkey(const int newHotkey) {
    hotkey_.store(newHotkey, std::memory_order_relaxed);
}


int HotkeyManager::getHotkey() const {
    return hotkey_;
}
