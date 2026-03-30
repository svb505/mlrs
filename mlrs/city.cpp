#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "city.h"
#include "vec3.h"
#include <random>
#include "effects.h"
#include "sounds.h"
std::vector<float> randomColor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    return {dis(gen),dis(gen),dis(gen)};
}
Building::Building(BuildingType t, const Vec3& pos, float w, float h, float d, std::vector<float> clr)
    : type(t), position(pos), width(w), height(h), depth(d), color(clr), destructionLevel(0.0f)
{
    generateMesh();
}
void Building::destroy(float amount) {
    destructionLevel += amount;
    if (destructionLevel > 1.0f) destructionLevel = 1.0f;

    applyDestruction();
}
void Building::draw() const {
    glBegin(GL_TRIANGLES);

    for (size_t i = 0; i < vertices.size(); i += 3) {
        const Vec3& a = vertices[i].position;
        const Vec3& b = vertices[i + 1].position;
        const Vec3& c = vertices[i + 2].position;

        // нормаль
        Vec3 ab = { b.x - a.x, b.y - a.y, b.z - a.z };
        Vec3 ac = { c.x - a.x, c.y - a.y, c.z - a.z };
        Vec3 normal = {
            ab.y * ac.z - ab.z * ac.y,
            ab.z * ac.x - ab.x * ac.z,
            ab.x * ac.y - ab.y * ac.x
        };
        float len = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (len != 0) { normal.x /= len; normal.y /= len; normal.z /= len; }

        glNormal3f(normal.x, normal.y, normal.z);
        
        if (type == BuildingType::Residential) glColor3f(color[0],color[1],color[2]);
        else if (destructionLevel > 0.5f) glColor3f(0.5f, 0.3f, 0.3f);
        else if (type == BuildingType::Storage) glColor3f(255.0f / 255.0f, 128.0f / 255.0f, 0.0f);
        else glColor3f(0.7f, 0.7f, 0.7f);

        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }

    glEnd();
}
void Building::generateMesh() {
    vertices.clear();

    float x = position.x, y = position.y, z = position.z;
    float w = width / 2.0f, h = height, d = depth / 2.0f;

    
    if (type == BuildingType::Residential) {
        addCube(x, y, z, w, h, d);
        addRoof(x, y + h, z, w, d);
    }
    else if (type == BuildingType::Factory) {
        addCube(x, y, z, w, h, d);
        addRoof(x, y + h, z, w, d);
        addChimney(x + w * 0.5f, y + h, z, w * 0.2f, h * 0.5f);
    }
    else if (type == BuildingType::Storage) addCube(x, y, z, w, h, d);
        
}

void Building::applyDestruction() {
    generateMesh(); // пересоздаем базовую форму

    for (auto& v : vertices) {
        float heightFactor = (v.position.y - position.y) / height;
        float destr = destructionLevel * heightFactor;

        v.position.y -= destr * height;
        float noise = (rand() % 1000 / 1000.0f - 0.5f);
        v.position.x += noise * destr * 5.0f;
        v.position.z += noise * destr * 5.0f;

        if (v.position.y > position.y + height * (1.0f - destructionLevel))
            v.position.y = position.y + height * (1.0f - destructionLevel);
    }
}
void Building::addCube(float x, float y, float z, float w, float h, float d) {
    Vec3 p0 = { x - w, y, z - d }, p1 = { x + w, y, z - d }, p2 = { x + w, y + h, z - d }, p3 = { x - w, y + h, z - d };
    Vec3 p4 = { x - w, y, z + d }, p5 = { x + w, y, z + d }, p6 = { x + w, y + h, z + d }, p7 = { x - w, y + h, z + d };

    addQuad(p0, p1, p2, p3);
    addQuad(p5, p4, p7, p6);
    addQuad(p4, p0, p3, p7);
    addQuad(p1, p5, p6, p2);
    addQuad(p3, p2, p6, p7);
    addQuad(p4, p5, p1, p0);
}
void Building::addRoof(float x, float y, float z, float w, float d) {
    Vec3 top = { x, y + (w + d) * 0.5f, z };
    Vec3 p0 = { x - w, y, z - d }, p1 = { x + w, y, z - d }, p2 = { x + w, y, z + d }, p3 = { x - w, y, z + d };
    addTriangle(p0, p1, top);
    addTriangle(p1, p2, top);
    addTriangle(p2, p3, top);
    addTriangle(p3, p0, top);
}
void Building::addChimney(float x, float y, float z, float w, float h) {
    addCube(x, y, z, w, h, w);
}
void Building::addQuad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) {
    addTriangle(a, b, c);
    addTriangle(a, c, d);
}
void Building::addTriangle(const Vec3& a, const Vec3& b, const Vec3& c) {
    vertices.push_back({ a });
    vertices.push_back({ b });
    vertices.push_back({ c });
}
bool Building::hitTest(const Vec3& point) const {
    Vec3 half = { width / 2.0f, height, depth / 2.0f };
    Vec3 min = { position.x - half.x, position.y, position.z - half.z };
    Vec3 max = { position.x + half.x, position.y + half.y, position.z + half.z };

    return (point.x >= min.x && point.x <= max.x &&
        point.y >= min.y && point.y <= max.y &&
        point.z >= min.z && point.z <= max.z);
}

