#include "overlay/OverlayController.h"

int main() {
    // Configure logger format (optional)
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("Starting BFVyze overlay...");

    overlay::OverlayController overlay;
    overlay.run();

    spdlog::info("Overlay application exited.");
    return 0;
}
