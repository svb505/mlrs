#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "rockets.h"
#include "vec3.h"
#include "getheight.h"

enum class Type {
	smoke,
	fire
};
struct GradLauncher {
	Vec3 position;     
	float yaw;         
	float pitch;       

	std::vector<Vec3> localGuides;

	std::vector<Vec3> worldGuides;
};
struct Particle {
	Type type;
	Vec3 pos;
	Vec3 vel;
	float life; 
};

Vec3 RotateYawPitch(const Vec3& v, float yaw, float pitch) {
	// YAW
	float cy = cosf(yaw);
	float sy = sinf(yaw);

	Vec3 r;
	r.x = v.x * cy - v.z * sy;
	r.z = v.x * sy + v.z * cy;
	r.y = v.y;

	// PITCH 
	float cp = cosf(pitch);
	float sp = sinf(pitch);

	Vec3 r2;
	r2.x = r.x;
	r2.y = r.y * cp - r.z * sp;
	r2.z = r.y * sp + r.z * cp;

	return r2;
}
Vec3 GetForward(float yaw, float pitch) {
	Vec3 dir = { 0.0f, 0.0f, 1.0f }; // локальная ось Z
	float cy = cosf(yaw), sy = sinf(yaw);
	float cp = cosf(pitch), sp = sinf(pitch);

	Vec3 result;
	result.x = dir.x * cy + dir.z * sy; // <-- знак + вместо -
	result.z = -dir.x * sy + dir.z * cy; // <-- знак -
	result.y = dir.y;

	Vec3 final;
	final.x = result.x;
	final.y = result.y * cp + result.z * sp;        // pitch
	final.z = -result.y * sp + result.z * cp;

	// нормализация
	float len = sqrtf(final.x * final.x + final.y * final.y + final.z * final.z);
	final.x /= len;
	final.y /= len;
	final.z /= len;

	return final;
}
class Grad {
public:
	GradLauncher grad = { {0.0f,getHeight(0.0f,0.0f),0.0f},147.3f,-0.4f};
	bool volley = false;
	bool checkedVolley = false;

	void DrawBox(float w, float h, float d) {
		float x = w * 0.5f;
		float y = h * 0.5f;
		float z = d * 0.5f;

		glBegin(GL_QUADS);

		// FRONT
		glVertex3f(-x, -y, z);
		glVertex3f(x, -y, z);
		glVertex3f(x, y, z);
		glVertex3f(-x, y, z);

		// BACK
		glVertex3f(-x, -y, -z);
		glVertex3f(-x, y, -z);
		glVertex3f(x, y, -z);
		glVertex3f(x, -y, -z);

		// LEFT
		glVertex3f(-x, -y, -z);
		glVertex3f(-x, -y, z);
		glVertex3f(-x, y, z);
		glVertex3f(-x, y, -z);

		// RIGHT
		glVertex3f(x, -y, -z);
		glVertex3f(x, y, -z);
		glVertex3f(x, y, z);
		glVertex3f(x, -y, z);

		// TOP
		glVertex3f(-x, y, -z);
		glVertex3f(-x, y, z);
		glVertex3f(x, y, z);
		glVertex3f(x, y, -z);

		// BOTTOM
		glVertex3f(-x, -y, -z);
		glVertex3f(x, -y, -z);
		glVertex3f(x, -y, z);
		glVertex3f(-x, -y, z);

		glEnd();
	}
	void DrawSingleGuide() {
		DrawBox(0.05f, 0.05f, 1.0f);
	}
	void DrawGuides(const GradLauncher& grad) {
		glColor3f(0.1f, 0.8f, 0.1f);

		for (const Vec3& p : grad.localGuides) {
			glPushMatrix();

			glTranslatef(p.x, p.y, p.z);

			DrawSingleGuide();

			glPopMatrix();
		}
	}
	void DrawChassis(const Vec3& position) {
		glPushMatrix();

		float y = getHeight(position.x, position.z) + 0.15f;
		glTranslatef(position.x, y, position.z);

		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(3.5f, 0.3f, 1.2f);
		DrawBox(1, 1, 1);

		glPopMatrix();
	}
	void DrawWheel(const Vec3& position, float offsetX, float offsetZ) {
		glPushMatrix();

		float x = position.x + offsetX;
		float z = position.z + offsetZ;
		float y = getHeight(x, z) + 0.2f; // центр колеса над землёй

		glTranslatef(x, y, z);

		glColor3f(0.05f, 0.05f, 0.05f);
		DrawBox(0.4f, 0.4f, 0.2f);

		glPopMatrix();
	}

