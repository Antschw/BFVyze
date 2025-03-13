#include "core/CheaterCountManager.h"

namespace core {

    CheaterCountManager::CheaterCountManager() : count_(0) {}

    void CheaterCountManager::setCount(int count) {
        count_.store(count, std::memory_order_relaxed);
    }

    int CheaterCountManager::getCount() const {
        return count_.load(std::memory_order_relaxed);
    }

} // namespace core
