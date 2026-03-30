#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "text.h"
#include "getheight.h"
#include "environnement.h"
#include <string>
#include <format>

const float size = 300.0f;
bool gridTextEnadled = true;

void drawGridText(float camX, float camZ) {
    float baseX = camX;
    float baseZ = camZ;
    float step = 10.0f;

    for (float i = -size / 2; i <= size / 2; i += step) {
        for (float j = -size / 2; j <= size / 2; j += step) {
            float x = baseX + i;
            float z = baseZ + j;

            std::string text = std::format("({:.0f},{:.0f})", x, -z);
            float y = getHeight(x, z) + 1.5f;
            RenderTextWorld(x, y, z, 1.0f, 1.0f, 1.0f, text.c_str());
        }
    }
}
void drawGrid(float camX, float camZ) {
    float baseX = camX;
    float baseZ = camZ;
    float step = 20.0f;

    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_LINES);

    for (float i = -size; i <= size; i += step) {

        for (float j = -size; j <= size; j += step) {
            float x = baseX + i;
            float z = baseZ + j;

            float y = getHeight(x, z) + 0.2f;

            // линии по X
            if (j < size) {
                float z2 = baseZ + j + step;
                float y2 = getHeight(x, z2) + 0.2f;

                glVertex3f(x, y, z);
                glVertex3f(x, y2, z2);
            }

            // линии по Z
            if (i < size) {
                float x2 = baseX + i + step;
                float y2 = getHeight(x2, z) + 0.2f;

                glVertex3f(x, y, z);
                glVertex3f(x2, y2, z);
            }
        }
    }

    glEnd();
}
void drawSky() {
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(51.0f / 255, 151.0f / 255, 255.0f / 255);
    glVertex2f(-1, 1);
    glVertex2f(1, 1);

    glColor3f(51.0f / 255, 151.0f / 255, 255.0f / 255);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}
void drawGround(float camX, float camZ) {
    float x0 = camX;
    float z0 = camZ;

    float step = 2.0f;

    glColor3f(0.0f, 102.0f / 255, 0.0f);

    for (float x = -size; x < size; x += step) {
        for (float z = -size; z < size; z += step) {

            float x1 = x0 + x;
            float z1 = z0 + z;

            float x2 = x1 + step;
            float z2 = z1 + step;

            float y1 = getHeight(x1, z1);
            float y2 = getHeight(x2, z1);
            float y3 = getHeight(x2, z2);
            float y4 = getHeight(x1, z2);

            glBegin(GL_QUADS);

            // нормаль (упрощённая)
            glNormal3f(0, 1, 0);

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x2, y3, z2);
            glVertex3f(x1, y4, z2);

            glEnd();
        }
    }
}