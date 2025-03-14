#ifndef CHEATER_COUNT_MANAGER_H
#define CHEATER_COUNT_MANAGER_H

#include <atomic>
#include <string>

/**
 * @brief Classe qui gère le nombre de cheaters détectés.
 */
namespace core {

class CheaterCountManager {
public:
    /**
     * @brief Constructeur initialisant le compteur à -1 (aucune analyse effectuée).
     */
    CheaterCountManager();

    /**
     * @brief Met à jour le nombre de cheaters.
     * @param count Nouveau nombre de cheaters.
     */
    void setCount(int count);

    /**
     * @brief Récupère le nombre de cheaters.
     * @return Le nombre de cheaters.
     */
    [[nodiscard]] int getCount() const;

    /**
     * @brief Définit un message d'erreur.
     */
    void setError(const std::string &msg);

    /**
     * @brief Récupère le message d'erreur.
     * @return Le message d'erreur.
     */
    [[nodiscard]] std::string getError() const;

    /**
     * @brief Set the OCR result extracted from the backend.
     * @param ocr The OCR result (e.g. the server number as a string).
     */
    void setOCR(const std::string &ocr);

    /**
     * @brief Get the OCR result extracted from the backend.
     * @return The OCR result as a std::string (empty if not set).
     */
    [[nodiscard]] std::string getOCR() const;


private:
    std::atomic<int> count_;
    std::string errorMessage_;
    std::string ocrResult_;
};

} // namespace core

#endif // CHEATER_COUNT_MANAGER_H
