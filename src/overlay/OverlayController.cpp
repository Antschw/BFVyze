#include "overlay/OverlayController.h"
#include "imgui.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace overlay {

    OverlayController::OverlayController()
        : visible_(true), dragging_(false), windowStartX_(0), windowStartY_(0) {
        dragStartPos_.x = dragStartPos_.y = 0;
    }

    void OverlayController::run() {
        // Initialize the overlay window (GLFW, OpenGL, ImGui)
        if (!window_.initialize(OverlayWindow::MIN_WIDTH, OverlayWindow::MIN_HEIGHT, "BFVyze Overlay")) {
            spdlog::error("Failed to initialize OverlayWindow. Exiting.");
            return;
        }

        // Enter the main render loop
        while (!window_.shouldClose()) {
            window_.pollEvents();

            // Handle window dragging (click & drag on the title bar)
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                // Left mouse button is down
                if (!dragging_) {
                    // Not already dragging: check if we should start a drag
                    GetCursorPos(&dragStartPos_);  // capture initial cursor pos (screen coordinates)
                    // Capture window's position at drag start
                    glfwGetWindowPos(window_.getGLFWwindow(), &windowStartX_, &windowStartY_);
                    // Check if the initial click was in the title bar region (top 30px) and not on a UI widget
                    HWND hwnd = glfwGetWin32Window(window_.getGLFWwindow());
                    POINT clientPos = dragStartPos_;
                    ScreenToClient(hwnd, &clientPos);
                    if (clientPos.y < 30 && !ImGui::IsAnyItemHovered()) {
                        dragging_ = true;
                        spdlog::debug("Started dragging the overlay window.");
                    }
                } else {
                    // Already dragging: update window position as cursor moves
                    POINT currentPos;
                    GetCursorPos(&currentPos);
                    int deltaX = currentPos.x - dragStartPos_.x;
                    int deltaY = currentPos.y - dragStartPos_.y;
                    int newX = windowStartX_ + deltaX;
                    int newY = windowStartY_ + deltaY;
                    window_.setWindowPos(newX, newY);
                }
            } else {
                // Left button released or not pressed: end drag if it was in progress
                if (dragging_) {
                    spdlog::debug("Ended dragging the overlay window.");
                }
                dragging_ = false;
            }

            // Start a new ImGui frame and render the UI
            window_.beginFrame();
            renderUI();
            window_.endFrame();
        }

        // Loop has exited, meaning window is closing
        spdlog::info("Overlay window closed. Exiting render loop.");
    }

    void OverlayController::renderUI() {
        // Create a full-screen ImGui window to cover the entire overlay (no default title bar)
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)window_.getWidth(), (float)window_.getHeight()));
        ImGui::Begin("OverlayMainWindow", nullptr, windowFlags);
        {
            // Draw custom title bar as a child region at the top
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));  // transparent background
            ImGui::BeginChild("TitleBarRegion", ImVec2(0, 30), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            {
                ImGui::TextUnformatted("BFVyze Overlay");
                // Place control buttons on the same line, aligned to the right
                ImGui::SameLine();
                float buttonWidth = 60.0f;
                float spacing = ImGui::GetStyle().ItemSpacing.x;
                float totalButtonArea = 3 * buttonWidth + 3 * spacing;

                // Calculate remaining space to push buttons to the right
                float spacerWidth = ImGui::GetContentRegionAvail().x - totalButtonArea;
                if (spacerWidth < 0) spacerWidth = 0;
                ImGui::Dummy(ImVec2(spacerWidth, 0));  // invisible spacer
                ImGui::SameLine();

                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

                if (ImGui::Button("Minimize", ImVec2(buttonWidth, 0))) {
                    window_.hideWindow();
                    visible_ = false;
                }

                ImGui::SameLine();
                if (ImGui::Button("Config", ImVec2(buttonWidth, 0))) {
                    spdlog::info("Configuration button pressed (no action implemented yet).");
                    // Future: open configuration modal
                }
                ImGui::SameLine();
                if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
                    window_.closeWindow();
                }
                ImGui::PopStyleVar();

            }
            ImGui::EndChild();
            ImGui::PopStyleColor();

            // Draw main content area below the title bar
            ImGui::Separator();
            ImGui::Text("Ready to analyze server");
            ImGui::Text("Press + to start the scan");
        }
        ImGui::End();
    }

} // namespace overlay
