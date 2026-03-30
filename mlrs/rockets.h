#pragma once
#include "vec3.h"
#include <vector>
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

extern float speed;

struct rocketPos {
	Vec3 pos;
	Vec3 vel;
	Vec3 nozzlePos;
	float spawnDelay = 0.0f; 
	float spawnTimer = 0.0f;
	bool active = false; 
};
float RandomSpread(float range);
Vec3 computeImpact(const Vec3& launcherPos, float speed, float pitch, float yaw);

class Rocket {
public:
	std::vector<rocketPos> rockets;
    void DrawBox(float w, float h, float d);
	void DrawRocket(const Vec3& position, const Vec3& direction);
};
