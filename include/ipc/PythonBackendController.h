#pragma once

#include <string>
#include <zmq.hpp>

namespace ipc {

/**
 * @brief Controls the lifecycle of the Python backend process.
 *
 * This class is responsible for launching the Python backend process (via the Windows ShellExecute API)
 * and sending a shutdown command to the backend using ZeroMQ.
 *
 * The paths to the Python executable and the Python script are injected via the constructor,
 * making it easy to configure these values from constants or configuration files.
 */
class PythonBackendController {
public:
    /**
     * @brief Construct a new PythonBackendController object.
     *
     * @param pythonExecutable The full path to the Python executable (e.g., "python-3.13.2-embed-amd64\\pythonw.exe").
     * @param pythonScript The full path to the Python backend script (e.g.,
     * "python-3.13.2-embed-amd64\\Scripts\\main.py").
     */
    PythonBackendController(std::string pythonExecutable, std::string pythonScript);

    /**
     * @brief Destroy the PythonBackendController object.
     */
    ~PythonBackendController();

    /**
     * @brief Launch the Python backend process.
     *
     * This method verifies that the executable and script exist, then launches the backend
     * using ShellExecuteA.
     */
    void launchBackend() const;

    /**
     * @brief Sends a shutdown command to the Python backend via ZeroMQ.
     *
     * This method creates a ZeroMQ REQ socket that connects to the backend's shutdown endpoint,
     * sends the “shutdown” command, waits for an acknowledgment, and logs the result.
     */
    void shutdownBackend();

private:
    /**
     * @brief Checks for the existence of required files and launches the Python backend.
     */
    void launchProcess() const;

    std::string    m_pythonExecutable;
    std::string    m_pythonScript;
    zmq::context_t m_shutdownContext;
};

} // namespace ipc
