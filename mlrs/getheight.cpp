#include "getheight.h"
#include <cmath>

float getHeight(float x, float z) {
    return sin(x * 0.05f) * 2.0f + cos(z * 0.05f) * 2.0f;
}