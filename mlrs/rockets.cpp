#pragma once
#include "vec3.h"
#include <vector>
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "rockets.h"

float speed = 40.0f;

float RandomSpread(float range) {
    return ((rand() / (float)RAND_MAX) - 0.5f) * range;
}
Vec3 computeImpact(const Vec3& launcherPos, float speed, float pitch, float yaw) {
    const float g = 9.81f;
    Vec3 dir;
    dir.x = cosf(pitch) * cosf(yaw);
    dir.y = sinf(pitch);
    dir.z = cosf(pitch) * sinf(yaw);

    Vec3 vel = { dir.x * speed, dir.y * speed, dir.z * speed };

    float a = -0.5f * g;
    float b = vel.y;
    float c = launcherPos.y;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0f) discriminant = 0.0f;

    float t = (-b + sqrtf(discriminant)) / (2 * a);

    Vec3 impact;
    impact.x = launcherPos.x + vel.z * t;
    impact.y = 0.0f;
    impact.z = launcherPos.z + vel.x * t;

    return impact;
}
void Rocket::DrawBox(float w, float h, float d) {
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
void Rocket::DrawRocket(const Vec3& position, const Vec3& direction) {
    glPushMatrix();

    // переводим в позицию ракеты
    glTranslatef(position.x, position.y, position.z);

    // вычисляем угол поворота для направления
    // направление предполагается нормализованным
    float pitch = asinf(-direction.y);              // угол наклона по X
    float yaw = atan2f(direction.x, direction.z); // угол поворота вокруг Y

    glRotatef(yaw * 57.2958f, 0, 1, 0);   // rad -> deg
    glRotatef(pitch * 57.2958f, 1, 0, 0);

    // рисуем тело ракеты
    glColor3f(0.8f, 0.1f, 0.1f);
    glScalef(0.05f, 0.05f, 0.6f); // толщина, радиус, длина
    DrawBox(1.0f, 1.0f, 1.0f);

    // опционально: добавить конус носа
    glTranslatef(0.0f, 0.0f, 0.5f); // перемещаемся к концу
    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.5f);

    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.5f);

    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.5f);

    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    glEnd();

    glPopMatrix();
}
