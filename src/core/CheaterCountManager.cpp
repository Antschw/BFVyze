#include "core/CheaterCountManager.h"

namespace core {

CheaterCountManager::CheaterCountManager() : count_(-1) {}

void CheaterCountManager::setCount(int count) {
    count_.store(count, std::memory_order_relaxed);
}

int CheaterCountManager::getCount() const {
    return count_.load(std::memory_order_relaxed);
}

void CheaterCountManager::setError(const std::string &msg) {
    errorMessage_ = msg;
}

std::string CheaterCountManager::getError() const {
    return errorMessage_;
}

void CheaterCountManager::setOCR(const std::string &ocr) {
    ocrResult_ = ocr;
}

std::string CheaterCountManager::getOCR() const {
    return ocrResult_;
}

} // namespace core
