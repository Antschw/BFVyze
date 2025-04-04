// Preprocessor definitions must be set before including any Windows headers.
#define WIN32_LEAN_AND_MEAN
#ifndef WIN32_WINNT
#define WIN32_WINNT 0x0601
#endif

// Include Windows headers first.
#include <windows.h>
#include <shellapi.h>

#include "ipc/PythonBackendController.h"

#include <filesystem>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>
#include <zmq.hpp>

namespace ipc {

PythonBackendController::PythonBackendController(std::string pythonExecutable, std::string pythonScript) :
    m_pythonExecutable(std::move(pythonExecutable)), m_pythonScript(std::move(pythonScript)), m_shutdownContext(1) {}

PythonBackendController::~PythonBackendController() = default;

void PythonBackendController::launchProcess() const {
    // Check if the Python executable exists.
    if (!std::filesystem::exists(m_pythonExecutable)) {
        spdlog::error("Python executable not found at: {}", m_pythonExecutable);
        return;
    }
    // Check if the Python script exists.
    if (!std::filesystem::exists(m_pythonScript)) {
        spdlog::error("Python script not found at: {}", m_pythonScript);
        return;
    }

    spdlog::info("Launching Python backend: {} {}", m_pythonExecutable, m_pythonScript);
    HINSTANCE hRes =
            ShellExecuteA(nullptr, "open", m_pythonExecutable.c_str(), m_pythonScript.c_str(), nullptr, SW_HIDE);
    if (auto code = reinterpret_cast<intptr_t>(hRes); code <= 32) {
        spdlog::error("ShellExecute failed with code {}", code);
    } else {
        spdlog::info("ShellExecute success, Python backend should be starting.");
    }
}

void PythonBackendController::launchBackend() const { launchProcess(); }

void PythonBackendController::shutdownBackend() {
    try {
        zmq::socket_t requester(m_shutdownContext, zmq::socket_type::req);
        // Connect to the shutdown endpoint (must match the endpoint used by the Python backend)
        requester.connect("tcp://localhost:5557");
        const std::string shutdownMsg = "shutdown";
        spdlog::info("Sending shutdown command to Python backend...");
        requester.send(zmq::buffer(shutdownMsg), zmq::send_flags::none);

        zmq::message_t reply;
        (void) requester.recv(reply, zmq::recv_flags::none);
        spdlog::info("Received shutdown reply from Python backend: {}", reply.to_string());
    } catch (const std::exception &e) {
        spdlog::error("Error during shutdown: {}", e.what());
    }
}

} // namespace ipc
