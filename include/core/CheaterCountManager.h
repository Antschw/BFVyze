#ifndef CHEATER_COUNT_MANAGER_H
#define CHEATER_COUNT_MANAGER_H

#include <atomic>
#include <memory>

/**
 * @brief Classe qui gère le nombre de cheaters détectés.
 */
namespace core {

    class CheaterCountManager {
    public:
        /**
         * @brief Constructeur initialisant le compteur à 0.
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

    private:
        std::atomic<int> count_;
    };

} // namespace core

#endif // CHEATER_COUNT_MANAGER_H
