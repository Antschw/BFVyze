//
// Created by Antoine on 09/03/2025.
//

#ifndef HOTKEY_MANAGER_H
#define HOTKEY_MANAGER_H

#include <functional>
#include <atomic>

/**
 * @class HotkeyManager
 * @brief Handles global hotkey detection using GetAsyncKeyState.
 */
namespace input {
    class HotkeyManager {
    public:
        using Callback = std::function<void()>;

        /**
         * @brief Constructs a HotkeyManager with the specified hotkey.
         * @param hotkey The virtual key code of the hotkey (e.g., VK_F9).
         */
        explicit HotkeyManager(int hotkey);

        /// @brief Destructor to ensure proper cleanup.
        ~HotkeyManager();

        /**
         * @brief Starts monitoring for the hotkey press.
         * @param callback Function to execute when the hotkey is pressed.
         */
        void start(const Callback& callback);

        /// @brief Stops monitoring the hotkey.
        void stop();

        /**
         * @brief Updates the hotkey dynamically.
         * @param newHotkey New virtual key code.
         */
        void setHotkey(int newHotkey);

        /// @return The current hotkey.
        [[nodiscard]] int getHotkey() const;

    private:
        std::atomic<bool> running_; ///< Atomic flag to control the loop execution
        std::atomic<int> hotkey_; ///< Current hotkey virtual key code
    };
}

#endif // HOTKEY_MANAGER_H
