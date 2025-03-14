#pragma once
#include <atomic>
#include <string>

namespace GlobalState {
    // Indicates if the pipeline is currently running.
    inline std::atomic<bool> pipelineActive{false};

    // Indicates if a scan has been initiated (i.e. the user pressed the scan hotkey).
    inline std::atomic<bool> scanInitiated{false};

    // Global error message, if any.
    inline std::string errorMessage;
}