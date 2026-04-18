#include "HighScoreStorage.h"

#include <fstream>
#include <utility>

HighScoreStorage::HighScoreStorage(std::string filename)
    : filename(std::move(filename)) {}

int HighScoreStorage::Load(int currentHighScore) const {
    int loadedHighScore = currentHighScore;
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> loadedHighScore;
        file.close();
    }
    return loadedHighScore;
}

int HighScoreStorage::SaveIfHigher(int currentHighScore, int newScore) const {
    if (newScore > currentHighScore) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << newScore;
            file.close();
        }
        return newScore;
    }

    return currentHighScore;
}
