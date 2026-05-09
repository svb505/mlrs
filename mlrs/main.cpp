#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <random>
#include "camera.h"
#include "environnement.h"
#include "lightning.h"
#include "city.h"
#include "getheight.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "sounds.h"
#include "GUI.h"
#include "text.h"
#include "grad.h"
#include "rockets.h"
#include "effects.h"
#include "minimap.h"
#include "randomCoordinates.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define WW 1500
#define WH 900
#define TotalB 20

#include "texture.h"

Light light;
Camera camera;
Sound sound;
GUI gui;
Grad gradL;
Rocket rockt;
MiniMap mnMap;
bool spacePressedLastFrame = false;
int score = 0;
float damage = 0.1f;
float sumDestructions = 0.0f;
int rCount = 0;

void fpsCount(double& deltaTime, double& lastTime, float& fpsTimer, int& frames, float& fps) {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    fpsTimer += deltaTime;
    frames++;

    if (fpsTimer >= 1.0f) {
        fps = frames / fpsTimer;
        frames = 0;
        fpsTimer = 0.0f;
    }
}
void processInput(GLFWwindow* window,float dt){
    float camStep = 20.0f;
    float step = 1.0f;
    bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (spaceNow && !spacePressedLastFrame && !gradL.volley) {
        rCount += 40;
        sumDestructions = 0.0f;
        gradL.spawRockets(rockt.rockets);
        sound.setSourcePosition(sound.shootSource,0.0f,0.0f,0.0f);
        alSourceStop(sound.shootSource);
        alSourcePlay(sound.shootSource);
    }
    spacePressedLastFrame = spaceNow;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) cam.y += camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) cam.y -= camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) cam.x -= camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) cam.x += camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) cam.z -= camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) cam.z += camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) cam.yaw += camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cam.yaw -= camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cam.pitch -= camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cam.pitch += camStep * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !gradL.volley) {
        if (gradL.grad.pitch >= -1.1f) gradL.grad.pitch -= step * dt;
        gradL.UpdateGuides();
        gradL.NormalizeAngles();
        sound.setSourcePosition(sound.turretSource, 0.0f, 0.0f, 0.0f);
        alSourceStop(sound.turretSource);
        alSourcePlay(sound.turretSource);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !gradL.volley) {
        gradL.grad.yaw += step * dt;
        gradL.UpdateGuides();
        gradL.NormalizeAngles();
        sound.setSourcePosition(sound.turretSource, 0.0f, 0.0f, 0.0f);
        alSourceStop(sound.turretSource);
        alSourcePlay(sound.turretSource);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !gradL.volley) {
        if (gradL.grad.pitch <= -0.3f) gradL.grad.pitch += step * dt;
        gradL.UpdateGuides();
        gradL.NormalizeAngles();
        sound.setSourcePosition(sound.turretSource, 0.0f, 0.0f, 0.0f);
        alSourceStop(sound.turretSource);
        alSourcePlay(sound.turretSource);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !gradL.volley) {
        gradL.grad.yaw -= step * dt;
        gradL.UpdateGuides();
        gradL.NormalizeAngles();
        sound.setSourcePosition(sound.turretSource, 0.0f, 0.0f, 0.0f);
        alSourceStop(sound.turretSource);
        alSourcePlay(sound.turretSource);
    }
}
int main(){
    sound.setupBuffers();
    sound.createSources();

    if (!glfwInit()){
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(WW, WH, "MLRS", NULL, NULL);
    if (!window){
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    BuildFont();
    glViewport(0, 0, WW, WH);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(camera.left, camera.right, camera.bottom, camera.top, camera.nearPlane, camera.farPlane);

    glMatrixMode(GL_MODELVIEW);

    double lastTime = glfwGetTime();
    double deltaTime = 0.0;
    float fpsTimer = 0.0f;
    int frames = 0;
    float fps = 0.0f;

    light.initLighting();

    auto coords = randomCoordinates();

    City city({ coords[0], getHeight(coords[0],coords[1]) , coords[1] }, 20.0f, TotalB);
    city.generate(TotalB);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();

    gradL.InitGuides(gradL.grad);

    std::vector<ExplosionEffect*> explosions;
    std::vector<SmokeEffect*> smokes;
    std::vector<Particle> particles;

    LoadAllTextures();

    while (!glfwWindowShouldClose(window)){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.render(fps,gradL.grad,city,TotalB,score,damage,sumDestructions,rCount);

        ImGui::Render();

        fpsCount(deltaTime, lastTime, fpsTimer, frames, fps);

        float radYaw = cam.yaw * 3.14159265f / 180.0f;
        float radPitch = cam.pitch * 3.14159265f / 180.0f;
        float fx = cos(radPitch) * sin(radYaw);
        float fy = sin(radPitch);
        float fz = -cos(radPitch) * cos(radYaw);

        sound.setListener(cam.x, cam.y, cam.z, fx, fy, fz);

        processInput(window,deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        glLoadIdentity();
        glRotatef(-cam.pitch, 1, 0, 0);
        glRotatef(-cam.yaw, 0, 1, 0);

        drawSky();

        glPopMatrix();

        glLoadIdentity();
        glRotatef(-cam.pitch, 1, 0, 0);
        glRotatef(-cam.yaw, 0, 1, 0);
        glTranslatef(-cam.x, -cam.y, -cam.z + 3.0f);

        drawGround(cam.x, cam.z);
        drawGrid(cam.x, cam.z);

        city.draw();

        gradL.draw(gradL.grad);
        gradL.updateRockets(particles,rockt.rockets,deltaTime);
        gradL.updateParticles(particles, deltaTime);

        updateExplosions(deltaTime,explosions);
        updateSmokes(deltaTime, smokes);
        
        for (auto& p : particles) {
            glPointSize(4.0f);

            glBegin(GL_POINTS);
            for (const auto& p : particles) {
                float alpha = p.life;

                if (p.type == Type::smoke) glColor4f(128.0f / 255, 128.0f / 255, 128.0f / 255, alpha);
                else glColor4f(255.0f / 255, 128.0f / 255, 0, alpha);
                glVertex3f(p.pos.x, p.pos.y, p.pos.z);
            }
            glEnd();
        }
        for (auto& b : city.buildings) {
            if (b.destructionLevel >= 0.7f) b.destructionLevel = 1.0f;
        }
        for (auto& r : rockt.rockets) {
            Vec3 dir = GetForward(gradL.grad.yaw, gradL.grad.pitch);
            rockt.DrawRocket(r.pos,dir);

            if (r.pos.y <= 0.0f) {
                explosions.push_back(new ExplosionEffect(r.pos.x, r.pos.y, r.pos.z,100,4.0f,0.5f,1.0));
                smokes.push_back(new SmokeEffect(r.pos.x, r.pos.y, r.pos.z));
                sound.setSourcePosition(sound.explosionSource, r.pos.x, r.pos.y, r.pos.z);
                alSourceStop(sound.explosionSource);
                alSourcePlay(sound.explosionSource);
            }

            city.applyShot(r.pos,explosions,smokes,sound,score,damage);
        }
        for (auto it = rockt.rockets.begin(); it != rockt.rockets.end();) {
            if ((*it).pos.y <= 0.0f) it = rockt.rockets.erase(it);
            else ++it;
        }

        mnMap.draw(WW, WH, city, explosions, smokes, camera, deltaTime);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

