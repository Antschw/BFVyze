#pragma once
#include "overlay/OverlayWindow.h"
#include "core/CheaterCountManager.h"
#ifdef _WIN32
#include <windows.h>   // For POINT, GetAsyncKeyState, etc.
#endif

namespace overlay {

    /**
     * @brief Controls the overlay UI and user interactions.
     *
     * The OverlayController is responsible for rendering the ImGui-based UI (including the custom title bar and content)
     * and handling user interactions such as dragging the window or clicking title bar buttons.
     * It uses an underlying OverlayWindow for window creation, context management, and low-level events.
     */
    class OverlayController {
    public:
        /**
         * @brief Constructs an OverlayController.
         * @param cheaterManager Shared pointer to the cheater count manager.
         */
        explicit OverlayController(std::shared_ptr<core::CheaterCountManager> cheaterManager);

        /**
         * @brief Runs the overlay's main loop.
         *
         * This will create and display the overlay window (via OverlayWindow) and then enter the render loop
         * to draw the UI each frame until the window is closed by the user.
         */
        void run();

        /**
         * @brief Checks if the overlay window is currently visible (not minimized/hidden).
         * @return true if the overlay is visible on screen, false if it is hidden/minimized.
         */
        bool isVisible() const { return visible_; }

    private:
        OverlayWindow window_;   ///< The underlying window that provides the GLFW context and event handling.
        bool visible_;           ///< Visibility state of the overlay (true if shown, false if hidden/minimized).
        bool dragging_;          ///< True if the user is currently dragging the window via the title bar.
        POINT dragStartPos_{};     ///< Cursor position (screen coords) at the moment a drag started.
        int windowStartX_, windowStartY_;  ///< Window position at the start of a drag operation.
        std::shared_ptr<core::CheaterCountManager> cheaterManager_; ///< Managing cheaters number.
        /**
         * @brief Renders the ImGui interface for one frame.
         *
         * This creates the overlay's ImGui window, including a custom title bar (with title text and control buttons)
         * and the main content area. It also handles input within the UI (button clicks) to trigger window actions.
         */
        void renderUI();
    };

} // namespace overlay
