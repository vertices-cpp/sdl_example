
#include "base/ogRandom.h"

std::mt19937 &OG::RandomHelper::getEngine() {
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());
    return engine;
}
