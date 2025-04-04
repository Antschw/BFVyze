#pragma once
#include <atomic>
#include <string>

namespace GlobalState {

/**
 * @brief Indicates if the pipeline is currently running.
 *
 * This atomic boolean flag is used to determine if the pipeline process is active.
 * It is set to true when the pipeline starts and set to false when it stops.
 */
inline std::atomic pipelineActive{false};

/**
 * @brief Indicates if a scan has been initiated.
 *
 * This atomic boolean flag is set to true when the user initiates a scan by pressing the scan hotkey.
 * It is reset to false once the scan process begins.
 */
inline std::atomic scanInitiated{false};

/**
 * @brief Global error message.
 *
 * This string holds any error message that might occur during the execution of the pipeline or scan process.
 * It is empty when there are no errors.
 */
inline std::string errorMessage;

} // namespace GlobalState