void Building::applyHit(const Vec3& hitPoint, float damageAmount) {
    destroy(damageAmount);

    for (auto& v : vertices) {
        float dx = v.position.x - hitPoint.x;
        float dy = v.position.y - hitPoint.y;
        float dz = v.position.z - hitPoint.z;

        float dist = sqrt(dx * dx + dy * dy + dz * dz);

        if (dist < 10.0f) {
            float factor = (10.0f - dist) / 10.0f;
            v.position.y -= factor * damageAmount * height;
            float noise = (rand() % 1000 / 1000.0f - 0.5f) * damageAmount * 5.0f;
            v.position.x += noise;
            v.position.z += noise;
        }
    }
}

// === City ===
City::City(const Vec3& c, float r, int totB) : center(c), radius(r), totalBuildings(totB){}

void City::generate(int count) {
    buildings.clear();

    for (int i = 0; i < count; ++i) {
        float angle = randomFloat(0.0f, 2.0f * 3.1415926f);
        float dist = randomFloat(0.0f, radius);

        float x = center.x + cos(angle) * dist;
        float z = center.z + sin(angle) * dist;
        Vec3 pos = { x, center.y, z };

        BuildingType type;

        int idx = i % 20; 

        if (idx == 0) type = BuildingType::Storage;        
        else if (idx < 18) type = BuildingType::Residential; 
        else type = BuildingType::Factory;              

        float w = (type == BuildingType::Storage) ? randomFloat(3.0f, 6.0f) : randomFloat(2.0f, 4.0f);
        float h = (type == BuildingType::Factory) ? randomFloat(3.0f, 6.0f) : randomFloat(1.0f, 3.0f);
        float d = (type == BuildingType::Storage) ? randomFloat(4.0f, 6.0f) : randomFloat(2.0f, 3.0f);

        auto color = randomColor();

        buildings.emplace_back(type, pos, w, h, d,color);
    }
}

void City::draw() const {
    for (const auto& b : buildings) b.draw();
}

void City::applyShot(const Vec3& shotPos, std::vector<ExplosionEffect*>& expl,
    std::vector<SmokeEffect*>& smokes,Sound& sound, int& score,float damage) {
    for (auto& b : buildings) {
        if (b.hitTest(shotPos)) {
            if (b.type == BuildingType::Storage && !b.explosed) {
                expl.push_back(new ExplosionEffect(b.position.x, b.position.y, b.position.z, 
                    600, 5.0f, 0.9f, 1.5f));
                smokes.push_back(new SmokeEffect(b.position.x, b.position.y, b.position.z,300));
                
                sound.setSourcePosition(sound.storageExplosionSource, b.position.x, b.position.y, b.position.z);
                alSourcePlay(sound.storageExplosionSource);

                b.explosed = true;

                score += 3;
            }
            else {
                expl.push_back(new ExplosionEffect(b.position.x, b.position.y, b.position.z, 200, 5.0f, 0.5f, 1.0f));
                smokes.push_back(new SmokeEffect(b.position.x, b.position.y, b.position.z, 100, 2.0f,
                    { 0.5f,0.5f,0.5f }));
                score++;           
            }

            if (!b.hitFlag) { totalBuildings--; b.hitFlag = true; }
            b.applyHit(shotPos, damage);
            break;
        }
    }
}

void City::config(const Vec3& newCenter,int totalB,int rCount) {
    center = newCenter;
    buildings.clear();
    totalBuildings = totalB;
    rCount = 0;
    generate(totalB); 
}
float City::randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}