#pragma once
#include <string>

struct Stats {
    int wins          = 0;
    int losses        = 0;
    int enemiesKilled = 0;
    int oasesRestored = 0;
    int highestWave   = 0;

    void save(const std::string& path) const;
    bool load(const std::string& path);
};
