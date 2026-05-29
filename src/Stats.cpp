#include "Stats.h"
#include <fstream>

void Stats::save(const std::string& path) const {
    std::ofstream f(path);
    if (!f) return;
    f << "wins="          << wins          << "\n"
      << "losses="        << losses        << "\n"
      << "enemiesKilled=" << enemiesKilled << "\n"
      << "oasesRestored=" << oasesRestored << "\n"
      << "highestWave="   << highestWave   << "\n";
}

bool Stats::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        try {
            int val = std::stoi(line.substr(eq + 1));
            if      (key == "wins")          wins          = val;
            else if (key == "losses")        losses        = val;
            else if (key == "enemiesKilled") enemiesKilled = val;
            else if (key == "oasesRestored") oasesRestored = val;
            else if (key == "highestWave")   highestWave   = val;
        } catch (...) {}
    }
    return true;
}
