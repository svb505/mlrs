#pragma once
#include <vector>

class City;
class ExplosionEffect;
class SmokeEffect;
class Camera;

class MiniMap {
private:
    float height = 30.0f;
public:
    int step = 3;

    void setHeight(float h) { height = h; }
    float getHeight() { return height; }
    void draw(int ECRANW, int ECRANH, City& city, std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes, Camera& camera, float dt);
};



