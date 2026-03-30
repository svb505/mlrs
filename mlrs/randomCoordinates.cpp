#include <vector>
#include "random"
#include "randomCoordinates.h"

std::vector<float> randomCoordinates() {
    std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distX(-20.0f, 20.0f);
    std::uniform_real_distribution<float> distZ(-80.0f, -150.0f);

    return { distX(gen),distZ(gen) };

}