#include "scene.h"
#include "utils.h"
#include "terrain.h"

void drawSakuraTree(float x, float z)
{
    float groundY = terrainHeight(x, z) - 0.35f;
    glPushMatrix();
    glTranslatef(x, groundY, z);

    // ---------------- TRUNCHI ----------------
    glBindTexture(GL_TEXTURE_2D, texWood);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    drawTexturedCylinder(1.2f, 10.0f, 32);
    glPopMatrix();

    // ---------------- RAMURI PRINCIPALE ----------------
    glBindTexture(GL_TEXTURE_2D, texWood);

    glPushMatrix();
    glTranslatef(0.0f, 7.5f, 0.0f);
    glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
    drawTexturedCylinder(0.45f, 4.5f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 7.8f, 0.0f);
    glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
    drawTexturedCylinder(0.45f, 4.0f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 8.2f, 0.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    drawTexturedCylinder(0.4f, 3.8f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 8.0f, 0.0f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawTexturedCylinder(0.4f, 3.8f, 20);
    glPopMatrix();

    // ---------------- COROANA ----------------
    glBindTexture(GL_TEXTURE_2D, texLeaves);

    glPushMatrix();
    glTranslatef(0.0f, 12.0f, 0.0f);
    drawTexturedSphere(3.8f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.5f, 11.0f, 0.0f);
    drawTexturedSphere(2.6f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 11.2f, 0.3f);
    drawTexturedSphere(2.7f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 10.8f, 2.5f);
    drawTexturedSphere(2.5f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 10.6f, -2.5f);
    drawTexturedSphere(2.5f, 18, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.8f, 13.0f, 1.5f);
    drawTexturedSphere(2.1f, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.8f, 13.2f, -1.2f);
    drawTexturedSphere(2.1f, 16, 16);
    glPopMatrix();

    glPopMatrix();
}

void drawPinkHouse(float x, float z)
{
    const float w = 7.5f;
    const float d = 5.8f;
    const float h = 4.2f;
    const float roofH = 2.6f;

    float groundY = terrainHeight(x, z) - 0.55f;

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glScalef(1.8f, 1.8f, 1.8f);

    glDisable(GL_TEXTURE_2D);

    float skirt = 0.65f;

    // ---------------- BASE / FOUNDATION ----------------
    // brighter front, darker back/sides for stronger visual shading
    glColor3f(0.78f, 0.63f, 0.71f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glEnd();

    glColor3f(0.54f, 0.45f, 0.50f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glEnd();

    glColor3f(0.63f, 0.52f, 0.58f);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(-w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glEnd();

    glColor3f(0.68f, 0.56f, 0.62f);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, -skirt, d / 2.0f);
    glVertex3f(w / 2.0f, -skirt, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glEnd();

    // ---------------- WALLS ----------------
    glColor3f(0.96f, 0.76f, 0.86f); // front = brightest
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glEnd();

    glColor3f(0.66f, 0.54f, 0.62f); // back = darkest
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glEnd();

    glColor3f(0.78f, 0.63f, 0.72f); // left
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(-w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glEnd();

    glColor3f(0.86f, 0.68f, 0.78f); // right
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, 0.0f, d / 2.0f);
    glVertex3f(w / 2.0f, 0.0f, -d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glEnd();

    // ---------------- DOOR ----------------
    glColor3f(0.42f, 0.20f, 0.12f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.75f, 0.0f, d / 2.0f + 0.02f);
    glVertex3f(0.75f, 0.0f, d / 2.0f + 0.02f);
    glVertex3f(0.75f, 2.3f, d / 2.0f + 0.02f);
    glVertex3f(-0.75f, 2.3f, d / 2.0f + 0.02f);
    glEnd();

    // small brighter strip on the door
    glColor3f(0.55f, 0.28f, 0.16f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.68f, 0.15f, d / 2.0f + 0.03f);
    glVertex3f(-0.52f, 0.15f, d / 2.0f + 0.03f);
    glVertex3f(-0.52f, 2.15f, d / 2.0f + 0.03f);
    glVertex3f(-0.68f, 2.15f, d / 2.0f + 0.03f);
    glEnd();

    // ---------------- WINDOWS ----------------
    GLfloat windowEmission[] = { 0.18f, 0.18f, 0.22f, 1.0f };
    GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, windowEmission);
    glColor3f(0.78f, 0.88f, 0.95f);

    // front left
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-2.7f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(-1.5f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(-1.5f, 2.6f, d / 2.0f + 0.02f);
    glVertex3f(-2.7f, 2.6f, d / 2.0f + 0.02f);
    glEnd();

    // front right
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.5f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(2.7f, 1.4f, d / 2.0f + 0.02f);
    glVertex3f(2.7f, 2.6f, d / 2.0f + 0.02f);
    glVertex3f(1.5f, 2.6f, d / 2.0f + 0.02f);
    glEnd();

    // left window
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f - 0.02f, 1.3f, -1.0f);
    glVertex3f(-w / 2.0f - 0.02f, 1.3f, 0.6f);
    glVertex3f(-w / 2.0f - 0.02f, 2.5f, 0.6f);
    glVertex3f(-w / 2.0f - 0.02f, 2.5f, -1.0f);
    glEnd();

    // right window
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f + 0.02f, 1.3f, 0.6f);
    glVertex3f(w / 2.0f + 0.02f, 1.3f, -1.0f);
    glVertex3f(w / 2.0f + 0.02f, 2.5f, -1.0f);
    glVertex3f(w / 2.0f + 0.02f, 2.5f, 0.6f);
    glEnd();

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

    // ---------------- ROOF ----------------
    // front roof
    glColor3f(0.88f, 0.38f, 0.62f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.78f, 0.62f);
    glVertex3f(-w / 2.0f - 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(w / 2.0f + 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glEnd();

    // back roof
    glColor3f(0.56f, 0.24f, 0.40f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.78f, -0.62f);
    glVertex3f(w / 2.0f + 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(-w / 2.0f - 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glEnd();

    // left roof
    glColor3f(0.68f, 0.30f, 0.48f);
    glBegin(GL_QUADS);
    glNormal3f(-0.78f, 0.62f, 0.0f);
    glVertex3f(-w / 2.0f - 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(-w / 2.0f - 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glEnd();

    // right roof
    glColor3f(0.80f, 0.34f, 0.56f);
    glBegin(GL_QUADS);
    glNormal3f(0.78f, 0.62f, 0.0f);
    glVertex3f(w / 2.0f + 0.3f, h, d / 2.0f + 0.3f);
    glVertex3f(w / 2.0f + 0.3f, h, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, -d / 2.0f - 0.3f);
    glVertex3f(0.0f, h + roofH, d / 2.0f + 0.3f);
    glEnd();

    // side triangles
    glColor3f(0.90f, 0.74f, 0.82f);
    glBegin(GL_TRIANGLES);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w / 2.0f, h, -d / 2.0f);
    glVertex3f(-w / 2.0f, h, d / 2.0f);
    glVertex3f(-w / 2.0f, h + roofH, 0.0f);
    glEnd();

    glColor3f(0.82f, 0.66f, 0.74f);
    glBegin(GL_TRIANGLES);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w / 2.0f, h, d / 2.0f);
    glVertex3f(w / 2.0f, h, -d / 2.0f);
    glVertex3f(w / 2.0f, h + roofH, 0.0f);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}
