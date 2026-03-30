#pragma once
#include <vector>
#include "vec3.h"

enum class BuildingType {
    Residential,
    Factory,
    Storage
};
struct Vertex {
    Vec3 position;
};

class ExplosionEffect;
class SmokeEffect;
class Sound;
class Building {
public:
    BuildingType type;
    Vec3 position;
    float width, height, depth;
    std::vector<float> color;

    float destructionLevel; // 0..1
    bool explosed = false;
    bool hitFlag = false;

    std::vector<Vertex> vertices;

    Building(BuildingType t, const Vec3& pos, float w, float h, float d, std::vector<float> clr);

    void destroy(float amount);
    void draw() const;
    bool hitTest(const Vec3& point) const;
    void applyHit(const Vec3& hitPoint, float damageAmount);
    void generateMesh();
    void applyDestruction();
private:
    void generateRuins();
    void addCube(float x, float y, float z, float w, float h, float d);
    void addQuad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d);
    
    void addRoof(float x, float y, float z, float w, float d);
    void addChimney(float x, float y, float z, float w, float h);
    void addTriangle(const Vec3& a, const Vec3& b, const Vec3& c);
};
class City {
public:
    std::vector<Building> buildings;

    Vec3 center;
    float radius;
    int totalBuildings;

    City(const Vec3& c, float r,int totB);

    void config(const Vec3& newCenter,int totalB,int rCount);
    void generate(int count);
    void draw() const;
    void applyShot(const Vec3& shotPos,std::vector<ExplosionEffect*>& expl,
        std::vector<SmokeEffect*>& smokes, Sound& sound,int& score,float damage = 0.1f);
private:
    float randomFloat(float min, float max);
};