	void DrawWheels(const Vec3& position) {
		float zOffset = 0.7f;
		float xPos[] = { -1.2f, 0.0f, 1.2f };

		for (int i = 0; i < 3; i++) {
			DrawWheel(position, xPos[i], zOffset);   // левая
			DrawWheel(position, xPos[i], -zOffset);  // правая
		}
	}
	void DrawCabin(const Vec3& position) {
		glPushMatrix();

		float x = position.x + 1.5f;
		float z = position.z;
		float y = getHeight(x, z) + 0.4f; // немного выше шасси

		glTranslatef(x, y, z);

		glColor3f(0.2f, 0.2f, 0.8f);
		glScalef(1.0f, 0.8f, 1.0f);
		DrawBox(1, 1, 1);

		glPopMatrix();
	}
	void DrawLauncherPlatform(const GradLauncher& grad) {
		glPushMatrix();

		float x = grad.position.x;
		float z = grad.position.z;
		float y = getHeight(x, z);

		glTranslatef(x, y, z);

		glRotatef(grad.yaw * 57.2958f, 0, 1, 0);
		glRotatef(grad.pitch * 57.2958f, 1, 0, 0);

		glColor3f(0.3f, 0.3f, 0.3f);
		glScalef(0.0f, 0.0f, 1.0f);
		DrawBox(1, 1, 1);

		glPopMatrix();
	}
	void DrawLauncher(const GradLauncher& grad) {
		glPushMatrix();

		float x = grad.position.x - 1.2f;
		float z = grad.position.z;
		float y = getHeight(x, z) + 0.15f;

		glTranslatef(x, y, z);

		glRotatef(grad.yaw * 57.2958f, 0, 1, 0);
		glRotatef(grad.pitch * 57.2958f, 1, 0, 0);

		DrawLauncherPlatform(grad);

		glTranslatef(0.0f, 0.5f, 0.5f);
		DrawGuides(grad);

		glPopMatrix();
	}
	void draw(const GradLauncher& grad) {
		DrawChassis(grad.position);
		DrawWheels(grad.position);
		DrawCabin(grad.position);
		DrawLauncher(grad);
	}
	void InitGuides(GradLauncher& grad) {
		const int rows = 4;
		const int cols = 10;

		const float spacingX = 0.15f;
		const float spacingY = 0.15f;

		grad.localGuides.clear();

		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < cols; x++) {
				Vec3 p;

				p.x = (x - cols / 2.0f) * spacingX;
				p.y = (y - rows / 2.0f) * spacingY;
				p.z = 0.3f;

				grad.localGuides.push_back(p);
			}
		}

		grad.worldGuides.resize(grad.localGuides.size());
	}
	void spawRockets(std::vector<rocketPos>& rockets) {
		Vec3 dir = GetForward(grad.yaw, grad.pitch);
		float delayStep = 0.25f;

		int i = 0;
		for (const Vec3& p : grad.worldGuides) {
			rocketPos r;
			r.pos.x = p.x - 1.5f;
			r.pos.y = p.y;
			r.pos.z = p.z;
			r.nozzlePos = p;
			float angleSpread = 0.1f;
			float rx = ((rand() / (float)RAND_MAX) - 0.5f) * angleSpread;
			float ry = ((rand() / (float)RAND_MAX) - 0.5f) * angleSpread;

			Vec3 dirSpread = {dir.x + rx,dir.y + ry,dir.z};

			float len = sqrtf(dirSpread.x * dirSpread.x + dirSpread.y * dirSpread.y + dirSpread.z * dirSpread.z);
			
			dirSpread.x = dirSpread.x / len * speed;
			dirSpread.y = dirSpread.y / len * speed;
			dirSpread.z = dirSpread.z / len * speed;

			r.vel = dirSpread;

			r.spawnDelay = i * delayStep; 
			r.active = false;

			rockets.push_back(r);
			++i;
		}
	}
	void spawnRocketFire(std::vector<Particle>& particles, const rocketPos& r) {
		int count = 1;

		for (int i = 0; i < count; ++i) {
			Particle p1;
			Particle p2;

			p1.type = Type::smoke;
			p2.type = Type::fire;

			p1.pos = r.pos;

			p2.pos.x = r.nozzlePos.x - 1.5f;
			p2.pos.y = r.nozzlePos.y;
			p2.pos.z = r.nozzlePos.z;

			Vec3 back = { -r.vel.x, -r.vel.y, -r.vel.z };

			float len = sqrtf(back.x * back.x + back.y * back.y + back.z * back.z);
			back.x /= len;
			back.y /= len;
			back.z /= len;

			float spread = 0.2f;
			back.x += ((rand() / (float)RAND_MAX) - 0.5f) * spread;
			back.y += ((rand() / (float)RAND_MAX) - 0.5f) * spread;
			back.z += ((rand() / (float)RAND_MAX) - 0.5f) * spread;

			float speed = 5.0f;
			p1.vel = { back.x * speed, back.y * speed, back.z * speed };
			p2.vel = { back.x * speed, back.y * speed, back.z * speed };

			p1.life = 0.5f; 
			p2.life = 0.05f;

			particles.push_back(p1);
			particles.push_back(p2);
		}
	}
	void updateRockets(std::vector<Particle>& particles,std::vector<rocketPos>& rockets, float dt) {
		bool anyInactive = false;

		for (auto& r : rockets) {
			if (!r.active) {
				r.spawnTimer += dt;
				if (r.spawnTimer >= r.spawnDelay) r.active = true;
				else anyInactive = true;
			}

			if (r.active) {
				r.vel.y -= 9.81f * dt; 
				r.pos.x += r.vel.x * dt;
				r.pos.y += r.vel.y * dt;
				r.pos.z += r.vel.z * dt;
				spawnRocketFire(particles, r);
			}
		}

		volley = anyInactive; 
	}
	void updateParticles(std::vector<Particle>& particles, float dt) {
		for (auto& p : particles) {
			p.pos.x += p.vel.x * dt;
			p.pos.y += p.vel.y * dt;
			p.pos.z += p.vel.z * dt;

			p.life -= dt;
		}

		particles.erase(std::remove_if(particles.begin(), particles.end(),
				[](const Particle& p) { return p.life <= 0.0f; }),
			particles.end()
		);
	}
	void UpdateGuides() {
		Vec3 launcherOffset = { 0.0f, 0.5f, -0.5f };

		for (size_t i = 0; i < grad.localGuides.size(); i++) {
			Vec3 p = grad.localGuides[i];

			// сначала учитываем смещение установки
			Vec3 local = {
				p.x + launcherOffset.x,
				p.y + launcherOffset.y,
				p.z + launcherOffset.z
			};

			// потом вращаем
			Vec3 rotated = RotateYawPitch(local, grad.yaw, grad.pitch);

			// потом в мир
			grad.worldGuides[i] = {
				rotated.x + grad.position.x,
				rotated.y + grad.position.y,
				rotated.z + grad.position.z
			};
		}
	}
	void NormalizeAngles() {
		while (grad.yaw > 3.14159265f) grad.yaw -= 2.0f * 3.14159265f;
		while (grad.yaw < -3.14159265f) grad.yaw += 2.0f * 3.14159265f;
	}
};
