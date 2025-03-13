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
    int getCount() const;

    /**
     * @brief Définit un message d'erreur.
     */
    void setError(const std::string &msg);

    /**
     * @brief Récupère le message d'erreur.
     * @return Le message d'erreur.
     */
    std::string getError() const;

private:
    std::atomic<int> count_;
    std::string errorMessage_;
};

} // namespace core

#endif // CHEATER_COUNT_MANAGER_H
