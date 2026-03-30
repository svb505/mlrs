#pragma once
#include <cmath>

struct CameraSettings {
    float x, y, z;
    float yaw;    // В ГРАДУСАХ
    float pitch;  // В ГРАДУСАХ
};

extern CameraSettings cam;

class Camera {
public:
    float fov = 70.0f;
    float aspect = 800.0f / 600.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float top = tan(fov * 3.1415926 / 360.0f) * nearPlane;
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
};
