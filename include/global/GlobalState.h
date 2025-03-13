#pragma once
#include <atomic>
#include <string>

namespace GlobalState {
    // Indique si le pipeline est en cours d'exécution.
    inline std::atomic<bool> pipelineActive{false};

    // Autres états globaux éventuels, par exemple un message d'erreur.
    inline std::string errorMessage = "";
}
