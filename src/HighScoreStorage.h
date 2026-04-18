#pragma once

#include <string>

class HighScoreStorage {
public:
    explicit HighScoreStorage(std::string filename);
    int Load(int currentHighScore) const;
    int SaveIfHigher(int currentHighScore, int newScore) const;

private:
    std::string filename;
};
