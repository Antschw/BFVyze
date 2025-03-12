#include <windows.h>
#include <spdlog/spdlog.h>
#include <thread>
#include "input/HotkeyManager.h"
#include "core/Constants.h"

namespace input {
    HotkeyManager::HotkeyManager(const int hotkey) : running_(false), hotkey_(hotkey) {
        spdlog::info("HotkeyManager created with hotkey: {}", hotkey);
    }

    HotkeyManager::~HotkeyManager() {
        spdlog::info("HotkeyManager destructor called. Stopping manager.");
        stop();
    }

    void HotkeyManager::start(const Callback& callback) {
        spdlog::info("Starting HotkeyManager.");
        running_ = true;
        std::thread([this, callback]() {
            spdlog::info("Hotkey polling thread started.");
            while (running_) {
                // Use .load() to get the atomic value for formatting.
                if (const volatile auto keyState = GetAsyncKeyState(hotkey_.load()); keyState & KEY_PRESSED_MASK) {
                    spdlog::info("Hotkey {} pressed. Executing callback.", hotkey_.load());
                    callback();
                    Sleep(HOTKEY_DEBOUNCE_DELAY); // Anti-debounce delay
                }
                Sleep(HOTKEY_POLL_INTERVAL);
            }
            spdlog::info("Hotkey polling thread exiting.");
        }).detach();
    }

    void HotkeyManager::stop() {
        spdlog::info("Stopping HotkeyManager.");
        running_ = false;
    }

    void HotkeyManager::setHotkey(const int newHotkey) {
        int oldHotkey = hotkey_.load();
        hotkey_.store(newHotkey, std::memory_order_relaxed);
        spdlog::info("Hotkey changed from {} to {}.", oldHotkey, newHotkey);
    }

    int HotkeyManager::getHotkey() const {
        spdlog::debug("getHotkey() called, returning {}.", hotkey_.load());
        return hotkey_.load();
    }
}
