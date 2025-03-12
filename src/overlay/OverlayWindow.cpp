#include <glad/glad.h>           // Load OpenGL functions first
#include <GLFW/glfw3.h>          // Then GLFW
#define GLFW_EXPOSE_NATIVE_WIN32 // Expose glfwGetWin32Window
#include <GLFW/glfw3native.h>    // For native access (Win32)
#include <windows.h>
#include <windowsx.h>           // For GET_X_LPARAM and GET_Y_LPARAM
#include "overlay/OverlayWindow.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace overlay {

    // Static variables and callback for custom window procedure (Win32)
    static WNDPROC s_originalWndProc = nullptr;

    /**
     * @brief Custom window procedure to enable resizing via edges on a borderless window and enforce size limits.
     *
     * This intercepts specific window messages:
     * - WM_NCHITTEST: Identifies if the mouse is on the edges or corners to allow resizing (returns appropriate HT code).
     * - WM_GETMINMAXINFO: Enforces the minimum and maximum tracking sizes of the window (to restrict resize within [400,1080] range).
     * Other messages are passed to the original window procedure.
     */
    static LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_NCHITTEST: {
            // Determine if the cursor is over a border/corner (for resizing)
            RECT rect;
            GetClientRect(hWnd, &rect);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hWnd, &pt);
            const int RESIZE_BORDER = 8;  // Resize handle width (pixels)
            bool onLeft   = pt.x <= RESIZE_BORDER;
            bool onRight  = pt.x >= rect.right - RESIZE_BORDER;
            bool onTop    = pt.y <= RESIZE_BORDER;
            bool onBottom = pt.y >= rect.bottom - RESIZE_BORDER;
            if (onTop && onLeft)    return HTTOPLEFT;
            if (onTop && onRight)   return HTTOPRIGHT;
            if (onBottom && onLeft) return HTBOTTOMLEFT;
            if (onBottom && onRight)return HTBOTTOMRIGHT;
            if (onTop)    return HTTOP;
            if (onBottom) return HTBOTTOM;
            if (onLeft)   return HTLEFT;
            if (onRight)  return HTRIGHT;
            return HTCLIENT;
        }
        case WM_GETMINMAXINFO: {
            // Enforce minimum and maximum window size
            LPMINMAXINFO pmmi = reinterpret_cast<LPMINMAXINFO>(lParam);
            pmmi->ptMinTrackSize.x = OverlayWindow::MIN_WIDTH;
            pmmi->ptMinTrackSize.y = OverlayWindow::MIN_HEIGHT;
            pmmi->ptMaxTrackSize.x = OverlayWindow::MAX_WIDTH;
            pmmi->ptMaxTrackSize.y = OverlayWindow::MAX_HEIGHT;
            return 0;  // We've set the limits
        }
        }
        // For all other messages, call the original window procedure
        return CallWindowProc(s_originalWndProc, hWnd, uMsg, wParam, lParam);
    }

    OverlayWindow::OverlayWindow()
        : window_(nullptr), width_(MIN_WIDTH), height_(MIN_HEIGHT) {
        // Nothing else to initialize here
    }

    OverlayWindow::~OverlayWindow() {
        // Cleanup ImGui and GLFW resources
        if (window_) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwDestroyWindow(window_);
        }
        glfwTerminate();
        spdlog::info("OverlayWindow destroyed and resources cleaned up.");
    }

    bool OverlayWindow::initialize(int width, int height, const std::string& title) {
        // Enforce minimum size on initialization parameters
        if (width < MIN_WIDTH)  width = MIN_WIDTH;
        if (height < MIN_HEIGHT) height = MIN_HEIGHT;
        width_ = width;
        height_ = height;
        title_ = title;

        // Initialize GLFW library
        if (!glfwInit()) {
            spdlog::error("Failed to initialize GLFW");
            return false;
        }
        // Configure GLFW window hints for a borderless, resizable, transparent window
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);    // No OS title bar/decorations
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);     // Allow resizing by the user
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);      // Keep window always on top (floating above others)
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);  // Transparent background
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create the GLFW window
        window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
        if (!window_) {
            spdlog::error("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }
        spdlog::info("GLFW window created ({}x{})", width_, height_);

        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1);  // Enable VSync (synchronize buffer swap with monitor refresh)

        // Load OpenGL function pointers via GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            spdlog::error("Failed to initialize GLAD (OpenGL loader)");
            glfwDestroyWindow(window_);
            glfwTerminate();
            return false;
        }
        spdlog::info("OpenGL context initialized successfully");

        // Obtain the native Win32 handle for the GLFW window and adjust window style for resizing
        HWND hWnd = glfwGetWin32Window(window_);
        if (hWnd) {
            // Add a thick frame style to allow resizing via borders (even though window has no standard frame)
            LONG style = GetWindowLong(hWnd, GWL_STYLE);
            style |= WS_THICKFRAME;
            SetWindowLong(hWnd, GWL_STYLE, style);

            // Notify Windows of the style change
            SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

            // Subclass the window procedure with our custom one for hit-testing and size limits
            s_originalWndProc = reinterpret_cast<WNDPROC>(
                SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CustomWndProc))
            );
        } else {
            spdlog::warn("Could not retrieve Win32 window handle for custom styling.");
        }

        // Position the window at the bottom-right of the primary monitor by default
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor) {
            const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
            if (videoMode) {
                int screenW = videoMode->width;
                int screenH = videoMode->height;
                glfwSetWindowPos(window_, screenW - width_, screenH - height_);
            }
        }

        // Initialize ImGui context and style for this window
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();  // start with the dark theme
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;  // round corners on ImGui windows
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.3f, 0.8f);  // semi-opaque navy blue background
        style.Colors[ImGuiCol_Text]     = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // white text for visibility

        // Load font(s) for ImGui (if the font file is available in working directory)
        io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 10.0f);
        io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 15.0f);

        // Initialize ImGui GLFW and OpenGL backends
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        spdlog::info("ImGui context created and configured for the overlay window.");

        return true;
    }

    bool OverlayWindow::shouldClose() const {
        return glfwWindowShouldClose(window_);
    }

    void OverlayWindow::pollEvents() {
        glfwPollEvents();
    }

    void OverlayWindow::beginFrame() {
        // Update stored width and height in case the window was resized
        int newW, newH;
        glfwGetWindowSize(window_, &newW, &newH);
        width_ = newW;
        height_ = newH;
        // Begin a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void OverlayWindow::endFrame() {
        // Finalize and render the ImGui frame
        ImGui::Render();
        // Set viewport to the entire window framebuffer
        int fbW, fbH;
        glfwGetFramebufferSize(window_, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);
        // Clear the screen (transparent background)
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Render ImGui draw data into the OpenGL context
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
    }

    void OverlayWindow::closeWindow() {
        if (window_) {
            spdlog::info("Close button pressed: closing overlay window.");
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
        }
    }

    void OverlayWindow::hideWindow() {
#ifdef _WIN32
        if (window_) {
            HWND hWnd = glfwGetWin32Window(window_);
            if (hWnd) {
                spdlog::info("Minimize/Hide action: minimizing overlay window.");
                ShowWindow(hWnd, SW_MINIMIZE);
            }
        }
#endif
    }

    void OverlayWindow::showWindow() {
#ifdef _WIN32
        if (window_) {
            HWND hWnd = glfwGetWin32Window(window_);
            if (hWnd) {
                spdlog::info("Restoring overlay window from hidden state.");
                ShowWindow(hWnd, SW_RESTORE);
            }
        }
#endif
    }

    void OverlayWindow::setWindowPos(int x, int y) {
        if (window_) {
            glfwSetWindowPos(window_, x, y);
        }
    }

} // namespace overlay
