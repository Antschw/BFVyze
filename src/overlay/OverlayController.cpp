#include "overlay/OverlayController.h"
#include "imgui.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "global/GlobalState.h"

namespace overlay {

OverlayController::OverlayController(std::shared_ptr<core::CheaterCountManager> cheaterManager) :
    visible_(true), dragging_(false), windowStartX_(0), windowStartY_(0), cheaterManager_(std::move(cheaterManager)) {
    dragStartPos_.x = dragStartPos_.y = 0;
}

void OverlayController::run() {
    // Initialize the overlay window (GLFW, OpenGL, ImGui)
    if (!window_.initialize(OverlayWindow::MIN_WIDTH, OverlayWindow::MIN_HEIGHT, "BFVyze")) {
        spdlog::error("Failed to initialize OverlayWindow. Exiting.");
        return;
    }

    // Enter the main render loop
    while (!window_.shouldClose()) {
        OverlayWindow::pollEvents();

        // Handle window dragging (click and drag on the title bar)
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            if (!dragging_) {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                int wx, wy;
                glfwGetWindowPos(window_.getGLFWwindow(), &wx, &wy);
                const int ww = window_.getWidth();

                // Check if the click is inside the window
                if (const int wh = window_.getHeight();
                    cursorPos.x >= wx && cursorPos.x < wx + ww && cursorPos.y >= wy && cursorPos.y < wy + wh) {
                    // Convert to client coordinates to see if it's near the top bar
                    POINT clientPos = cursorPos;
                    // ReSharper disable once CppLocalVariableMayBeConst
                    HWND hwnd      = glfwGetWin32Window(window_.getGLFWwindow());
                    ScreenToClient(hwnd, &clientPos);
                    if (clientPos.y < 30 && !ImGui::IsAnyItemHovered()) {
                        dragging_     = true;
                        dragStartPos_ = cursorPos;
                        windowStartX_ = wx;
                        windowStartY_ = wy;
                        spdlog::debug("Started dragging the overlay window.");
                    }
                }
            } else {
                // Already dragging: update window position as cursor moves
                POINT currentPos;
                GetCursorPos(&currentPos);
                const int deltaX = currentPos.x - dragStartPos_.x;
                const int deltaY = currentPos.y - dragStartPos_.y;
                const int newX   = windowStartX_ + deltaX;
                const int newY   = windowStartY_ + deltaY;
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

    // The loop has exited, meaning the window is closing
    spdlog::info("Overlay window closed. Exiting render loop.");
}

void OverlayController::renderUI() const {
    // We no longer update the window title with OCR.
    // Instead, we will show it inside the overlay if available.

    // Prepare a full-screen ImGui window
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window_.getWidth()), static_cast<float>(window_.getHeight())));
    ImGui::Begin("OverlayMainWindow", nullptr, windowFlags);
    {
        // Draw custom title bar as a child region at the top
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0)); // transparent background
        ImGui::BeginChild("TitleBarRegion", ImVec2(0, 30), false,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::TextUnformatted("BFVyze");
            if (GlobalState::pipelineActive.load()) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Pipeline running");
            }
            // Place control buttons aligned to the right
            ImGui::SameLine();
            constexpr float buttonWidth     = 60.0f;
            const float     spacing         = ImGui::GetStyle().ItemSpacing.x;
            const float     totalButtonArea = 3 * buttonWidth + 3 * spacing;
            float           spacerWidth     = ImGui::GetContentRegionAvail().x - totalButtonArea;
            if (spacerWidth < 0)
                spacerWidth = 0;
            ImGui::Dummy(ImVec2(spacerWidth, 0));
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            if (ImGui::Button("Minimize", ImVec2(buttonWidth, 0))) {
                window_.hideWindow();
            }
            ImGui::SameLine();
            if (ImGui::Button("Config", ImVec2(buttonWidth, 0))) {
                spdlog::info("Configuration button pressed (not implemented yet).");
            }
            ImGui::SameLine();
            if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
                window_.closeWindow();
            }
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Separator();

        // If a scan is not started yet, show instructions
        if (!GlobalState::scanInitiated.load()) {
            ImGui::Text("Ready to analyze server");
            ImGui::Text("Press + to start the scan");
        } else {
            // Show OCR result if non-empty
            if (cheaterManager_) {
                const std::string ocrResult = cheaterManager_->getOCR();
                if (!ocrResult.empty()) {
                    ImGui::TextColored(ImVec4(0, 1, 1, 1), "Scan for server #%s", ocrResult.c_str());
                }
            }

            // Show the error if present
            const std::string errorMsg = cheaterManager_ ? cheaterManager_->getError() : "";
            if (!errorMsg.empty()) {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Error: %s", errorMsg.c_str());
            }

            // Show cheater count if > 0
            if (cheaterManager_ && cheaterManager_->getCount() > 0) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Detected cheaters: %d", cheaterManager_->getCount());
            }
        }
    }
    ImGui::End();
}

} // namespace overlay
