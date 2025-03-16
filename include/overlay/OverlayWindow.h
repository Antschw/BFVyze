#pragma once
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string>

namespace overlay {

/**
 * @brief Represents the overlay window and its underlying GLFW context.
 *
 * This class handles the creation of a borderless overlay window using GLFW and Win32.
 * It sets up an OpenGL context and integrates Dear ImGui for rendering the UI.
 * It also manages custom window styles (e.g., draggable title bar, resizable borderless window)
 * and provides functions to control the window (closing, hiding, showing, repositioning).
 */
class OverlayWindow {
public:
    /**
     * @brief Constructs an OverlayWindow (does not open a window yet).
     *
     * Use initialize() to create the actual window and set up the rendering context.
     */
    OverlayWindow();

    /**
     * @brief Destructs the OverlayWindow, cleaning up GLFW/ImGui resources.
     */
    ~OverlayWindow();

    /**
     * @brief Initializes the overlay window with OpenGL and ImGui.
     *
     * Creates a GLFW window without a native title bar (we will draw a custom one),
     * enables resizing (with a custom borderless style via Win32),
     * and sets up the OpenGL context and ImGui environment.
     *
     * @param width  Initial width of the window (enforced minimum is 400).
     * @param height Initial height of the window (enforced minimum is 400).
     * @param title  Title text for the window (used for display and logging).
     * @return true on successful initialization, false on failure (error will be logged).
     */
    bool initialize(int width = 400, int height = 400, const std::string &title = "BFVyze Overlay");

    /**
     * @brief Checks if the window is signaled to close.
     * @return true if the window should close (for example, the user clicked the close button), false otherwise.
     */
    [[nodiscard]] bool shouldClose() const;

    /**
     * @brief Polls for window events (input, window movement, etc.).
     *
     * This should be called once per frame in the main loop to process events.
     */
    static void pollEvents();

    /**
     * @brief Begins a new ImGui frame for this window.
     *
     * This updates the current window size (in case of resizing) and sets up ImGui for rendering new UI elements.
     */
    void beginFrame();

    /**
     * @brief Ends the ImGui frame and renders the UI to the window.
     *
     * This function finalizes the ImGui frame, clears the window with a transparent background,
     * renders ImGui draw data via OpenGL, and swaps the GLFW window buffers.
     */
    void endFrame() const;

    /**
     * @brief Signals the window to close.
     *
     * This sets the window's should-close flag, which will break out of the render loop and trigger shutdown.
     */
    void closeWindow() const;

    /**
     * @brief Hide (minimize) the overlay window.
     *
     * Minimizes the window (using Win32 `ShowWindow`) without destroying it, so it can be shown again later.
     */
    void hideWindow() const;

    /**
     * @brief Show the overlay window if it was hidden or minimized.
     */
    void showWindow() const;

    /**
     * @brief Moves the window to a new screen position.
     * @param x New X coordinate (screen coordinate of the window's top-left corner).
     * @param y New Y coordinate (screen coordinate of the window's top-left corner).
     */
    void setWindowPos(int x, int y) const;

    /**
     * @brief Returns the current GLFW window pointer (native window handle access if needed).
     */
    [[nodiscard]] GLFWwindow *getGLFWwindow() const { return window_; }

    /** @brief Returns the current width of the window. */
    [[nodiscard]] int getWidth() const { return width_; }

    /** @brief Returns the current height of the window. */
    [[nodiscard]] int getHeight() const { return height_; }

    /// Minimum allowed window dimensions.
    static constexpr int MIN_WIDTH  = 400;
    static constexpr int MIN_HEIGHT = 400;
    /// Maximum allowed window dimensions.
    static constexpr int MAX_WIDTH  = 1080;
    static constexpr int MAX_HEIGHT = 1080;

    // Disable copy construction and assignment (the window resource is non-copyable).
    OverlayWindow(const OverlayWindow &)            = delete;
    OverlayWindow &operator=(const OverlayWindow &) = delete;

private:
    GLFWwindow *window_; ///< Pointer to the underlying GLFW window.
    int         width_; ///< Current window width (updated on resize).
    int         height_; ///< Current window height (updated on resize).
    std::string title_; ///< Window title (for logging and identification).
};

} // namespace overlay
