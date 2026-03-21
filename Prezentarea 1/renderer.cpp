#include "renderer.h"
#include "utils.h"
#include "terrain.h"
#include "lighting.h"
#include "scene.h"
#include "bee.h"
#include "ladybug.h"

void drawAxis()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
    glEnd();

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void drawCrosshair()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);

    float cx = windowWidth * 0.5f;
    float cy = windowHeight * 0.5f;
    float size = 10.0f;
    float gap = 4.0f;

    glBegin(GL_LINES);
    // horizontal left
    glVertex2f(cx - size, cy);
    glVertex2f(cx - gap, cy);

    // horizontal right
    glVertex2f(cx + gap, cy);
    glVertex2f(cx + size, cy);

    // vertical bottom
    glVertex2f(cx, cy - size);
    glVertex2f(cx, cy - gap);

    // vertical top
    glVertex2f(cx, cy + gap);
    glVertex2f(cx, cy + size);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    updateMovement();
    updateLadybugs();

    // Third-person: camera always looks at the bee
    gluLookAt(
        camX, camY, camZ,
        beeX, beeY, beeZ,
        0.0f, 1.0f, 0.0f
    );

    setLighting();

    const float sceneHalf = 50.0f;
    const float sceneHeight = 35.0f;

    glColor3f(1.0f, 1.0f, 1.0f);

    drawFloor(sceneHalf);
    drawCeiling(sceneHalf, sceneHeight);
    drawWalls(sceneHalf, sceneHeight);
    drawMoonGlow(-45.0f, 31.8f, -45.0f);

    drawMoon(-45.0f, 31.8f, -45.0f, 9.0f);
    drawMoonRays(-45.0f, 31.8f, -44.8f);
    drawRelief();
    drawCircuit();
    drawPinkHouse(0.0f, 0.0f);
    float r = 34.0f;

    for (int i = 0; i < 9; i++)
    {
        float angle = i * 2.0f * M_PI / 9.0f;

        float x = cos(angle) * r;
        float z = sin(angle) * r;
        drawSakuraTree(x, z);
    }
    drawBee(beeX, beeY, beeZ, beeYaw);
    drawAllLadybugs();
    // drawAxis();
    drawLampPost(-20.0f, 0.0f);
    drawLampPost(20.0f, 0.0f);
    drawLampPost(0.0f, 20.0f);

    drawCrosshair();
    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 300.0);

    glMatrixMode(GL_MODELVIEW);
}

void init()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1.0f);    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_NORMALIZE);


    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    texGrass = loadTexture("grass.jpg");
    texSky = loadTexture("sky.jpg");
    texMountain = loadTexture("tree.jpg");
    texWater = loadTexture("water.jpg");
    texWood = loadTexture("wood.jpg");
    texLeaves = loadTexture("leaves.jpg");
    texMoon = loadTexture("moon.jpg");

    if (texGrass == 0 || texSky == 0 || texMountain == 0 || texWater == 0 ||
        texWood == 0 || texLeaves == 0 || texMoon == 0)
    {
        printf("Una sau mai multe texturi nu s-au incarcat.\n");
    }

    initLadybugs();
    glutSetCursor(GLUT_CURSOR_NONE);
    ignoreWarp = true;
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
}
