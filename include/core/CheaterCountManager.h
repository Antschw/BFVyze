#ifndef CHEATER_COUNT_MANAGER_H
#define CHEATER_COUNT_MANAGER_H

#include <atomic>
#include <string>

namespace core {

class CheaterCountManager {
public:
    /**
     * @brief Constructor that initializes the counter to -1 (no analysis performed).
     */
    CheaterCountManager();

    /**
     * @brief Updates the number of detected cheaters.
     * @param count The new cheater count.
     */
    void setCount(int count);

    /**
     * @brief Retrieves the number of detected cheaters.
     * @return The current cheater count.
     */
    [[nodiscard]] int getCount() const;

    /**
     * @brief Sets an error message.
     * @param msg The error message.
     */
    void setError(const std::string &msg);

    /**
     * @brief Retrieves the error message.
     * @return The current error message.
     */
    [[nodiscard]] std::string getError() const;

    /**
     * @brief Sets the OCR result extracted from the backend.
     * @param ocr The OCR result (e.g. the server number as a string).
     */
    void setOCR(const std::string &ocr);

    /**
     * @brief Retrieves the OCR result extracted from the backend.
     * @return The OCR result as a string (empty if not set).
     */
    [[nodiscard]] std::string getOCR() const;

private:
    std::atomic<int> count_;
    std::string      errorMessage_;
    std::string      ocrResult_;
};

} // namespace core

#endif // CHEATER_COUNT_MANAGER_H
