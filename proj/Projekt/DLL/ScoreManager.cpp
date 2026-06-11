#include "ScoreManager.h"
#include <fstream>

void ScoreManager::saveHighScore(int score, const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        throw FileException("Nie udalo sie otworzyc pliku do zapisu binarnego: " + filename);
    }

    file.write(reinterpret_cast<const char*>(&score), sizeof(score));
    file.close();
}

int ScoreManager::loadHighScore(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw FileException("Brak pliku zapisu lub nie mozna go odczytac: " + filename);
    }

    int loadedScore = 0;
    file.read(reinterpret_cast<char*>(&loadedScore), sizeof(loadedScore));
    file.close();

    return loadedScore;
}